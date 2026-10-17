#include <Arduino.h>

extern "C" {
#include "hardware.h"
}

// wrap arduino native millis into seconds
double seconds(void) {
  return millis() / 1000.0;
}

// basic H bridge
void set_wheels(double left, double right) {
  digitalWrite(22, left > 0);
  digitalWrite(23, left < 0);
  digitalWrite(24, right > 0);
  digitalWrite(25, right < 0);
  analogWrite(5, constrain((int)(fabs(left) * 255 / 6.28), 0, 255));
  analogWrite(6, constrain((int)(fabs(right) * 255 / 6.28), 0, 255));
}

// basic ir sensors
void read_ir(int values[8]) {
  for (int i = 0; i < 8; ++i) {
    values[i] = analogRead(A0 + i);
  }
}
