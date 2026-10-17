#include "baton.h"
#include <math.h>
#include <stdio.h>

double decay;

// quick simpler alternative to original strafe function.
// slower to avoid slipping

void no_strafe(void) {

  double measure = (baton.ir[5] - 250);

  if (measure > 0) {
    measure = sqrt(measure);
    decay-=.3;
  } else {
    decay+=.3;
  }

  decay = fmin(fmax(decay, -5.0), 5.0);

  measure = fmin(fmax(measure, -0.5), 0.5);

  baton.left = 1.0 + measure+(decay/11.0);
  baton.right = 1.0 - measure-(decay/11.0);
  /* printf("%f %f %f \n",decay,baton.left,baton.right); */
}
