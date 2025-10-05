#include <ESP32Servo.h>

static const int servoPin1 = 12;  // Pin where the servo is connected
static const int servoPin2 = 27;  // Pin where the servo is connected
static const int servoPin3 = 25;  // Pin where the servo is connected
static const int servoPin4 = 32;  // Pin where the servo is connected
static const int servoPin5 = 35;  // Pin where the servo is connected

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

void setup() {
    Serial.begin(115200);      // Initialize serial communication at 115200 baud rate
    servo1.setPeriodHertz(50);  // Set the frequency for the servo (50Hz typical)
    servo1.attach(servoPin1);
    servo2.setPeriodHertz(50);  // Set the frequency for the servo (50Hz typical)
    servo2.attach(servoPin2); 
    servo3.setPeriodHertz(50);  // Set the frequency for the servo (50Hz typical)
    servo3.attach(servoPin3); 
    servo4.setPeriodHertz(50);  // Set the frequency for the servo (50Hz typical)
    servo4.attach(servoPin4); 
    servo5.setPeriodHertz(50);
    servo5.attach(servoPin5); 
  // Set the frequency for the servo (50Hz typical)
}
void loop() {
  for(int pos = 500; pos <= 1000; pos+=20) {
        servo1.writeMicroseconds(pos);  // Set the pulse width in microseconds
        servo2.writeMicroseconds(pos);
        servo3.writeMicroseconds(pos);
        servo4.writeMicroseconds(pos);
        servo5.writeMicroseconds(pos);
        Serial.println(pos);            // Print the pulse width in microseconds
        delay(20);  
  }
    // Sweep the servo from 1000 µs (counterclockwise) to 2000 µs (counterclockwise)
    for(int pos = 1000; pos <= 2000; pos+=20) {
        servo1.writeMicroseconds(pos);  // Set the pulse width in microseconds
        servo2.writeMicroseconds(pos);
        servo3.writeMicroseconds(pos);
        servo4.writeMicroseconds(pos);
        servo5.writeMicroseconds(pos);
        Serial.println(pos);            // Print the pulse width in microseconds
        delay(20);                      // Wait for the servo to move
    }
    // Sweep the servo from 2000 µs (counterclockwise) to 2500 µs (clockwise)
    for(int pos = 2000; pos <= 2500; pos+=20) {
        servo1.writeMicroseconds(pos);
        servo2.writeMicroseconds(pos);
        servo3.writeMicroseconds(pos);
        servo4.writeMicroseconds(pos);  // Set the pulse width in microseconds
        servo5.writeMicroseconds(pos);
        Serial.println(pos);            // Print the pulse width in microseconds
        delay(20);                      // Wait for the servo to move
    }
    // Sweep the servo from 2500 µs (clockwise) back to 2000 µs (counterclockwise)
    for(int pos = 2500; pos >= 2000; pos-=20) {
        servo1.writeMicroseconds(pos);
        servo2.writeMicroseconds(pos);  // Set the pulse width in microseconds
        servo3.writeMicroseconds(pos);
        servo4.writeMicroseconds(pos);
        servo5.writeMicroseconds(pos);
        Serial.println(pos);            // Print the pulse width in microseconds
        delay(20);                      // Wait for the servo to move
    }
    // Sweep the servo from 2000 µs (counterclockwise) back to 1000 µs (counterclockwise)
    for(int pos = 2000; pos >= 1000; pos-=20) {
        servo1.writeMicroseconds(pos);  // Set the pulse width in microseconds
        servo2.writeMicroseconds(pos);
        servo3.writeMicroseconds(pos);
        servo4.writeMicroseconds(pos);
        servo5.writeMicroseconds(pos);
        Serial.println(pos);            // Print the pulse width in microseconds
        delay(20);           
    }
      for(int pos = 1000; pos >= 500; pos-=20) {
        servo1.writeMicroseconds(pos);  // Set the pulse width in microseconds
        servo2.writeMicroseconds(pos);
        servo3.writeMicroseconds(pos);
        servo4.writeMicroseconds(pos);
        servo5.writeMicroseconds(pos);
        Serial.println(pos);            // Print the pulse width in microseconds
        delay(20);             // Wait for the servo to move
    }
}