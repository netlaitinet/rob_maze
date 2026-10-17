#pragma once
#include <stdbool.h>
#include <math.h>
#include "baton.h"

bool compare_heading(double d, double h1, double h2);
bool snug(bool should_clamp);
int detect_continuous_slope(int n, double margin);
void zero_strafe();
void pause_when_wall_lost(void);
bool recover_wall();
void smartwheels();
void emergency_avoidance(int dh);
int detect_hook(int end);
