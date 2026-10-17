#include "history.h"
#include "baton.h"
int rb[32][8];
double historical_wheels_rb[32][2];

static int iterator;
// 0 = last tick;
int history_pick(int n, int m) {
  return rb[(iterator+31-n)%32][m];
}

// UNUSED helper
void history_peek(int n,int ir[8]) {
  for (int i = 0; i < 8; i++){
    ir[i] = history_pick(n, i);
  }
}
// this is always done last in a turn.
void history_push(void) {
  for (int i = 0; i < 8; i++){
    rb[iterator][i] = baton.ir[i];
  }
  historical_wheels_rb[iterator][0] = baton.left;
  historical_wheels_rb[iterator][1] = baton.right;
  iterator=(iterator+1)%32;
}

// 0 - the most recently pushed wheel state, practically last turns.
double historical_left_wheel(int n){
  return historical_wheels_rb[(iterator+31-n)%32][0];
}
double historical_right_wheel(int n){
  return historical_wheels_rb[(iterator+31-n)%32][1];
}

// detects whether ticks of n contain span of margin of wheel change.
// this was once very useful for clamping during fast strafing, but now UNUSED.
bool bumpy_ride(int n,double margin){ 
  double min=9999, max=0;
  for (int i = 0; i < n; i++){
    if (historical_right_wheel(i) > max) max = historical_right_wheel(i);
    if (historical_right_wheel(i) < min) min = historical_right_wheel(i);
    if (historical_left_wheel(i) > max) max = historical_left_wheel(i);
    if (historical_left_wheel(i) < min) min = historical_left_wheel(i);
  }
  if (baton.right > max) max = baton.right;
  if (baton.left > max) max = baton.left;
  if (baton.right < min) min = baton.right;
  if (baton.left < min) min = baton.left;
  if (max-min > margin) return true;
  return false;
}
