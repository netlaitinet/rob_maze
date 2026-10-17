#pragma once
#include <stdbool.h>
struct baton {
  double left, right;
  double heading;
  int hugging; // 0: none, 5: left 2: right
  int ir[8];
  int warning; // 0: none, -1: empty, -2: cramped, -3: head-on, -4: expect head-on
  double turn_until; // 0 = disabled
  double hook_target;
  bool hooking; // lock
  double snug_old; // lock
  double s_finetune;
  double X;
  double Y;
  double lost_heading; // -1 = disabled
  bool request_clamp;
};

extern struct baton baton;

