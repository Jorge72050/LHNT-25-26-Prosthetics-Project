#include "servo_control.h"
#include <Servo.h>

// Define the Servo objects (the actual memory)

// Example pins for Arduino Uno use
const int servoPin1 = 3;
const int servoPin2 = 5;
const int servoPin3 = 6;
const int servoPin4 = 9;
const int servoPin5 = 10;

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo tempServo;

void initializeServos() {
    Serial.begin(115200);
    servo1.attach(servoPin1);
    servo2.attach(servoPin2);
    servo3.attach(servoPin3);
    servo4.attach(servoPin4);
    servo5.attach(servoPin5);
}

void move_clockwise(int start_pos, int end_pos, int increment) {
    for(int pos = start_pos; pos <= end_pos; pos+=increment) {
        servo1.writeMicroseconds(pos);  // Set the pulse width in microseconds
        servo2.writeMicroseconds(pos);
        servo3.writeMicroseconds(pos);
        servo4.writeMicroseconds(pos);
        servo5.writeMicroseconds(pos);
        delay(20);   
    }
}

void move_counterclockwise(int start_pos, int end_pos, int increment) {
    for(int pos = start_pos; pos >= end_pos; pos-=increment) {
        servo1.writeMicroseconds(pos);  // Set the pulse width in microseconds
        servo2.writeMicroseconds(pos);
        servo3.writeMicroseconds(pos);
        servo4.writeMicroseconds(pos);
        servo5.writeMicroseconds(pos);
        delay(20);   
    }
}

void move_all_servos_to(int pos) {
    servo1.writeMicroseconds(pos);  // Set the pulse width in microseconds
    servo2.writeMicroseconds(pos);
    servo3.writeMicroseconds(pos);
    servo4.writeMicroseconds(pos);
    servo5.writeMicroseconds(pos);
}

void move_one_servo_to(int pin, int pos){
    tempServo.attach(pin);
    tempServo.writeMicroseconds(pos);
    delay(1000);
    tempServo.detach();
}

void move_one_clockwise(int pin, int start_pos, int end_pos, int increment){
    tempServo.attach(pin);
    for(int pos = start_pos; pos <= end_pos; pos += increment) {
        tempServo.writeMicroseconds(pos);
        delay(20);
    }
    tempServo.detach();
}

void move_one_counterclockwise(int pin, int start_pos, int end_pos, int increment){
    tempServo.attach(pin);
    for(int pos = start_pos; pos >= end_pos; pos -= increment) {
        tempServo.writeMicroseconds(pos);
        delay(20);
    }
    tempServo.detach();
}

