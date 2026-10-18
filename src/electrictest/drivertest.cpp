
#include <Arduino.h>
// Simplest DRV8833 test: motor A slowly speeds up, then restarts.

 
void setup() {
  ledcSetup(0, 20000, 8);
  ledcAttachPin(26, 0);        // PWM on AIN1
  pinMode(27, OUTPUT);
  digitalWrite(27, LOW);       // AIN2 held LOW
}
 
void loop() {
  for (int pwm = 0; pwm <= 255; pwm += 5) {
    ledcWrite(0, pwm);
    delay(100);
  }
}
 


/*
// Both motors speed up together, then restart.
//double
void setup() {
  ledcSetup(0, 20000, 8); ledcAttachPin(26, 0);   // motor A: PWM on AIN1
  ledcSetup(1, 20000, 8); ledcAttachPin(19, 1);   // motor B: PWM on BIN1
  pinMode(27, OUTPUT); digitalWrite(27, LOW);     // AIN2 low
  pinMode(21, OUTPUT); digitalWrite(21, LOW);     // BIN2 low
}
 
void loop() {
  for (int pwm = 0; pwm <= 255; pwm += 5) {
    ledcWrite(0, pwm);
    ledcWrite(1, pwm);
    delay(100);
  }
}
*/