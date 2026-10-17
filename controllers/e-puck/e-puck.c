#include "hardware.h"
#include "runtime.h"
#include "history.h"
#include "orient.h"
#include "compass.h"
#include "baton.h"
#include "tools.h"
#include <stdio.h>

struct baton baton = {.lost_heading = -1};

void loop(void) {
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
      baton.left = baton.left * 70.0f/(float)dh; // little math to wrap the robot around a thin obstacle.
  /* printf("%f          \n",baton.left ); */
    }

    emergency_avoidance(dh); // last check against common danger situations
    smartwheels(); // smoothes drastic changes
  }

  history_push(); // wheels have been chosen, push them to the history.
  set_wheels(baton.left, baton.right); // always last one to touch wheels
}
