#include "baton.h"
#include "tools.h"
#include "orient.h"
#include "hardware.h"
#include "history.h"
#include "moves.h"
#include "no_strafe.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
bool oriented = false;

// returns index of closest baton.ir[] sensor
// also clears and puts a fresh warning in baton to warn other functions for closing in wall or other error.
int closest_sensor(void) {
  int closest = 0; // closest value
  int cli = 0; // closest index
  for (int i = 0; i < 8; i++){
    int cur = baton.ir[i];
    if (cur > closest){
      closest=cur;
      cli=i;
    }
  }
  baton.warning = 0;
  if (closest <= 80) {
    baton.warning = -1; // empty
  }
  if (baton.ir[0] > 100 && baton.ir[7] > 100) baton.warning = -4; // predicted head-on
  if ((baton.ir[0]+baton.ir[7]) > 300 && baton.warning == -4) baton.warning = -3; // head-on
  return cli;
}

// the main navigation flow point.
void hug(void) {
  // after 180 turn, cookie helps pull back towards the wall.
    bool cookie = (history_pick(5, 5) < 150 && baton.ir[5] < 150);

  // re-alient in progress
  if (baton.turn_until != 0.0) {
    rehug(true);
    return;
  }
  if (baton.hooking){ hook_manouver(cookie);
    return;}
  // wall disappearance detection
  if (baton.ir[5] < 80.0 && history_pick(1, 5) > baton.ir[5] + 20.0){
    baton.hook_target = round(baton.heading * 4.0) / 4.0 - 0.25;
    baton.hooking = true;
    hook_manouver(cookie);
    return;
  }
  // trigger once to get warnings.
  closest_sensor();
  if (baton.warning == -3){ // straight head-on , hug the front wall instead.
    unhug();
  } else {
    no_strafe();
  }
}

//________________________________
// this needs to run before any wall has been achieved, if dropped in a weird orientation (like 45 degrees) in the maze, or got disoriented by some mistake. spares me from having to prep the robots position. It can find a wall by itself from any starting position.

// it has effect on the soft compass' immediate initial heading.
bool bootstrap(void) {
  if (baton.hugging == 5) return true;
    
  // where is the closest wall?
  int cs=closest_sensor();
  // this needs to always be able to catch the wall with [5], snug makes it then precise.
  if (baton.ir[5] > INTIMACY || (cs == 5 && (baton.ir[5] > INTIMACY-100))){
    if (snug(true)){
      oriented = true;
      baton.hugging = 5;
      return true;
    }
  // overshoot to ensure closeness.
  } else if (baton.ir[cs] > INTIMACY){
    // spin to align left.
    baton.left = 3.0;
    baton.right = -3.0;
  }
  return false;
}
