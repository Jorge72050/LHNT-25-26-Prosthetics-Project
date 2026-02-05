#include <Arduino.h>
#include "servo_control.h"
#include <ESP32Servo.h>
// Define the Servo objects (the actual memory)

const int servoPin1 = 7;

// DC Motor Pin
int pinENA = 12; 
int pinENB = 13; 
//int enable1Pin = 14; 

// DC Motor PWM Properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int speed = 0;

Servo servo1;

void initializeAll() {
    Serial.begin(115200);
    servo1.setPeriodHertz(50);
    servo1.attach(servoPin1);
}

void move_180_clockwise(int start_pos, int end_pos, int increment) {
    for(int pos = start_pos; pos <= end_pos; pos+=increment) {
        servo1.writeMicroseconds(pos);  // Set the pulse width in microseconds
        delay(20);   
    }
}

void move_180_counterclockwise(int start_pos, int end_pos, int increment) {
    for(int pos = start_pos; pos >= end_pos; pos-=increment) {
        servo1.writeMicroseconds(pos);  // Set the pulse width in microseconds
        delay(20);   
    }
}

void move_180_all_servos_to(int pos) {
    servo1.writeMicroseconds(pos);  // Set the pulse width in microseconds
}

/*void move_180_one_servo_to(int pin, int pos){
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
*/
Motor::Motor(uint8_t in1, uint8_t in2, uint8_t pwmPin)
{
    _in1 = in1;
    _in2 = in2;
    _pwmPin = pwmPin;
}

void Motor::begin(uint32_t freq, uint8_t resolution)
{
    _resolution = resolution;

    pinMode(_in1, OUTPUT);
    pinMode(_in2, OUTPUT);

    // ESP32 Core v3.x: ledcAttach replaces ledcSetup + ledcAttachPin
    // ledcAttach(_pwmPin, freq, resolution);
}

void Motor::setSpeed(int speed)
{
    int maxDuty = (1 << _resolution) - 1;

    if (speed > 0) {
        digitalWrite(_in1, HIGH);
        digitalWrite(_in2, LOW);
    }
    else if (speed < 0) {
        digitalWrite(_in1, LOW);
        digitalWrite(_in2, HIGH);
        speed = -speed;
    }
    else {
        stop();
        return;
    }

    if (speed > maxDuty) speed = maxDuty;

    // ESP32 v3.x: ledcWrite(pin, duty)
    ledcWrite(_pwmPin, speed);
}

void Motor::stop()
{
    digitalWrite(_in1, LOW);
    digitalWrite(_in2, LOW);
    ledcWrite(_pwmPin, 0);
}



