// Five-motor DRV8833 test: ramps each motor alone, then (optionally) all five together.
#include <Arduino.h>

const int PWM_PIN[5] = {26, 19, 25, 18, 16};   // IN1 of each motor (gets PWM)
const int LOW_PIN[5] = {27, 21, 23, 22, 17};   // IN2 of each motor (held LOW)
const bool RUN_ALL = false;                    // set true only after every single motor works

void rampMotors(int first, int last) {
  for (int pwm = 0; pwm <= 255; pwm += 5) {
    for (int i = first; i <= last; i++) ledcWrite(i, pwm);
    delay(60);
  }
  for (int i = first; i <= last; i++) ledcWrite(i, 0);
  delay(500);
}

void setup() {
  for (int i = 0; i < 5; i++) {
    ledcSetup(i, 20000, 8);
    ledcAttachPin(PWM_PIN[i], i);
    pinMode(LOW_PIN[i], OUTPUT);
    digitalWrite(LOW_PIN[i], LOW);
  }
}

void loop() {
  for (int i = 0; i < 5; i++) rampMotors(i, i);   // motor 1, 2, 3, 4, 5 in turn
  if (RUN_ALL) rampMotors(0, 4);
}
