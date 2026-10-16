/*  #include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
} */

#include <Arduino.h>

#define ENC_A 18
#define ENC_B 21
#define IN1   27   // ForwardPin
#define IN2   26   // BackwardPin
#define EN    25   // PWM / enable

volatile long encoderValue = 0;

void IRAM_ATTR updateEncoder() {
  if (digitalRead(ENC_B) == LOW) encoderValue++;
  else                           encoderValue--;
}

void pwmSetup() {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(EN, 20000, 8);        // pin, 20 kHz, 8-bit
#else
  ledcSetup(0, 20000, 8);
  ledcAttachPin(EN, 0);
#endif
}

void pwmWrite(int duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(EN, duty);
#else
  ledcWrite(0, duty);
#endif
}

void motor(int speed) {            // -255 to 255
  digitalWrite(IN1, speed > 0);
  digitalWrite(IN2, speed < 0);
  pwmWrite(abs(speed));
}

void runFor(const char* label, int speed, int ms) {
  Serial.println(label);
  motor(speed);
  unsigned long t = millis();
  while (millis() - t < (unsigned long)ms) {
    Serial.println(encoderValue);
    delay(200);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pwmSetup();
  attachInterrupt(digitalPinToInterrupt(ENC_A), updateEncoder, RISING);
}

void loop() {
  runFor("forward slow", 130, 2000);
  runFor("forward full", 255, 2000);
  runFor("stop",           0, 1000);
  runFor("reverse",     -170, 2000);
  runFor("stop",           0, 1000);
}