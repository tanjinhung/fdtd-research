#include "../headers/fdtd.h"

static double cdtds, ppw = 0;

void ezIncInit(Grid *g) {
  printf("Enter the points per wavelength for Ricker Source: ");
  scanf(" %lf", &ppw);
  cdtds = Cdtds;
  return;
}

double ezInc(double time, double location) {
  double arg;

  if (ppw <= 0) {
    fprintf(stderr, "[ezInc] ezIncInit must be called before calling ezInc\n"
                    "        Points per wavelength must be > 0\n");
    exit(EXIT_FAILURE);
  }

  arg = M_PI * ((cdtds * time - location) / ppw - 1.0);
  arg = arg * arg;

  return (1.0 - 2.0 * arg) * exp(-arg);
}
