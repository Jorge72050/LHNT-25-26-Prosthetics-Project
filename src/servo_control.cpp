#include "servo_control.h"
#include <ESP32Servo.h>
#include <Arduino.h>

// Define the Servo objects (the actual memory)

const int servoPin1 = 7;
const int servoPin2 = 8;
const int servoPin3 = 9;
const int servoPin4 = 10;
const int servoPin5 = 11;
const int motorIN1 = 12;
const int motorIN2 = 13;
const int motorPWM = 14;


Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo tempServo;

void initializeAll() {
    Serial.begin(115200);
    servo1.setPeriodHertz(50);
    servo2.setPeriodHertz(50);
    servo3.setPeriodHertz(50);
    servo4.setPeriodHertz(50);
    servo5.setPeriodHertz(50);
    servo1.attach(servoPin1);
    servo2.attach(servoPin2);
    servo3.attach(servoPin3);
    servo4.attach(servoPin4);
    servo5.attach(servoPin5);
}

void move_180_clockwise(int start_pos, int end_pos, int increment) {
    for(int pos = start_pos; pos <= end_pos; pos+=increment) {
        servo1.writeMicroseconds(pos);  // Set the pulse width in microseconds
        servo2.writeMicroseconds(pos);
        servo3.writeMicroseconds(pos);
        servo4.writeMicroseconds(pos);
        servo5.writeMicroseconds(pos);
        delay(20);   
    }
}

void move_180_counterclockwise(int start_pos, int end_pos, int increment) {
    for(int pos = start_pos; pos >= end_pos; pos-=increment) {
        servo1.writeMicroseconds(pos);  // Set the pulse width in microseconds
        servo2.writeMicroseconds(pos);
        servo3.writeMicroseconds(pos);
        servo4.writeMicroseconds(pos);
        servo5.writeMicroseconds(pos);
        delay(20);   
    }
}

void move_180_all_servos_to(int pos) {
    servo1.writeMicroseconds(pos);  // Set the pulse width in microseconds
    servo2.writeMicroseconds(pos);
    servo3.writeMicroseconds(pos);
    servo4.writeMicroseconds(pos);
    servo5.writeMicroseconds(pos);
}

void move_180_one_servo_to(int pin, int pos){
    tempServo.attach(pin);
    tempServo.writeMicroseconds(pos);
    delay(1000);
    tempServo.detach();
}

void move_180_one_clockwise(int pin, int start_pos, int end_pos, int increment){
    tempServo.attach(pin);
    for(int pos = start_pos; pos <= end_pos; pos += increment) {
        tempServo.writeMicroseconds(pos);
        delay(20);
    }
    tempServo.detach();
}

void move_180_one_counterclockwise(int pin, int start_pos, int end_pos, int increment){
    tempServo.attach(pin);
    for(int pos = start_pos; pos >= end_pos; pos -= increment) {
        tempServo.writeMicroseconds(pos);
        delay(20);
    }
    tempServo.detach();
}

void move_360_servos_clockwise(int speed, int duration){
    if (speed > 90){
        move_360_servos_counterclockwise(speed, duration);
    }
    servo1.write(speed);
    servo2.write(speed);
    servo3.write(speed);
    servo4.write(speed);
    servo5.write(speed);
    Serial.println("clockwise");
    delay(duration);

    // 90 stops the servos and puts them at 0 speed
    servo1.write(90);
    servo2.write(90);
    servo3.write(90);
    servo4.write(90);
    servo5.write(90);
}
void move_360_servos_counterclockwise(int speed, int duration){
    if (speed < 90){
        move_360_servos_clockwise(speed, duration);
    }
    servo1.write(speed);
    servo2.write(speed);
    servo3.write(speed);
    servo4.write(speed);
    servo5.write(speed);
    delay(duration);
    
    // 90 stops the servos and puts them at 0 speed
    servo1.write(90);
    servo2.write(90);
    servo3.write(90);
    servo4.write(90);
    servo5.write(90);
}
void move_360_one_clockwise(int pin, int speed, int duration){
    if (speed > 90){
        move_360_one_counterclockwise(pin, speed, duration);
    }
    tempServo.attach(pin);
    tempServo.write(speed);
    delay(duration);
    tempServo.detach();
}
void move_360_one_counterclockwise(int pin, int speed, int duration){
    if (speed < 90){
        move_360_servos_clockwise(speed, duration);
    }
    tempServo.attach(pin);
    tempServo.write(speed);
    delay(duration);
    tempServo.detach();
}
    
void motor_init() {
    pinMode(motorIN1, OUTPUT);
    pinMode(motorIN2, OUTPUT);

    // ESP32 LEDC PWM setup
    ledcAttachPin(motorPWM, 0);   // Channel 0
    ledcSetup(0, 5000, 8);        // 5 kHz, 8-bit
      // 5kHz, 8-bit resolution

    motor_stop();
}

void motor_stop() {
    digitalWrite(motorIN1, LOW);
    digitalWrite(motorIN2, LOW);
    ledcWrite(0, 0);
}

void motor_clockwise(int speed) {
    digitalWrite(motorIN1, HIGH);
    digitalWrite(motorIN2, LOW);
    ledcWrite(0, speed);    // 0–255
}

void motor_counterclockwise(int speed) {
    digitalWrite(motorIN1, LOW);
    digitalWrite(motorIN2, HIGH);
    ledcWrite(0, speed);
}