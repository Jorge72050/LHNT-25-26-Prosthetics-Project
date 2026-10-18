#include <Arduino.h>

void stopAll();
void setMotor(int index, char direction, int speed);
void zeroEncoders();
void printTelemetryHeader();
void sendTelemetry();


// 5-motor control: direction + PWM speed via Serial Monitor (115200 baud, Newline)
// Commands: "1 F 200" (motor 1 forward, speed 200), "3 B 100", "2 S" (stop motor 2), "X" (stop all)
//           "Z" (zero all encoder counts), "T 50" (telemetry at 50 Hz, "T 0" = off), "H" (reprint header)
// Works on Arduino-ESP32 core 2.x AND 3.x (uses LEDC instead of analogWrite)
//
// Telemetry for 3D visualization: one line per sample, all 5 motors, starting with "D,".
// Lines starting with "#" describe the columns. Anything else is a human message and can be ignored.
//   D,<ms>, then per motor: <cmdDir F/B/S>,<cmdPWM 0-255>,<count>,<angleDeg>,<rpm>,<measDir 1/-1/0>
// angleDeg and rpm are for the gearbox OUTPUT shaft. rpm is signed (+ = forward).
// measDir comes from the encoder, so it is correct even if the finger is pushed by hand.
// See tools/motor_telemetry.py for a Python reader.

int NUM_MOTORS = 5;

// CHANGE: pin numbers (placeholders except motor 1). Position 0 = motor 1, etc.
// If a motor spins the wrong way, swap its forward and backward pins.
int forwardPins[5]  = {27, 14, 19, 32, 4};
int backwardPins[5] = {26, 16, 22, 33, 5};
int enablePins[5]   = {25, 17, 23, 13, 2};    // PWM pins

// CHANGE: encoder pins (C1/A and C2/B wires). -1 = encoder not wired yet (reports 0).
// Motor 1 matches motor_test.cpp. If count goes DOWN while driving F, swap that motor's A and B.
// Note: 5 motors x (3 drive + 2 encoder) = 25 GPIO, more than the ESP32 safely has free.
// Free safe pins left: 34, 35, 36, 39 (input only, good for encoders), plus boot-strapping pins 0, 12, 15.
int encoderAPins[5] = {18, -1, -1, -1, -1};
int encoderBPins[5] = {21, -1, -1, -1, -1};

// CHANGE: N20 encoder pulses per MOTOR revolution (per channel) and gearbox ratio.
// To check: send "Z", turn the output shaft exactly one turn by hand, read the count.
// That count should equal ENCODER_PPR * 4 * GEAR_RATIO.
const float ENCODER_PPR = 7.0;
const float GEAR_RATIO  = 150.0;
const float COUNTS_PER_REV = ENCODER_PPR * 4 * GEAR_RATIO;   // x4 = both edges of both channels

// CHANGE: speed limit (0-255)
int MAX_SPEED = 255;

// PWM settings
const int PWM_FREQ = 1000;      // 1 kHz
const int PWM_RES  = 8;         // 8-bit -> 0-255

// Telemetry
int telemetryHz = 20;
const float STOPPED_RPM = 0.5;  // below this |rpm| counts as not moving

// What each motor was last told to do (for telemetry)
char cmdDirections[5] = {'S', 'S', 'S', 'S', 'S'};
int  cmdSpeeds[5]     = {0, 0, 0, 0, 0};

// Encoder state (updated in interrupts)
volatile long encoderCounts[5] = {0, 0, 0, 0, 0};
volatile uint8_t encoderStates[5] = {0, 0, 0, 0, 0};
long lastCounts[5] = {0, 0, 0, 0, 0};
unsigned long lastSampleMicros = 0;

// ---- PWM helpers (handle core 2.x vs 3.x) ----
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  // Core 3.x: LEDC is attached per pin, write by pin
  void pwmSetup(int index) {
    ledcAttach(enablePins[index], PWM_FREQ, PWM_RES);
  }
  void pwmWrite(int index, int duty) {
    ledcWrite(enablePins[index], duty);
  }
#else
  // Core 2.x: LEDC uses channels (0-15), so use the motor index as the channel
  void pwmSetup(int index) {
    ledcSetup(index, PWM_FREQ, PWM_RES);
    ledcAttachPin(enablePins[index], index);
  }
  void pwmWrite(int index, int duty) {
    ledcWrite(index, duty);
  }
#endif

// ---- Encoder (quadrature, x4 decoding) ----
// Index = (previous AB << 2) | current AB. Valid one-step moves give +1/-1, glitches give 0.
const int8_t QUAD_STEPS[16] = {0, -1, 1, 0,  1, 0, 0, -1,  -1, 0, 0, 1,  0, 1, -1, 0};

uint8_t IRAM_ATTR readEncoderAB(int index) {
  return (digitalRead(encoderAPins[index]) << 1) | digitalRead(encoderBPins[index]);
}

void IRAM_ATTR onEncoderEdge(void* arg) {
  int index = (int)(intptr_t)arg;
  uint8_t state = readEncoderAB(index);
  encoderCounts[index] += QUAD_STEPS[(encoderStates[index] << 2) | state];
  encoderStates[index] = state;
}

void encoderSetup(int index) {
  if (encoderAPins[index] < 0 || encoderBPins[index] < 0) return;
  pinMode(encoderAPins[index], INPUT_PULLUP);   // pins 34-39 have no pullup: encoder board must provide it
  pinMode(encoderBPins[index], INPUT_PULLUP);
  encoderStates[index] = readEncoderAB(index);
  attachInterruptArg(encoderAPins[index], onEncoderEdge, (void*)(intptr_t)index, CHANGE);
  attachInterruptArg(encoderBPins[index], onEncoderEdge, (void*)(intptr_t)index, CHANGE);
}

// Warn if two signals share a pin (easy to do while the placeholders get filled in)
void checkPins() {
  int pins[25];
  int n = 0;
  for (int i = 0; i < NUM_MOTORS; i++) {
    pins[n++] = forwardPins[i];
    pins[n++] = backwardPins[i];
    pins[n++] = enablePins[i];
    pins[n++] = encoderAPins[i];
    pins[n++] = encoderBPins[i];
  }
  for (int a = 0; a < n; a++) {
    for (int b = a + 1; b < n; b++) {
      if (pins[a] >= 0 && pins[a] == pins[b]) {
        Serial.printf("WARNING: GPIO %d is assigned twice (motor %d and motor %d)\n",
                      pins[a], a / 5 + 1, b / 5 + 1);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  checkPins();

  for (int i = 0; i < NUM_MOTORS; i++) {
    pinMode(forwardPins[i], OUTPUT);
    pinMode(backwardPins[i], OUTPUT);
    pwmSetup(i);
    encoderSetup(i);
  }

  stopAll();
  zeroEncoders();
  Serial.println("Ready. Type commands like: 1 F 200");
  printTelemetryHeader();
}

void loop() {
  if (telemetryHz > 0 && micros() - lastSampleMicros >= 1000000UL / telemetryHz) {
    sendTelemetry();
  }

  if (Serial.available() > 0) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    line.toUpperCase();

    if (line == "X") {
      stopAll();
      Serial.println("All motors stopped");
      return;
    }
    if (line == "Z") {
      zeroEncoders();
      Serial.println("Encoders zeroed");
      return;
    }
    if (line == "H") {
      printTelemetryHeader();
      return;
    }
    int hz = 0;
    if (sscanf(line.c_str(), "T %d", &hz) == 1) {
      telemetryHz = constrain(hz, 0, 100);
      Serial.printf("Telemetry %d Hz\n", telemetryHz);
      return;
    }

    int motor = 0, speed = 0;
    char direction = 'S';
    int n = sscanf(line.c_str(), "%d %c %d", &motor, &direction, &speed);

    if (n >= 2 && motor >= 1 && motor <= NUM_MOTORS &&
        (direction == 'F' || direction == 'B' || direction == 'S')) {
      setMotor(motor - 1, direction, speed);
      Serial.printf("Motor %d %c %d\n", motor, direction, speed);
    } else {
      Serial.println("Bad command. Examples: 1 F 200, 2 S, X, Z, T 20");
    }
  }
}

// Drives any one motor. direction: 'F', 'B', or anything else = stop
void setMotor(int index, char direction, int speed) {
  if (speed < 0) speed = 0;
  if (speed > MAX_SPEED) speed = MAX_SPEED;

  if (direction == 'F') {
    digitalWrite(forwardPins[index], HIGH);
    digitalWrite(backwardPins[index], LOW);
    pwmWrite(index, speed);
  } else if (direction == 'B') {
    digitalWrite(forwardPins[index], LOW);
    digitalWrite(backwardPins[index], HIGH);
    pwmWrite(index, speed);
  } else {
    direction = 'S';
    speed = 0;
    digitalWrite(forwardPins[index], LOW);
    digitalWrite(backwardPins[index], LOW);
    pwmWrite(index, 0);
  }

  cmdDirections[index] = direction;
  cmdSpeeds[index] = speed;
}

void stopAll() {
  for (int i = 0; i < NUM_MOTORS; i++) {
    setMotor(i, 'S', 0);
  }
}

void zeroEncoders() {
  noInterrupts();
  for (int i = 0; i < NUM_MOTORS; i++) {
    encoderCounts[i] = 0;
    lastCounts[i] = 0;
  }
  interrupts();
}

void printTelemetryHeader() {
  Serial.printf("# counts_per_rev=%.1f hz=%d\n", COUNTS_PER_REV, telemetryHz);
  Serial.print("# D,ms");
  for (int i = 1; i <= NUM_MOTORS; i++) {
    Serial.printf(",m%d_cmd,m%d_pwm,m%d_count,m%d_deg,m%d_rpm,m%d_dir", i, i, i, i, i, i);
  }
  Serial.println();
}

void sendTelemetry() {
  unsigned long now = micros();
  float dtMinutes = (now - lastSampleMicros) / 60000000.0;
  lastSampleMicros = now;

  long counts[5];
  noInterrupts();
  for (int i = 0; i < NUM_MOTORS; i++) counts[i] = encoderCounts[i];
  interrupts();

  // Build the whole line first so it goes out in one piece
  char line[256];
  int len = snprintf(line, sizeof(line), "D,%lu", millis());

  for (int i = 0; i < NUM_MOTORS; i++) {
    float angleDeg = counts[i] * 360.0 / COUNTS_PER_REV;
    float rpm = (counts[i] - lastCounts[i]) / COUNTS_PER_REV / dtMinutes;
    int measDir = rpm > STOPPED_RPM ? 1 : (rpm < -STOPPED_RPM ? -1 : 0);
    lastCounts[i] = counts[i];

    len += snprintf(line + len, sizeof(line) - len, ",%c,%d,%ld,%.1f,%.1f,%d",
                    cmdDirections[i], cmdSpeeds[i], counts[i], angleDeg, rpm, measDir);
  }

  Serial.println(line);
}
