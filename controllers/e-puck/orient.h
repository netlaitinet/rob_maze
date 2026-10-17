#pragma once
#include <stdbool.h>
bool bootstrap(void);
void hug(void);

extern bool oriented;

/*
  80 > possible wall
  100 > sure wall
  250 > approaching collision, exponentially growing readings.
  1000 > practically collided.
  except non-straight angles 
*/
