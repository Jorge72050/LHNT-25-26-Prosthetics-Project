// basic sketch for now
#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Hello, new C++ file!");
}

void loop() {
    Serial.println("Still running...");
    delay(1000);
}
