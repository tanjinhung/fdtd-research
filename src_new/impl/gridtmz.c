#include "../headers/fdtd.h"

/**
 * Initialize Grid
 */
void grid_init(Grid *g) {
  double imp0 = 377.0;
  int mm, nn;

  Type = twoDimensionalMagnetic;
  SizeX = 101;
  SizeY = 81;
  MaxTime = 300;
  Cdtds = 1.0 / sqrt(2.0);

  CALLOC_2D(g->hx, double, SizeX, SizeY - 1);
  CALLOC_2D(g->chxh, double, SizeX, SizeY - 1);
  CALLOC_2D(g->chxe, double, SizeX, SizeY - 1);

  CALLOC_2D(g->hy, double, SizeX - 1, SizeY);
  CALLOC_2D(g->chyh, double, SizeX - 1, SizeY);
  CALLOC_2D(g->chye, double, SizeX - 1, SizeY);

  CALLOC_2D(g->ez, double, SizeX, SizeY);
  CALLOC_2D(g->ceze, double, SizeX, SizeY);
  CALLOC_2D(g->cezh, double, SizeX, SizeY);

  /* Set Electric Field */
  for (mm = 0; mm < SizeX; mm++) {
    for (nn = 0; nn < SizeY; nn++) {
      Ceze(mm, nn) = 1.0;
      Cezh(mm, nn) = Cdtds * imp0;
    }
  }

  /* Set Magnetic Field */
  for (mm = 0; mm < SizeX; mm++) {
    for (nn = 0; nn < SizeY - 1; nn++) {
      Chxh(mm, nn) = 1.0;
      Chxe(mm, nn) = Cdtds / imp0;
    }
  }

  for (mm = 0; mm < SizeX - 1; mm++) {
    for (nn = 0; nn < SizeY; nn++) {
      Chyh(mm, nn) = 1.0;
      Chye(mm, nn) = Cdtds / imp0;
    }
  }

  return;
}
