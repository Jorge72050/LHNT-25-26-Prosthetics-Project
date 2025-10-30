#include <ESP32Servo.h>
#include "servo_control.h"

void setup() {
    initializeServos();
}

void loop() {
    move_clockwise(500,2500,20);
    move_counterclockwise(2500,500,20);
}
