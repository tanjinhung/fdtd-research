#include "./headers/fdtd.h"

int main() {
  Grid *g;

  CALLOC_1D(g, Grid, 1);

  grid_init(g);
  ezIncInit(g);
  snapshotInit2d(g);

  printf("Starting to simulate...\n");
  for (Time = 0; Time < MaxTime; Time++) {
    updateE2d(g);
    updateH2d(g);
    Ez(SizeX / 2, SizeY / 2) = ezInc(Time, 0.0);
    snapshot2d(g);
  }
  printf("End simulation\n");

  return EXIT_SUCCESS;
}
