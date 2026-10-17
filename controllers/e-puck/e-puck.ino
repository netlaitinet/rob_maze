extern "C" {
#include "hardware.h"
#include "history.h"
#include "orient.h"
#include "compass.h"
#include "baton.h"
#include "tools.h"
}

struct baton baton;

void setup(void) {
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(25, OUTPUT);
  set_wheels(3.0, 3.0);
}

void loop(void) {
  unsigned long start = millis();
  read_ir(baton.ir);
  compass(); XY_record(); // Integrate wheel effects from previous tick.

  if (!oriented) { // the robot has not found a wall yet
    if (bootstrap()) hug();
  } else {
    hug(); // normal behaviour hugging left wall.
  }

  if (baton.turn_until == 0){ // if a turn-in-place is not scheduled 

    int dh = detect_hook(100); // continuous hard turn duration counter.
    if (dh > 70) {
      baton.left = baton.left * 70.0f/(float)dh; // little math to wrap the robot around 180 fence like a whiplash.
    }

    emergency_avoidance(dh); // last check against common danger situations
    smartwheels(); // smoothes drastic changes
  }

  history_push(); // wheels have been chosen, push them to the history.
  set_wheels(baton.left, baton.right); // always last one to touch wheels

  // wait if tick takes less than 32ms
  unsigned long elapsed = millis() - start;
  if (elapsed < 32) {
    delay(32 - elapsed);
  }
}
