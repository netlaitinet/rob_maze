#include "orient.h"
#include "tools.h"
#include "compass.h"
#include "strafe.h"
#include "history.h"
#include <stdlib.h>

int simplecounter;
int turnscounter; // during continuous hard turn 
int stopcounter; // not a turn anymore
int avoidance_buffer; // turn a little more to be sure


// u hook detection and whiplashing, also last measure against ir[6] related collision.
// does a little bit of simple math to lock on around the tip of the fence.
// the numbers are not perfect, they are just simple gradual reasoning steps towards the right behaviour until the robot hooks succesfully. then i left the numbers alone.
int detect_hook(int end){ 
  double l = historical_left_wheel(0);
    double r = historical_right_wheel(0);
    if ( r - l > 1.0 ) {
      turnscounter=(turnscounter+1)%end;
    }
    else {
      stopcounter++;
    }
  if (turnscounter < stopcounter * 5){ turnscounter = 0; stopcounter = 0; }

  if (baton.ir[6] > 150 || (baton.ir[5] > 400)) {
    avoidance_buffer = fmin(avoidance_buffer+1,3);
    if (baton.right > 0) {
    /* baton.right = baton.right * 0.25; */
    baton.right = fmin(baton.right,-1.0);
    turnscounter = 0;
    }
    } else { avoidance_buffer = fmax(avoidance_buffer-1,0);}

 if (avoidance_buffer > 0) {
    baton.right = fmin(baton.right,-1.0);
    }
 /* if (turnscounter > 50){ */
 /*   baton.left = baton.left + ((50.0 - (double)turnscounter)/2)/10.0; */
 /*     } */

  return turnscounter;
}

void smartwheels(){
  double o_L = historical_left_wheel(0)+6.28;
  double o_R = historical_right_wheel(0)+6.28;
  double b_L = baton.left+6.28;
  double b_R = baton.right+6.28;
  double Ld = b_L - o_L;
  double Rd = b_R - o_R;
  if ( Ld > 3.0) Ld = 3.0;
  if ( Ld < -3.0) Ld = -3.0;
  if ( Rd > 3.0) Rd = 3.0;
  if ( Rd < -3.0) Rd = -3.0;

  /* if (baton.warning == -4){ */
  /* baton.left = baton.left/2; */
  /* baton.right = baton.right/2; */
  /* } */

  baton.left = fmin(historical_left_wheel(0)+Ld,6.28);
  baton.right = fmin(historical_right_wheel(0)+Rd,6.28);

}

// UNUSED because the robot doesnt lose a wall ever anymore.
// is still good to have 
bool recover_wall(){
  if (baton.lost_heading > -1){
    if (simplecounter < 15){
      baton.right = 3.0;
      baton.left = -3.0;
      simplecounter++;
      return true;
    }
    if (simplecounter < 25){
      baton.right = 3.0;
      baton.left = 3.0;
      simplecounter++;
      return true;
    }
    if (compare_heading(0.035, baton.heading, baton.lost_heading)){
      baton.right = -3.0;
      baton.left = 3.0;
      return true;
    }
    baton.lost_heading = -1;
    simplecounter = 0;
  }
  return false;
}


// can deny and reset a strafing manouver from the outside.
void zero_strafe(){
  smnv = vnms = false;
  heading_stamp = baton.heading;
  baton.s_finetune = 1.0;
}

// answers whether h1 is d apart from h2
bool compare_heading(double d, double h1, double h2){
  double difference = fabs(h1 - h2);
  if (difference > 0.5) difference = 1.0 - difference;
  if (difference >= d){
    return true;
  }
  return false;
}

// can crudely detect matches against a trend
// its weakness is that the ir readings are exponentially stronger near a wall, so it always detects little biased towards a growing trend.
int detect_continuous_slope(int n, double margin){
  int rising = 0;
  int falling = 0;
  for (int i = 0; i < n; i++){
    double c = history_pick(i,5);
    double o = history_pick(i+1,5);
    double d = c - o;
    if (d > margin) rising++;
    if (d < -margin) falling++;
  }
  return rising-falling;
}

// orient the robots ir[5] snugly towards the left wall.
// does lock to own wheels, and causes a clamp attempt in compass.c, unless passed a bool false.
// the clamp can also be passed another bool to clamp once against a higher margin, but not currently used.
bool snug(bool should_clamp){
  double tmp = baton.ir[5];
  if (tmp > baton.snug_old){
    baton.snug_old = tmp;
    baton.left = 3.0;
    baton.right = -3.0;
    return false;
  } 
  baton.left = 0.0;
  baton.right = 0.0;
  baton.snug_old = 0.0;
  if (should_clamp) clamp(false);
  return true;
}


// guard common failures.
void emergency_avoidance(int dh){
  int bir5 = baton.ir[5];
  int bir6 = baton.ir[6];
  /* bool g_1a = (bir6 > 80); */
  bool g1 = (detect_continuous_slope(10, 0.3) > 3);
  bool g2 = (bir5 > 350);
  bool g3 = (bir6 > 170);
  bool g4 = (bir6 - history_pick(1, 6) < -30);
  /* bool g4 = (baton.ir[6] + baton.ir[5] > 500); */

  if (g1+g2+g3+g4 >= 2 && dh < 5){
    baton.left = baton.left + 1.0;
    baton.right = 0.0;
    }
  if (bir6 < 80 && dh > 60){
    baton.left = baton.left + 0.5;
  if (dh > 80) baton.left = 1.0;
  }
  }
