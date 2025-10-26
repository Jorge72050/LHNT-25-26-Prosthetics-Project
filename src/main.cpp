#include <ESP32Servo.h>
#include "servo_control.h"

/*
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

*/

// Hello World
// Hello World
//Hello World
//Hello World

void setup() {
    initializeServos();
}

void loop() {
    move_clockwise(500,2500,20);
    move_counterclockwise(2500,500,20);
}
