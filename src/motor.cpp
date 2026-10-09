// 5-motor control: direction + PWM speed via Serial Monitor (9600 baud, Newline)
// Commands: "1 F 200" (motor 1 forward, speed 200), "3 B 100", "2 S" (stop motor 2), "X" (stop all)
// Works on Arduino-ESP32 core 2.x AND 3.x (uses LEDC instead of analogWrite)

int NUM_MOTORS = 5;

// CHANGE: pin numbers (placeholders except motor 1). Position 0 = motor 1, etc.
// If a motor spins the wrong way, swap its forward and backward pins.
int forwardPins[5]  = {27, 14, 19, 32, 4};
int backwardPins[5] = {26, 16, 22, 33, 18};
int enablePins[5]   = {25, 17, 23, 13, 21};   // PWM pins

// CHANGE: speed limit (0-255)
int MAX_SPEED = 255;

// PWM settings
const int PWM_FREQ = 1000;      // 1 kHz
const int PWM_RES  = 8;         // 8-bit -> 0-255

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

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < NUM_MOTORS; i++) {
    pinMode(forwardPins[i], OUTPUT);
    pinMode(backwardPins[i], OUTPUT);
    pwmSetup(i);
  }

  stopAll();
  Serial.println("Ready. Type commands like: 1 F 200");
}

void loop() {
  if (Serial.available() > 0) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    line.toUpperCase();

    if (line == "X") {
      stopAll();
      Serial.println("All motors stopped");
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
      Serial.println("Bad command. Examples: 1 F 200, 2 S, X");
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
    digitalWrite(forwardPins[index], LOW);
    digitalWrite(backwardPins[index], LOW);
    pwmWrite(index, 0);
  }
}

void stopAll() {
  for (int i = 0; i < NUM_MOTORS; i++) {
    setMotor(i, 'S', 0);
  }
}
