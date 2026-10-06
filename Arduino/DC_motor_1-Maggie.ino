// #include <Arduino.h>
#define EncoderPinA 18
#define EncoderPinB 21
#define ForwardPin 27
#define BackwardPin 26
#define EnablePin 25

volatile long Encodervalue = 0;
volatile int lastEncoded = 0;

enum MotorDirection { DIR_STOPPED = 0, DIR_FORWARD = 1, DIR_BACKWARD = -1 };
volatile MotorDirection currentDirection = DIR_STOPPED;

//Calculate speed and direction in ticks per second -> convert to RPM later if needed
long  lastEncoderValue = 0;
unsigned long lastSpeedCalcTime = 0;
const unsigned long SPEED_CALC_INTERVAL_MS = 100;
float currentTicksPerSecond = 0.0;

void IRAM_ATTR updateEncoder() {
  int MSB = digitalRead(EncoderPinA);
  int LSB = digitalRead(EncoderPinB);

  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    Encodervalue--;
  }
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    Encodervalue++;
  }

  lastEncoded = encoded;
}

void updateSpeedAndDirection() {
  unsigned long now = millis();
  if (now - lastSpeedCalcTime < SPEED_CALC_INTERVAL_MS) {
    return; 
  }

  noInterrupts();
  long snapshot = Encodervalue;
  interrupts();

  long delta = snapshot - lastEncoderValue;
  unsigned long elapsedMs = now - lastSpeedCalcTime;

  if (delta > 0) {
    currentDirection = DIR_FORWARD;
  } else if (delta < 0) {
    currentDirection = DIR_BACKWARD;
  } else {
    currentDirection = DIR_STOPPED;
  }

  // Speed in ticks per second (no unit conversion needed/assumed)
  float elapsedSeconds = elapsedMs / 1000.0;
  currentTicksPerSecond = (elapsedSeconds > 0) ? ((float)delta / elapsedSeconds) : 0.0;

  lastEncoderValue = snapshot;
  lastSpeedCalcTime = now;
}

MotorDirection getDirection() {
  return currentDirection;
}

float getTicksPerSecond() {
  return fabs(currentTicksPerSecond);
}

long getRawEncoderCount() {
  noInterrupts();
  long val = Encodervalue;
  interrupts();
  return val;
}

void printDebugReading() {
  Serial.print("Ticks: ");
  Serial.print(getRawEncoderCount());
  Serial.print("  |  Speed: ");
  Serial.print(getTicksPerSecond());
  Serial.print(" ticks/sec  |  Direction: ");
  switch (getDirection()) {
    case DIR_FORWARD:  Serial.println("FORWARD");  break;
    case DIR_BACKWARD: Serial.println("BACKWARD"); break;
    default:            Serial.println("STOPPED");  break;
  }
}

void setup() {
    pinMode(ForwardPin, OUTPUT);
    pinMode(BackwardPin,OUTPUT);
    pinMode(EnablePin, OUTPUT);
    pinMode(EncoderPinA, INPUT_PULLUP);
    pinMode(EncoderPinB, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(EncoderPinA), updateEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(EncoderPinB), updateEncoder, CHANGE);
    Serial.begin(9600);
}

void loop() {
    digitalWrite(ForwardPin,HIGH);
    digitalWrite(BackwardPin,LOW);
    analogWrite(EnablePin,255);
    updateSpeedAndDirection();
    printDebugReading();
    delay(100);
}
