#include "orient.h"
#include "moves.h"
#include "tools.h"
#include "strafe.h"
#include "baton.h"
#include "hardware.h"
#include "history.h"
#include <stdbool.h>
#include <math.h>

double INTIMACY = 250; // how close the robot wants to be to the wall it is hugging. measured in infrared value.

// hooking was suprisingly hard to implement right.
// it must finish the hook every time without knowing the exact entry position + whether the turn is 90 or 180 long.
// checks amount of turn done and possible ir data about a appearing wall.
// cookie is meant for ending the turn. It gives extra kick to finalize long 180 turns with a inwards hook.
// success also depends on some factors outside of the function. (see e-puck.c main loop)
void hook_manouver(bool cookie) {
  zero_strafe();
  double turn = baton.hook_target - baton.heading;
  turn -= round(turn);
  if (baton.ir[6] > 80 || fabs(turn) <= 0.01) {
    baton.hooking = false;
  if (cookie) {
    oriented = false;
  baton.left = 0.0;
  /* baton.right = baton.right+1.0; */
  baton.right = 6.28;
  }
    return;
  }
  baton.left = 0.5;
  baton.right = 3.0;

  for (int i = 0; i < 5; i++) {
  // lol stands for Lostness Of Ledge
  double lol = (baton.ir[5] - history_pick(i, 5));

  // lost
  if (lol < -100.0){
    /* baton.left = -(1.0-i/10.0f); */
    baton.left = baton.left-0.1;
  }
  // found
  if (lol > 100.0){
    baton.left = baton.left+3.0;
    }

  // extra absentness check ,UNUSED
  /* if (history_pick(i, 5) < 80){ */
  /*   baton.left = baton.left+0.25;; */
  /* } */
}
  }

// enough time to spin to prime the upcoming snug.
void unhug(void) {
  zero_strafe();
  baton.hugging = 0;
  baton.turn_until = seconds() + 2.980 / 5;
  baton.left = 2.1;
  baton.right = -2.1;
}

// keep trying until snug returns success, also pass in a bool should_clamp whether to clamp or prevent clamping deliberately.
// There are many possible situations to clamp in, but during rehug() seems the most straightforward one.
void rehug(bool should_clamp) {
  baton.left = 2.1;
  baton.right = -2.1;
  if (baton.turn_until <= seconds()) {
    if (snug(should_clamp)){
      baton.hugging = 5;
      baton.left = 0;
      baton.right = 0;
      baton.turn_until = 0.0;
    }
  }
}
