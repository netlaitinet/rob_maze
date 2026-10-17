#pragma once
#include <stdbool.h>

// these are margins at which to steer away from with increasing urgency the further it is from INTIMACY.
#define squeeze 50.0

#define BR_4 100
#define BR_3 200
#define BR_2 INTIMACY
#define B_1 INTIMACY+squeeze
#define BL_4 800
#define BL_3 500
#define BL_2 B_1

// fine tune variables
#define arr_smooth 1.5f

#define acceleration_lol 0.5f

#define arrive_delta 0.0225
#define depart_delta 0.05

extern bool smnv, vnms, sign;
extern double heading_stamp, boost;
extern unsigned int depart_ticks, arrive_ticks;
void strafe(void);
