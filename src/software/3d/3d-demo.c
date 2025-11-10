#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define FDTD_IMPLEMENTATION
#include "fdtd.h"
#define NOB_IMPLEMENTATION
#include "../../../nob.h"

double ez_source_input(Grid *grid, SourceType type, SourceParameter param) {
  (void)type;
  double arg;
  arg = M_PI *
        ((grid->param.cdtds * param.time - param.location) / param.ppw - 1.0);
  arg *= arg;

  return (1.0 - 2.0 * arg) * exp(-arg);
}

void snapshotGrid3d(Grid *grid, Snapshot *snap) {
  int   mm, nn, pp;
  float v;
  char  filename[100];
  FILE *out;

  // nob_minimal_log_level = NOB_WARNING;
  if (!nob_mkdir_if_not_exists(snap->filename))
    return;
  // nob_minimal_log_level = NOB_INFO;

  if (!(grid->time >= snap->start_time &&
        (grid->time - snap->start_time) % snap->temporalStride == 0))
    return;

  pp = snap->slice;

  snprintf(filename, sizeof(filename), "%s/%s-z.%d", snap->filename,
           snap->basename, snap->frame++);
  out = fopen(filename, "wb");
  if (!out) {
    perror("fopen");
    return;
  }

  fwrite("FDTD", 1, 4, out);
  int32_t nx     = (int32_t)grid->param.sizeX;
  int32_t ny     = (int32_t)grid->param.sizeY;
  int32_t nz     = (int32_t)pp;
  float   time_f = (float)grid->time;

  fwrite(&nx, sizeof(int32_t), 1, out);
  fwrite(&ny, sizeof(int32_t), 1, out);
  fwrite(&nz, sizeof(int32_t), 1, out);
  fwrite(&time_f, sizeof(float), 1, out);

  for (mm = 0; mm < grid->param.sizeX; ++mm) {
    for (nn = 0; nn < grid->param.sizeY; ++nn) {
      v = (float)grid
              ->ez[IDX3(mm, nn, pp, grid->param.sizeY, grid->param.sizeZ - 1)];
      fwrite(&v, sizeof(float), 1, out);
    }
  }

  fclose(out);
  return;
}

void boundary_init_3d(Grid *grid, BoundaryType type, BoundaryParam3d *param) {
  (void)type;
  param->coef = (grid->param.cdtds - 1.0) / (grid->param.cdtds + 1.0);
  int SizeX   = grid->param.sizeX;
  int SizeY   = grid->param.sizeY;
  int SizeZ   = grid->param.sizeZ;

  CALLOC(param->eyx0, double, ((SizeY - 1) * (SizeZ)));
  CALLOC(param->ezx0, double, ((SizeY) * (SizeZ - 1)));
  CALLOC(param->eyx1, double, ((SizeY - 1) * (SizeZ)));
  CALLOC(param->ezx1, double, ((SizeY) * (SizeZ - 1)));

  CALLOC(param->exy0, double, ((SizeX - 1) * (SizeZ)));
  CALLOC(param->ezy0, double, ((SizeX) * (SizeZ - 1)));
  CALLOC(param->exy1, double, ((SizeX - 1) * (SizeZ)));
  CALLOC(param->ezy1, double, ((SizeX) * (SizeZ - 1)));

  CALLOC(param->exz0, double, ((SizeX - 1) * (SizeY)));
  CALLOC(param->eyz0, double, ((SizeX) * (SizeY - 1)));
  CALLOC(param->exz1, double, ((SizeX - 1) * (SizeY)));
  CALLOC(param->eyz1, double, ((SizeX) * (SizeY - 1)));

  return;
}

void boundary_abc_3d(Grid *grid, BoundaryParam3d *param) {
  int mm, nn, pp;
#define IDX(A, B, W) ((A) * (W) + (B))
  int SizeX = grid->param.sizeX;
  int SizeY = grid->param.sizeY;
  int SizeZ = grid->param.sizeZ;

  mm = 0;
  for (nn = 0; nn < SizeY - 1; nn++) {
    for (pp = 0; pp < SizeZ; pp++) {
      grid->ey[IDX3(mm, nn, pp, SizeY - 1, SizeZ)] =
          param->eyx0[IDX(nn, pp, SizeZ)] +
          param->coef * (grid->ey[IDX3(mm + 1, nn, pp, SizeY - 1, SizeZ)] -
                         grid->ey[IDX3(mm, nn, pp, SizeY - 1, SizeZ)]);
      param->eyx0[IDX(nn, pp, SizeZ)] =
          grid->ey[IDX3(mm + 1, nn, pp, SizeY - 1, SizeZ)];
    }
  }
  for (nn = 0; nn < SizeY; nn++) {
    for (pp = 0; pp < SizeZ - 1; pp++) {
      grid->ez[IDX3(mm, nn, pp, SizeY, SizeZ - 1)] =
          param->ezx0[IDX(nn, pp, SizeZ - 1)] +
          param->coef * (grid->ez[IDX3(mm + 1, nn, pp, SizeY, SizeZ - 1)] -
                         grid->ez[IDX3(mm, nn, pp, SizeY, SizeZ - 1)]);
      param->ezx0[IDX(nn, pp, SizeZ - 1)] =
          grid->ez[IDX3(mm + 1, nn, pp, SizeY, SizeZ - 1)];
    }
  }

  mm = SizeX - 1;
  for (nn = 0; nn < SizeY - 1; nn++) {
    for (pp = 0; pp < SizeZ; pp++) {
      grid->ey[IDX3(mm, nn, pp, SizeY - 1, SizeZ)] =
          param->eyx1[IDX(nn, pp, SizeZ)] +
          param->coef * (grid->ey[IDX3(mm - 1, nn, pp, SizeY - 1, SizeZ)] -
                         grid->ey[IDX3(mm, nn, pp, SizeY - 1, SizeZ)]);
      param->eyx1[IDX(nn, pp, SizeZ)] =
          grid->ey[IDX3(mm - 1, nn, pp, SizeY - 1, SizeZ)];
    }
  }
  for (nn = 0; nn < SizeY; nn++) {
    for (pp = 0; pp < SizeZ - 1; pp++) {
      grid->ez[IDX3(mm, nn, pp, SizeY, SizeZ - 1)] =
          param->ezx1[IDX(nn, pp, SizeZ - 1)] +
          param->coef * (grid->ez[IDX3(mm - 1, nn, pp, SizeY, SizeZ - 1)] -
                         grid->ez[IDX3(mm, nn, pp, SizeY, SizeZ - 1)]);
      param->ezx1[IDX(nn, pp, SizeZ - 1)] =
          grid->ez[IDX3(mm - 1, nn, pp, SizeY, SizeZ - 1)];
    }
  }

  nn = 0;
  for (mm = 0; mm < SizeX - 1; mm++) {
    for (pp = 0; pp < SizeZ; pp++) {
      grid->ex[IDX3(mm, nn, pp, SizeY, SizeZ)] =
          param->exy0[IDX(mm, pp, SizeZ)] +
          param->coef * (grid->ex[IDX3(mm, nn + 1, pp, SizeY, SizeZ)] -
                         grid->ex[IDX3(mm, nn, pp, SizeY, SizeZ)]);
      param->exy0[IDX(mm, pp, SizeZ)] =
          grid->ex[IDX3(mm, nn + 1, pp, SizeY, SizeZ)];
    }
  }
  for (mm = 0; mm < SizeX; mm++) {
    for (pp = 0; pp < SizeZ - 1; pp++) {
      grid->ez[IDX3(mm, nn, pp, SizeY, SizeZ - 1)] =
          param->ezy0[IDX(mm, pp, SizeZ - 1)] +
          param->coef * (grid->ez[IDX3(mm, nn + 1, pp, SizeY, SizeZ - 1)] -
                         grid->ez[IDX3(mm, nn, pp, SizeY, SizeZ - 1)]);
      param->ezy0[IDX(mm, pp, SizeZ - 1)] =
          grid->ez[IDX3(mm, nn + 1, pp, SizeY, SizeZ - 1)];
    }
  }

  nn = SizeY - 1;
  for (mm = 0; mm < SizeX - 1; mm++) {
    for (pp = 0; pp < SizeZ; pp++) {
      grid->ex[IDX3(mm, nn, pp, SizeY, SizeZ)] =
          param->exy1[IDX(mm, pp, SizeZ)] +
          param->coef * (grid->ex[IDX3(mm, nn - 1, pp, SizeY, SizeZ)] -
                         grid->ex[IDX3(mm, nn, pp, SizeY, SizeZ)]);
      param->exy1[IDX(mm, pp, SizeZ)] =
          grid->ex[IDX3(mm, nn - 1, pp, SizeY, SizeZ)];
    }
  }
  for (mm = 0; mm < SizeX; mm++) {
    for (pp = 0; pp < SizeZ - 1; pp++) {
      grid->ez[IDX3(mm, nn, pp, SizeY, SizeZ - 1)] =
          param->ezy1[IDX(mm, pp, SizeZ - 1)] +
          param->coef * (grid->ez[IDX3(mm, nn - 1, pp, SizeY, SizeZ - 1)] -
                         grid->ez[IDX3(mm, nn, pp, SizeY, SizeZ - 1)]);
      param->ezy1[IDX(mm, pp, SizeZ - 1)] =
          grid->ez[IDX3(mm, nn - 1, pp, SizeY, SizeZ - 1)];
    }
  }

  pp = 0;
  for (mm = 0; mm < SizeX - 1; mm++) {
    for (nn = 0; nn < SizeY; nn++) {
      grid->ex[IDX3(mm, nn, pp, SizeY, SizeZ)] =
          param->exz0[IDX(mm, nn, SizeY)] +
          param->coef * (grid->ex[IDX3(mm, nn, pp + 1, SizeY, SizeZ)] -
                         grid->ex[IDX3(mm, nn, pp, SizeY, SizeZ)]);
      param->exz0[IDX(mm, nn, SizeY)] =
          grid->ex[IDX3(mm, nn, pp + 1, SizeY, SizeZ)];
    }
  }
  for (mm = 0; mm < SizeX; mm++) {
    for (nn = 0; nn < SizeY - 1; nn++) {
      grid->ey[IDX3(mm, nn, pp, SizeY - 1, SizeZ)] =
          param->eyz0[IDX(mm, nn, SizeY - 1)] +
          param->coef * (grid->ey[IDX3(mm, nn, pp + 1, SizeY - 1, SizeZ)] -
                         grid->ey[IDX3(mm, nn, pp, SizeY - 1, SizeZ)]);
      param->eyz0[IDX(mm, nn, SizeY - 1)] =
          grid->ey[IDX3(mm, nn, pp + 1, SizeY - 1, SizeZ)];
    }
  }

  pp = SizeZ - 1;
  for (mm = 0; mm < SizeX - 1; mm++) {
    for (nn = 0; nn < SizeY; nn++) {
      grid->ex[IDX3(mm, nn, pp, SizeY - 1, SizeZ)] =
          param->exz1[IDX(mm, nn, SizeY)] +
          param->coef * (grid->ex[IDX3(mm, nn, pp - 1, SizeY - 1, SizeZ)] -
                         grid->ex[IDX3(mm, nn, pp, SizeY - 1, SizeZ)]);
      param->exz1[IDX(mm, nn, SizeY)] =
          grid->ex[IDX3(mm, nn, pp - 1, SizeY - 1, SizeZ)];
    }
  }
  for (mm = 0; mm < SizeX; mm++) {
    for (nn = 0; nn < SizeY - 1; nn++) {
      grid->ey[IDX3(mm, nn, pp, SizeY - 1, SizeZ)] =
          param->eyz1[IDX(mm, nn, SizeY - 1)] +
          param->coef * (grid->ey[IDX3(mm, nn, pp - 1, SizeY - 1, SizeZ)] -
                         grid->ey[IDX3(mm, nn, pp, SizeY - 1, SizeZ)]);
      param->eyz1[IDX(mm, nn, SizeY - 1)] =
          grid->ey[IDX3(mm, nn, pp - 1, SizeY - 1, SizeZ)];
    }
  }
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  Grid            *grid;
  BoundaryParam3d *p;

  CALLOC(grid, Grid, 1);
  CALLOC(p, BoundaryParam3d, 1);

  grid_init(grid, ThreeDimension,
            (GridParameter){
                .sizeX   = 32,
                .sizeY   = 31,
                .sizeZ   = 31,
                .maxTime = 300,
                .cdtds   = 1.0 / sqrt(3.0),
                .imp0    = 377.0,
            });

  boundary_init_3d(grid, ABC, p);

  for (grid->time = 0; grid->time < grid->param.maxTime; grid->time++) {
    printf("updateH\n");
    updateH(grid);
    printf("updateE\n");
    updateE(grid);
    printf("ez_source_input\n");
    grid->ex[IDX3((grid->param.sizeX - 1) / 2, grid->param.sizeY / 2,
                  grid->param.sizeZ / 2, grid->param.sizeY,
                  grid->param.sizeZ)] += ez_source_input(grid, RickerWavelet,
                                                         (SourceParameter){
                                                             .time = grid->time,
                                                             .location = 0.0,
                                                             .ppw      = 15,
                                                         });
    // printf("boundary_abc_3d\n");
    // boundary_abc_3d(grid, p);
    printf("snapshotGrid3d\n");
    snapshotGrid3d(grid, &(Snapshot){
                             .start_time     = 10,
                             .temporalStride = 10,
                             .slice          = (grid->param.sizeZ / 2),
                             .startX         = 0,
                             .endX           = (grid->param.sizeX - 1),
                             .spatialStrideX = 1,
                             .startY         = 0,
                             .endY           = (grid->param.sizeY - 1),
                             .spatialStrideY = 1,
                             .startZ         = 0,
                             .endZ           = (grid->param.sizeZ - 1),
                             .spatialStrideZ = 1,
                             .basename       = "sim",
                             .filename       = "3d-tfsf",
                         });
  }

  grid_free(grid);
  free(grid);

  return EXIT_SUCCESS;
}
