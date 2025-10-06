// This file has general rotator functions
#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H
#include <ESP32Servo.h>

extern Servo servo1;
extern Servo servo2;
extern Servo servo3;
extern Servo servo4;
extern Servo servo5;
extern Servo tempServo;

// Pin assignments (constants)
extern const int servoPin1;
extern const int servoPin2;
extern const int servoPin3;
extern const int servoPin4;
extern const int servoPin5;

// Setup function — call this in setup()
void initializeServos();

// Movement helpers

// Start is generally 500 ms, end is generally 2500 ms (full clockwise rotation),
// and increment is generall 20 (higher for higher speed and vice versa)
void move_clockwise(int start_pos, int end_pos, int increment);

// Same as move_clockwise, except reverse - start is 2500 ms, end is 500 ms
void move_counterclockwise(int start_pos, int end_pos, int increment);

// Move all servos to a given position
void move_all_servos_to(int pos);

void move_one_servo_to(int pin, int pos);
void move_one_clockwise(int pin, int start_pos, int end_pos, int increment);
void move_one_counterclockwise(int pin, int start_pos, int end_pos, int increment);

#endif