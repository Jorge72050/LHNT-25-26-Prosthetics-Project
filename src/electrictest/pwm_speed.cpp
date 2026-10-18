// Steps PWM up and prints encoder counts per second at each step.
// Motor A on L298N: ENA=25 IN1=26 IN2=27, encoder A=32 B=33
#include <Arduino.h>

const int EN = 25, IN1 = 26, IN2 = 27, ENC_A = 32, ENC_B = 33;
volatile long count = 0;

void IRAM_ATTR onA() { count += digitalRead(ENC_B) ? -1 : 1; }

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  attachInterrupt(ENC_A, onA, RISING);
  ledcSetup(0, 5000, 8);
  ledcAttachPin(EN, 0);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  for (int pwm = 0; pwm <= 255; pwm += 30) {
    ledcWrite(0, pwm);
    delay(500);                      // let speed settle
    long c0 = count;
    delay(500);
    Serial.printf("pwm=%d  counts/s=%ld\n", pwm, (count - c0) * 2);
  }
  ledcWrite(0, 0);
  Serial.println("done (press reset to run again)");
}

void loop() {}