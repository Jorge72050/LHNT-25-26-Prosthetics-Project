// This file has general rotator functions
#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H
#include <Stepper.h>
#include <L298N.h>
#if defined(ESP32)
    #include <ESP32Servo.h>
#else
    #include <Servo.h>
#endif

// Servo Objects
extern Servo servo1;
extern Servo servo2;
extern Servo servo3;
extern Servo servo4;
extern Servo servo5;
extern Servo tempServo;

// All other objects
extern Stepper myStepper;

// Pin assignments (constants for servos)
extern const int servoPin1;
extern const int servoPin2;
extern const int servoPin3;
extern const int servoPin4;
extern const int servoPin5;

// All other pin assignments
extern const int solenoidPin;
extern const int StepperPin1;
extern const int StepperPin2;
extern const int StepperPin3;
extern const int StepperPin4;
extern const int stepsPerRevolution;

// Setup function — call this in setup()
void initializeAll();

/////////////////////////////////////////////////////////////////
// Movement helpers for 180 Servos
/////////////////////////////////////////////////////////////////

// Start is generally 500 ms, end is generally 2500 ms (full clockwise rotation),
// and increment is generall 20 (higher for higher speed and vice versa)
void move_180_clockwise(int start_pos, int end_pos, int increment);

// Same as move_clockwise, except reverse - start is 2500 ms, end is 500 ms
void move_180_counterclockwise(int start_pos, int end_pos, int increment);

// Move all servos to a given position
void move_180_all_servos_to(int pos);

void move_180_one_servo_to(int pin, int pos);
void move_180_one_clockwise(int pin, int start_pos, int end_pos, int increment);
void move_180_one_counterclockwise(int pin, int start_pos, int end_pos, int increment);


/////////////////////////////////////////////////////////////////
// Movement helpers for 360 servos
/////////////////////////////////////////////////////////////////
void move_360_servos_clockwise(int speed, int duration);
void move_360_servos_counterclockwise(int speed, int duration);
void move_360_one_clockwise(int pin, int speed, int duration);
void move_360_one_counterclockwise(int pin, int speed, int duration);

/////////////////////////////////////////////////////////////////
// Movement helpers for steppers, linear actuators, and DC Motors
/////////////////////////////////////////////////////////////////

void stepper_clockwise();
void stepper_counterclockwise();
void push_actuator();
void pull_actuator();

 /// Write dc motor functions here
 //  and implement in servo_control.cpp

#endif