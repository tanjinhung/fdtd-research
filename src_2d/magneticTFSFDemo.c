#include <stdlib.h>
#include <string.h>
#define FDTD_IMPLEMENTATION
#include "fdtd.h"
#define NOB_IMPLEMENTATION
#include "../nob.h"

double ez_source(Grid *grid, double time, double location, int ppw) {
  double arg;
  arg = M_PI * ((grid->param.cdtds * time - location) / ppw - 1.0);
  arg = arg * arg;

  return (1.0 - 2.0 * arg) * exp(-arg);
}

void boundary_init(Grid *grid, BoundaryType type, BoundaryParam *param) {
  (void)type;
  CALLOC(param->ezLeft, double, grid->param.sizeY * 6);
  CALLOC(param->ezRight, double, grid->param.sizeY * 6);
  CALLOC(param->ezTop, double, grid->param.sizeX * 6);
  CALLOC(param->ezBottom, double, grid->param.sizeX * 6);

  double temp1 = sqrt(grid->cezh[0] * grid->chye[0]);
  double temp2 = 1.0 / temp1 + 2.0 + temp1;
  param->coef0 = -(1.0 / temp1 - 2.0 + temp1) / temp2;
  param->coef1 = -2.0 * (temp1 - 1.0 / temp1) / temp2;
  param->coef2 = 4.0 * (temp1 + 1.0 / temp1) / temp2;

  return;
}

void boundary_abc(Grid *grid, BoundaryParam *param) {
  int mm, nn;

#define SIZEY grid->param.sizeY
#define SIZEX grid->param.sizeX

#define IDX_ABC_LR(m, q, n) (((n) * 6) + ((q) * 3) + (m))
#define IDX_ABC_TB(m, q, n) (((m) * 6) + ((q) * 3) + (n))

  /* left */
  for (nn = 0; nn < SIZEY; nn++) {
    grid->ez[IDX2(0, nn, SIZEY)] =
        param->coef0 * (grid->ez[IDX2(2, nn, SIZEY)] +
                        param->ezLeft[IDX_ABC_LR(0, 1, nn)]) +
        param->coef1 * (param->ezLeft[IDX_ABC_LR(0, 0, nn)] +
                        param->ezLeft[IDX_ABC_LR(2, 0, nn)] -
                        grid->ez[IDX2(1, nn, SIZEY)] -
                        param->ezLeft[IDX_ABC_LR(1, 1, nn)]) +
        param->coef2 * param->ezLeft[IDX_ABC_LR(1, 0, nn)] -
        param->ezLeft[IDX_ABC_LR(2, 1, nn)];

    for (mm = 0; mm < 3; mm++) {
      param->ezLeft[IDX_ABC_LR(mm, 1, nn)] =
          param->ezLeft[IDX_ABC_LR(mm, 0, nn)];
      param->ezLeft[IDX_ABC_LR(mm, 0, nn)] = grid->ez[IDX2(mm, nn, SIZEY)];
    }
  }

  /* right */
  for (nn = 0; nn < SIZEY; nn++) {
    grid->ez[IDX2(SIZEX - 1, nn, SIZEY)] =
        param->coef0 * (grid->ez[IDX2(SIZEX - 3, nn, SIZEY)] +
                        param->ezRight[IDX_ABC_LR(0, 1, nn)]) +
        param->coef1 * (param->ezRight[IDX_ABC_LR(0, 0, nn)] +
                        param->ezRight[IDX_ABC_LR(2, 0, nn)] -
                        grid->ez[IDX2(SIZEX - 2, nn, SIZEY)] -
                        param->ezRight[IDX_ABC_LR(1, 1, nn)]) +
        param->coef2 * param->ezRight[IDX_ABC_LR(1, 0, nn)] -
        param->ezRight[IDX_ABC_LR(2, 1, nn)];

    for (mm = 0; mm < 3; mm++) {
      param->ezRight[IDX_ABC_LR(mm, 1, nn)] =
          param->ezRight[IDX_ABC_LR(mm, 0, nn)];
      param->ezRight[IDX_ABC_LR(mm, 0, nn)] =
          grid->ez[IDX2(SIZEX - 1 - mm, nn, SIZEY)];
    }
  }

  /* bottom */
  for (mm = 0; mm < SIZEX; mm++) {
    grid->ez[IDX2(mm, 0, SIZEY)] =
        param->coef0 * (grid->ez[IDX2(mm, 2, SIZEY)] +
                        param->ezBottom[IDX_ABC_TB(mm, 1, 0)]) +
        param->coef1 * (param->ezBottom[IDX_ABC_TB(mm, 0, 0)] +
                        param->ezBottom[IDX_ABC_TB(mm, 0, 2)] -
                        grid->ez[IDX2(mm, 1, SIZEY)] -
                        param->ezBottom[IDX_ABC_TB(mm, 1, 1)]) +
        param->coef2 * param->ezBottom[IDX_ABC_TB(mm, 0, 1)] -
        param->ezBottom[IDX_ABC_TB(mm, 1, 2)];

    for (nn = 0; nn < 3; nn++) {
      param->ezBottom[IDX_ABC_TB(mm, 1, nn)] =
          param->ezBottom[IDX_ABC_TB(mm, 0, nn)];
      param->ezBottom[IDX_ABC_TB(mm, 0, nn)] = grid->ez[IDX2(mm, nn, SIZEY)];
    }
  }

  /* top */
  for (mm = 0; mm < SIZEX; mm++) {
    grid->ez[IDX2(mm, SIZEY - 1, SIZEY)] =
        param->coef0 * (grid->ez[IDX2(mm, SIZEY - 3, SIZEY)] +
                        param->ezTop[IDX_ABC_TB(mm, 1, 0)]) +
        param->coef1 * (param->ezTop[IDX_ABC_TB(mm, 0, 0)] +
                        param->ezTop[IDX_ABC_TB(mm, 0, 2)] -
                        grid->ez[IDX2(mm, SIZEY - 2, SIZEY)] -
                        param->ezTop[IDX_ABC_TB(mm, 1, 1)]) +
        param->coef2 * param->ezTop[IDX_ABC_TB(mm, 0, 1)] -
        param->ezTop[IDX_ABC_TB(mm, 1, 2)];

    for (nn = 0; nn < 3; nn++) {
      param->ezTop[IDX_ABC_TB(mm, 1, nn)] = param->ezTop[IDX_ABC_TB(mm, 0, nn)];
      param->ezTop[IDX_ABC_TB(mm, 0, nn)] =
          grid->ez[IDX2(mm, SIZEY - 1 - nn, SIZEY)];
    }
  }

#undef IDX_ABC_LR
#undef IDX_ABC_TB
#undef SIZEY
#undef SIZEX

  return;
}

bool grid_init_lossy(Grid *grid, int nLoss, float maxLoss) {
  double depthInLayer, lossFactor;
  int mm;

  grid->param.sizeX += nLoss;
  grid->type = OneDimension;

  CALLOC(grid->hy, double, grid->param.sizeX - 1);
  CALLOC(grid->chyh, double, grid->param.sizeX - 1);
  CALLOC(grid->chye, double, grid->param.sizeX - 1);
  CALLOC(grid->ez, double, grid->param.sizeX);
  CALLOC(grid->ceze, double, grid->param.sizeX);
  CALLOC(grid->cezh, double, grid->param.sizeX);

  for (mm = 0; mm < grid->param.sizeX - 1; mm++) {
    if (mm < grid->param.sizeX - 1 - nLoss) {
      grid->ceze[mm] = 1.0;
      grid->cezh[mm] = grid->param.cdtds * grid->param.imp0;
      grid->chyh[mm] = 1.0;
      grid->chye[mm] = grid->param.cdtds / grid->param.imp0;
    } else {
      depthInLayer = mm - (grid->param.sizeX - 1 - nLoss) + 0.5;
      lossFactor = maxLoss * pow(depthInLayer / nLoss, 2);
      grid->ceze[mm] = (1.0 - lossFactor) / (1.0 + lossFactor);
      grid->cezh[mm] =
          grid->param.cdtds * grid->param.imp0 / (1.0 + lossFactor);
      depthInLayer += 0.5;
      lossFactor = maxLoss * pow(depthInLayer / nLoss, 2);

      grid->chyh[mm] = (1.0 - lossFactor) / (1.0 + lossFactor);
      grid->chye[mm] =
          grid->param.cdtds / grid->param.imp0 / (1.0 + lossFactor);
    }
  }

  return true;
}

void tfsfUpdate(Grid *grid, Grid *g_sub, tfsfRectangle rect) {
  int mm, nn;

#define SIZEY grid->param.sizeY
#define SIZEX grid->param.sizeX

  mm = rect.first.x - 1;
  for (nn = rect.first.y; nn <= rect.last.y; nn++)
    grid->hy[IDX2(mm, nn, SIZEY)] -=
        grid->chye[IDX2(mm, nn, SIZEY)] * g_sub->ez[mm + 1];

  mm = rect.last.x;
  for (nn = rect.first.y; nn <= rect.last.y; nn++)
    grid->hy[IDX2(mm, nn, SIZEY)] +=
        grid->chye[IDX2(mm, nn, SIZEY)] * g_sub->ez[mm];

  nn = rect.first.y - 1;
  for (mm = rect.first.x; mm <= rect.last.x; mm++)
    grid->hx[IDX2(mm, nn, SIZEY - 1)] +=
        grid->chxe[IDX2(mm, nn, SIZEY - 1)] * g_sub->ez[mm];

  nn = rect.last.y;
  for (mm = rect.first.x; mm <= rect.last.x; mm++)
    grid->hx[IDX2(mm, nn, SIZEY - 1)] -=
        grid->chxe[IDX2(mm, nn, SIZEY - 1)] * g_sub->ez[mm];

  updateH(g_sub);
  updateE(g_sub);

  g_sub->ez[0] = ez_source(grid, g_sub->time, 0.0, 20);
  g_sub->time++;

  mm = rect.first.x;
  for (nn = rect.first.y; nn <= rect.last.y; nn++)
    grid->ez[IDX2(mm, nn, SIZEY)] -=
        grid->cezh[IDX2(mm, nn, SIZEY)] * g_sub->hy[mm - 1];

  mm = rect.last.x;
  for (nn = rect.first.y; nn <= rect.last.y; nn++)
    grid->ez[IDX2(mm, nn, SIZEY)] +=
        grid->cezh[IDX2(mm, nn, SIZEY)] * g_sub->hy[mm];

#undef SIZEY
#undef SIZEX

  return;
}

int main() {
  Grid *g;
  Grid *g_sub;
  BoundaryParam *p;

  CALLOC(g, Grid, 1);
  CALLOC(g_sub, Grid, 1);
  CALLOC(p, BoundaryParam, 1);

  grid_init(g, TwoDimensionMagnetic,
            (GridParameter){
                .sizeX = 101,
                .sizeY = 81,
                .maxTime = 300,
                .cdtds = (1.0 / sqrt(2.0)),
                .imp0 = 377.0,
            });

  memcpy(g_sub, g, sizeof(Grid));
  grid_init_lossy(g_sub, 20, 0.35);

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
      .filename = "tfsf",
  };

  printf("-----------------------------------------------\n");
  printf("Start time: %d\t", s.start_time);
  printf("temporalStride: %d\n", s.temporalStride);
  printf("X:\t%d\t%d\t%d\n", s.startX, s.endX, s.spatialStrideX);
  printf("Y:\t%d\t%d\t%d\n", s.startY, s.endY, s.spatialStrideY);
  printf("path: %s/%s\n", s.filename, s.basename);
  printf("-----------------------------------------------\n");

  boundary_init(g, ABC, p);

  int mm = 20;
  for (int nn = 20; nn < g->param.sizeY - 20; nn++) {
    g->cezh[IDX2(mm, nn, g->param.sizeY)] = 0;
    g->ceze[IDX2(mm, nn, g->param.sizeY)] = 0;
  }

  for (g->time = 0; g->time < g->param.maxTime; g->time++) {
    updateH(g);
    tfsfUpdate(g, g_sub, (tfsfRectangle){.first = {5, 5}, .last = {97, 75}});
    updateE(g);
    // int mm = g->param.sizeX / 2;
    // int nn = g->param.sizeY / 2;
    // g->ez[IDX2(mm, nn, g->param.sizeY)] = ez_source(g, g->time, 0.0, 20);
    boundary_abc(g, p);
    snapshotGrid(g, &s);
  }

  grid_free(g);
  free(g);
  free(p);

  return EXIT_SUCCESS;
}
