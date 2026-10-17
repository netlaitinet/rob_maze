#pragma once

void hardware_init(void);

double seconds(void);

void set_wheels(double left, double right);

void read_ir(int values[8]);
