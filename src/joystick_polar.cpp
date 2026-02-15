#include "joystick_polar.h"
#include <math.h>

static int _pinX, _pinY; //
static int _x0 = 0, _y0 = 0;
static float _r = 0.0f;
static const float R_MIN = 800.0f; // ESP32 ADC is often 0..4095; tune this

void joystickPolarInit(int pinX, int pinY, int samples) {
  _pinX = pinX;
  _pinY = pinY;

  long sx = 0, sy = 0;
  for (int i = 0; i < samples; i++) {
    sx += analogRead(_pinX);
    sy += analogRead(_pinY);
    delay(2);
  }
  _x0 = (int)(sx / samples);
  _y0 = (int)(sy / samples);
}

bool joystickPolarActive() {
  return _r >= R_MIN;
}

float joystickPolarGetRadius() {
  return _r;
}

float joystickPolarGetCW01() {
  int xRaw = analogRead(_pinX);
  int yRaw = analogRead(_pinY);

  float dx = (float)(xRaw - _x0);
  float dy = (float)(yRaw - _y0);

  _r = sqrtf(dx*dx + dy*dy);

  if (_r < R_MIN) return 0.0f; // or keep last value if you prefer “hold”

  // atan2 is CCW by default: convert to [0,1], then flip for CW.
  float theta = atan2f(dy, dx);                 // [-pi, pi]
  float t_ccw = (theta + PI) / (2.0f * PI);     // [0, 1]
  float t_cw  = 1.0f - t_ccw;                   // [0, 1] clockwise increasing

  return t_cw;
}
