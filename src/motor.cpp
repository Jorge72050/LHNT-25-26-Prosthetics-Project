// 5-motor control: direction + PWM speed via Serial Monitor (9600 baud, Newline)
// Commands: "1 F 200" (motor 1 forward, speed 200), "3 B 100", "2 S" (stop motor 2), "X" (stop all)

int NUM_MOTORS = 5;

// CHANGE: pin numbers (placeholders except motor 1). Position 0 = motor 1, etc.
// If a motor spins the wrong way, swap its forward and backward pins.
int forwardPins[5]  = {27, 14, 19, 32, 4};
int backwardPins[5] = {26, 16, 22, 33, 5};
int enablePins[5]   = {25, 17, 23, 13, 15};   // must support PWM

// CHANGE: speed limit (0-255)
int MAX_SPEED = 255;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < NUM_MOTORS; i++) {
    pinMode(forwardPins[i], OUTPUT);
    pinMode(backwardPins[i], OUTPUT);
    pinMode(enablePins[i], OUTPUT);
  }

  stopAll();
  Serial.println("Ready. Type commands like: 1 F 200");
}

void loop() {
  if (Serial.available() > 0) {
    String line = Serial.readStringUntil('\n');
    line.trim();

    int motor = line.substring(0, 1).toInt();
    char direction = line.charAt(2);
    int speed = line.substring(4).toInt();

    if (line == "X") {
      stopAll();
      Serial.println("All motors stopped");
    } else if (motor >= 1 && motor <= NUM_MOTORS) {
      setMotor(motor - 1, direction, speed);
    } else {
      Serial.println("Bad command");
    }
  }
}

// Drives any one motor. direction: 'F', 'B', or anything else = stop
void setMotor(int index, char direction, int speed) {
  if (speed < 0) speed = 0;
  if (speed > MAX_SPEED) speed = MAX_SPEED;

  if (direction == 'F') {
    digitalWrite(forwardPins[index], HIGH);
    digitalWrite(backwardPins[index], LOW);
    analogWrite(enablePins[index], speed);
  } else if (direction == 'B') {
    digitalWrite(forwardPins[index], LOW);
    digitalWrite(backwardPins[index], HIGH);
    analogWrite(enablePins[index], speed);
  } else {
    digitalWrite(forwardPins[index], LOW);
    digitalWrite(backwardPins[index], LOW);
    analogWrite(enablePins[index], 0);
  }
}

void stopAll() {
  for (int i = 0; i < NUM_MOTORS; i++) {
    setMotor(i, 'S', 0);
  }
}
