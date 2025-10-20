#include <stdlib.h>
#define FDTD_IMPLEMENTATION
#include "fdtd.h"
#define NOB_IMPLEMENTATION
#include "../nob.h"

double ez_source(Grid *grid, double time, double location, int ppw) {
  double arg;
  if (ppw <= 0) {
    fprintf(stderr, "ppw <= 0\n");
    exit(EXIT_FAILURE);
  }

  arg = M_PI * ((grid->param.cdtds * time - location) / ppw - 1.0);
  arg = arg * arg;

  return (1.0 - 2.0 * arg) * exp(-arg);
}

int main() {
  Grid *g;
  CALLOC(g, Grid, 1);
  if (!grid_init(g, TwoDimensionMagnetic,
                 (GridParameter){
                     .cdtds = 1.0 / sqrt(2.0),
                     .imp0 = 377.0,
                     .maxTime = 300,
                     .sizeX = 101,
                     .sizeY = 81,
                 }))
    return EXIT_FAILURE;

  Snapshot s = {
      .start_time = 10,
      .temporalStride = 10,
      .startX = 0,
      .endX = (g->param.sizeX - 1),
      .spatialStrideX = 1,
      .startY = 0,
      .endY = (g->param.sizeY - 1),
      .spatialStrideY = 1,
      .basename = "sim",
      .filename = "normal",
  };

  printf("-----------------------------------------------\n");
  printf("Start time: %d\t", s.start_time);
  printf("temporalStride: %d\n", s.temporalStride);
  printf("X:\t%d\t%d\t%d\n", s.startX, s.endX, s.spatialStrideX);
  printf("Y:\t%d\t%d\t%d\n", s.startY, s.endY, s.spatialStrideY);
  printf("path: %s/%s\n", s.filename, s.basename);
  printf("-----------------------------------------------\n");

  if (!nob_mkdir_if_not_exists(s.filename))
    return 1;

  for (g->time = 0; g->time < g->param.maxTime; g->time++) {
    updateH(g);
    updateE(g);
    int mm = g->param.sizeX / 2;
    int nn = g->param.sizeY / 2;
    g->ez[IDX2(mm, nn, g->param.sizeY)] = ez_source(g, g->time, 0.0, 20);
    snapshotGrid(g, &s);
  }

  return EXIT_SUCCESS;
}
