// Spin the motor shaft by hand. Count should go up one way, down the other.
#include <Arduino.h>

const int ENC_A = 32, ENC_B = 33;
volatile long count = 0;

void IRAM_ATTR onA() { count += digitalRead(ENC_B) ? -1 : 1; }

void setup() {
  Serial.begin(115200);
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  attachInterrupt(ENC_A, onA, RISING);
}

void loop() {
  Serial.println(count);
  delay(200);
}