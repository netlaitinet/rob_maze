#pragma once
#include <stdbool.h>

void history_push(void);
void history_peek(int n,int ir[8]);
int history_pick(int n,int m);
double historical_left_wheel(int n);
double historical_right_wheel(int n);
bool bumpy_ride(int n,double margin);
