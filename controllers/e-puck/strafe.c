#include "compass.h"
#include "moves.h"
#include "baton.h"
#include "tools.h"
#include "hardware.h"
#include "history.h"
#include "strafe.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// by strafing, i mean traversing parallel to a surface with a small distance in between.
// this is just for fun
// It backs off immediately when in doubt, making it sometimes stutter a bit, but more robust for general unpredictable maze traversal without collision risk.


double heading_stamp; // can remember saved heading before a manouver
unsigned int depart_ticks, arrive_ticks; // keeps track of how long a move took.

bool smnv, vnms; // s_manouver and the same backwards to represent 2 states of ongoing and returning.

bool sign; // simply to save - or + direction.

double boost; // this increments slowly to accelerate the speed, and resets when problems are forecasted.

// save compass reading at start,
// s manouver like switching a lane.
// this function only modifies *state* by setting variables, and not *action*.
void s_mnv(){
  if (!smnv){
    heading_stamp = baton.heading;
    depart_ticks = arrive_ticks = 0;
    smnv=true;
  }
  if (!vnms){
    if (compare_heading((depart_delta*baton.s_finetune),baton.heading,heading_stamp)){
      // departed
      vnms=true;
    }
  } else {
    if (!compare_heading((arrive_delta*baton.s_finetune),baton.heading,heading_stamp)){
      // arrived
      smnv = false;
      vnms = false;
    }
  }}

// this makes choices using real-time and forecasted data together.
// it skips during manouver
// it starts a new manouver in certain direction according it its choice.
void speculate(int trend){
  if (smnv){
    s_mnv();
    return;
  }
  if (trend <= 0 && baton.ir[5] < B_1){
    // too right
    if (baton.ir[6] > 90){ // intentional >
      baton.snug_old = 0.0;
      baton.turn_until = seconds(); // Enter snug directly, without a timed turn.
      boost = 0.0;
      rehug(baton.request_clamp);
      return;
    }
    sign = 0;
    s_mnv();
  }
  if (trend > 2 && baton.ir[5] > B_1){
    // too left
    sign = 1;
    s_mnv();
  }
}

// applies incrementally growing boost when feeling safe. never when in a doubt.
// always applied as a transformation on top of previous wheel state.
void universalboost(){
  double brake_l = 1.0,brake_r = 1.0;
  double bir = baton.ir[5];
  double bdr = fmin(1+(fabs(bir - INTIMACY)/100.0),2.0);
  if (bir > INTIMACY) brake_r = bdr;
  if (bir < INTIMACY) brake_l = bdr;
  baton.left = baton.left+(boost/brake_l);
  baton.right = baton.right+(boost/brake_r);
  boost = fmin(boost+acceleration_lol,3.28);

  // general common failure cases
  if (bir < 150.0) {
    baton.right = fmin(fmax(baton.right-(detect_continuous_slope(10,0.5)),baton.left),6.28);
  }
  if (bir < 90.0) {
    baton.right = 6.28;
  }
}

// this actually reads the state and applies wheel effects to baton.
void signed_smnv(){
  baton.right = 3.0;
  baton.left = 3.0;
  if (vnms){
    arrive_ticks++;
    baton.left = 3.0 - (arr_smooth*sign);
    baton.right = 3.0-arr_smooth + (arr_smooth*sign);
    return;
  }
  if (smnv) {
    depart_ticks++;
    baton.right = 3.0 - (3.0f*sign);
    baton.left = 0.0 + (3.0f*sign);
  }
}

// all together
void strafe(){
  double bir = baton.ir[5];
  if (bir < BR_4 || baton.warning == -4) {
    boost = 0.0;
  }

  int trend = detect_continuous_slope(10, 5.0);
  baton.s_finetune = 0.8 + (double)trend/50.0;
  speculate(trend);
  if (baton.turn_until != 0.0) return;
  signed_smnv();

  if (boost < 3.28){
  printf("%f \n",boost);
  } else {
  printf(" \n");
  }
  universalboost();

  // general common failure cases
  if ((bir > BL_3 && trend > 6) || bir > BL_4){
    boost = 0.0;
    baton.right = 0.0;
    baton.left = 6.28;
  }
}
