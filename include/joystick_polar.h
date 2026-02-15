#pragma once
#include <Arduino.h>

// Call once in setup()
void joystickPolarInit(int pinX, int pinY, int samples = 200);

// Returns true if joystick is pushed out far enough to count as a “rotation gesture”
bool joystickPolarActive();

// Clockwise progress around the circle, 0.0 .. 1.0
// (0 = fully open, 1 = fully closed) IF you map it that way.
float joystickPolarGetCW01();

// Optional: joystick radius (how far from center)
float joystickPolarGetRadius();
