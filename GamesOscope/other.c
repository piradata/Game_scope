#include "Arduino.h"

  long random1(long howbig) {
    if (howbig == 0) { return 0; }
    return rand() % howbig;
  }

  long random2(long howsmall, long howbig) {
    if (howsmall >= howbig) { return howsmall; }
    return random1(howbig - howsmall) + howsmall;
  }

  void vector_rotate(double* x, double* y, double rads) {
    *x = *x * cos(rads) - *y * sin(rads);
    *y = *x * sin(rads) + *y * cos(rads);
  }

  void vector_scale(double* x, double* y, double multiplier) {
    *x *= multiplier;
    *y *= multiplier;
  }



