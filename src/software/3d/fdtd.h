#ifndef FDTD_H_
#define FDTD_H_
#include "../../../nob.h"
#include <math.h>

typedef enum {
  OneDimension,
  TwoDimensionElectric,
  TwoDimensionMagnetic,
  ThreeDimension,
} GridType;

typedef struct {
  int sizeX, sizeY, sizeZ;
  int maxTime;
  double cdtds;
  double imp0;
} GridParameter;

typedef struct {
  int time;
  double *hx, *chxh, *chxe;
  double *hy, *chyh, *chye;
  double *hz, *chzh, *chze;
  double *ex, *cexe, *cexh;
  double *ey, *ceye, *ceyh;
  double *ez, *ceze, *cezh;
  GridType type;
  GridParameter param;
} Grid;

typedef enum {
  ABC,
  PML,
  cPML,
} BoundaryType;

typedef struct {
  double coef0, coef1, coef2;
  double *ezLeft, *ezRight, *ezTop, *ezBottom;
} BoundaryParam;

typedef struct {
  double coef;
  double *exy0, *exy1, *exz0, *exz1;
  double *eyx0, *eyx1, *eyz0, *eyz1;
  double *ezx0, *ezx1, *ezy0, *ezy1;
} BoundaryParam3d;

typedef enum {
  GaussianPulse,
  HarmonicSources,
  RickerWavelet,
} SourceType;

typedef struct {
  double time;
  double location;
  int ppw;
} SourceParameter;

typedef struct {
  struct {
    int x, y;
  } first;
  struct {
    int x, y;
  } last;
} tfsfRectangle;

typedef struct {
  int start_time;
  int temporalStride;
  int frame;
  int slice;
  int startX, endX, spatialStrideX;
  int startY, endY, spatialStrideY;
  int startZ, endZ, spatialStrideZ;
  char *filename;
  char *basename;
} Snapshot;

#define CALLOC(PNTR, TYPE, SIZE)                                               \
  PNTR = (TYPE *)calloc((SIZE), sizeof(TYPE));                                 \
  if (!PNTR) {                                                                 \
    fprintf(stderr, "[ERROR] Allocation failed for " #PNTR ".\n");             \
    abort();                                                                   \
  }

#define FREE(p)                                                                \
  do {                                                                         \
    free(p);                                                                   \
    (p) = NULL;                                                                \
  } while (0)

#define IDX2(M, N, WIDTH) ((M) * (WIDTH) + (N))
#define IDX3(M, N, P, COL, ROW) ((((M) * (COL) + (N)) * (ROW) + (P)))

void updateH(Grid *grid);
void updateE(Grid *grid);

bool grid_init(Grid *grid, GridType type, GridParameter param);
bool grid_free(Grid *g);

bool grid_init_lossy(Grid *grid, int nLoss, float maxLoss);

void boundary_init(Grid *grid, BoundaryType type, BoundaryParam *param);
void boundary_abc(Grid *grid, BoundaryParam *param);
void boundary_init_3d(Grid *grid, BoundaryType type, BoundaryParam3d *param);
void boundary_abc_3d(Grid *grid, BoundaryParam3d *param);

double ez_source(Grid *grid, double time, double location, int ppw);
double ez_source_input(Grid *grid, SourceType type, SourceParameter param);

void tfsfUpdate(Grid *grid, Grid *g_sub, tfsfRectangle rect);

void snapshotGrid(Grid *grid, Snapshot *snap);
void snapshotGrid3d(Grid *grid, Snapshot *snap);

// #define FDTD_IMPLEMENTATION
#ifdef FDTD_IMPLEMENTATION
static inline bool _mul_2_safe(size_t a, size_t b, size_t *out) {
  if (a == 0 || b == 0) {
    *out = 0;
    return true;
  }
  if (a > SIZE_MAX / b)
    return false;
  *out = a * b;
  return true;
}

static inline bool _mul_3_safe(size_t a, size_t b, size_t c, size_t *out) {
  size_t tmp;
  if (!_mul_2_safe(a, b, &tmp))
    return false;
  return _mul_2_safe(tmp, c, out);
}

bool grid_free(Grid *g) {
  if (!g) {
    return false;
  }

  FREE(g->hx);
  FREE(g->chxe);
  FREE(g->chxh);

  FREE(g->hy);
  FREE(g->chye);
  FREE(g->chyh);

  FREE(g->hz);
  FREE(g->chze);
  FREE(g->chzh);

  FREE(g->ex);
  FREE(g->cexe);
  FREE(g->cexh);

  FREE(g->ey);
  FREE(g->ceye);
  FREE(g->ceyh);

  FREE(g->ez);
  FREE(g->ceze);
  FREE(g->cezh);

  g->time = 0;
  g->type = OneDimension;
  memset(&g->param, 0, sizeof(g->param));

  return true;
}

bool grid_init(Grid *grid, GridType type, GridParameter param) {
  if (!grid) {
    return false;
  }

  GridParameter p = param;

  grid->type = type;
  grid->param = p;
  grid->time = 0;

  size_t sx = (size_t)p.sizeX;
  size_t sy = (size_t)p.sizeY;
  size_t sz = (size_t)p.sizeZ;
  size_t sx_1 = sx ? sx - 1 : 0;
  size_t sy_1 = sy ? sy - 1 : 0;
  size_t sz_1 = sz ? sz - 1 : 0;

  size_t ex_cnt = 0, ey_cnt = 0, ez_cnt = 0;
  size_t hx_cnt = 0, hy_cnt = 0, hz_cnt = 0;

  switch (type) {
  case OneDimension:
    ez_cnt = sx;
    hy_cnt = sx_1;
    break;
  case TwoDimensionMagnetic:
    if (!_mul_2_safe(sx, sy, &ez_cnt))
      goto overflow;
    if (!_mul_2_safe(sx, sy_1, &hx_cnt))
      goto overflow;
    if (!_mul_2_safe(sx_1, sy, &hy_cnt))
      goto overflow;
    break;
  case TwoDimensionElectric:
    if (!_mul_2_safe(sx_1, sy_1, &hz_cnt))
      goto overflow;
    if (!_mul_2_safe(sx, sy_1, &ex_cnt))
      goto overflow;
    if (!_mul_2_safe(sx_1, sy, &ey_cnt))
      goto overflow;
    break;
  case ThreeDimension:
    if (!_mul_3_safe(sx, sy_1, sz_1, &ex_cnt))
      goto overflow;
    if (!_mul_3_safe(sx_1, sy, sz_1, &ey_cnt))
      goto overflow;
    if (!_mul_3_safe(sx_1, sy_1, sz, &ez_cnt))
      goto overflow;
    if (!_mul_3_safe(sx_1, sy, sz, &hx_cnt))
      goto overflow;
    if (!_mul_3_safe(sx, sy_1, sz, &hy_cnt))
      goto overflow;
    if (!_mul_3_safe(sx, sy, sz_1, &hz_cnt))
      goto overflow;
    break;
  default:
    NOB_UNREACHABLE("type");
  }

  CALLOC(grid->ex, double, ex_cnt);
  CALLOC(grid->cexe, double, ex_cnt);
  CALLOC(grid->cexh, double, ex_cnt);
  CALLOC(grid->ey, double, ey_cnt);
  CALLOC(grid->ceye, double, ey_cnt);
  CALLOC(grid->ceyh, double, ey_cnt);
  CALLOC(grid->ez, double, ez_cnt);
  CALLOC(grid->ceze, double, ez_cnt);
  CALLOC(grid->cezh, double, ez_cnt);
  CALLOC(grid->hx, double, hx_cnt);
  CALLOC(grid->chxh, double, hx_cnt);
  CALLOC(grid->chxe, double, hx_cnt);
  CALLOC(grid->hy, double, hy_cnt);
  CALLOC(grid->chyh, double, hy_cnt);
  CALLOC(grid->chye, double, hy_cnt);
  CALLOC(grid->hz, double, hz_cnt);
  CALLOC(grid->chzh, double, hz_cnt);
  CALLOC(grid->chze, double, hz_cnt);

  for (size_t i = 0; i < ex_cnt; ++i) {
    grid->cexe[i] = 1.0;
    grid->cexh[i] = p.cdtds * p.imp0;
  }
  for (size_t i = 0; i < ey_cnt; ++i) {
    grid->ceye[i] = 1.0;
    grid->ceyh[i] = p.cdtds * p.imp0;
  }
  for (size_t i = 0; i < ez_cnt; ++i) {
    grid->ceze[i] = 1.0;
    grid->cezh[i] = p.cdtds * p.imp0;
  }

  for (size_t i = 0; i < hx_cnt; ++i) {
    grid->chxh[i] = 1.0;
    grid->chxe[i] = p.cdtds / p.imp0;
  }
  for (size_t i = 0; i < hy_cnt; ++i) {
    grid->chyh[i] = 1.0;
    grid->chye[i] = p.cdtds / p.imp0;
  }
  for (size_t i = 0; i < hz_cnt; ++i) {
    grid->chzh[i] = 1.0;
    grid->chze[i] = p.cdtds / p.imp0;
  }

  return true;

overflow:
  fprintf(stderr, "[grid_init] Requested grid size overflow size_t\n");
  return false;
}

void updateH(Grid *grid) {
  int mm, nn, pp;
  switch (grid->type) {
  case OneDimension:
    for (mm = 0; mm < (grid->param.sizeX - 1); mm++) {
      grid->hy[mm] = grid->chyh[mm] * grid->hy[mm] +
                     grid->chye[mm] * (grid->ez[mm + 1] - grid->ez[mm]);
    }
    return;

  case TwoDimensionMagnetic:
    for (mm = 0; mm < grid->param.sizeX; mm++) {
      for (nn = 0; nn < (grid->param.sizeY - 1); nn++) {
        grid->hx[IDX2(mm, nn, (grid->param.sizeY - 1))] =
            grid->chxh[IDX2(mm, nn, (grid->param.sizeY - 1))] *
                grid->hx[IDX2(mm, nn, (grid->param.sizeY - 1))] -
            grid->chxe[IDX2(mm, nn, (grid->param.sizeY - 1))] *
                (grid->ez[IDX2(mm, nn + 1, (grid->param.sizeY))] -
                 grid->ez[IDX2(mm, nn, (grid->param.sizeY))]);
        // Hx(grid, mm, nn) = Chxh(grid, mm, nn) * Hx(grid, mm, nn) - Chxe(grid,
        // mm, nn) * (Ez(grid, mm, nn + 1) - Ez(grid, mm, nn));
      }
    }
    for (mm = 0; mm < (grid->param.sizeX - 1); mm++) {
      for (nn = 0; nn < grid->param.sizeY; nn++) {
        grid->hy[IDX2(mm, nn, (grid->param.sizeY))] =
            grid->chyh[IDX2(mm, nn, (grid->param.sizeY))] *
                grid->hy[IDX2(mm, nn, (grid->param.sizeY))] +
            grid->chye[IDX2(mm, nn, grid->param.sizeY)] *
                (grid->ez[IDX2(mm + 1, nn, grid->param.sizeY)] -
                 grid->ez[IDX2(mm, nn, grid->param.sizeY)]);
        // Hy(grid, mm, nn) = Chyh(grid, mm, nn) * Hy(grid, mm, nn) + Chye(grid,
        // mm, nn) * (Ez(grid, mm + 1, nn) - Ez(grid, mm, nn));
      }
    }
    return;

  case TwoDimensionElectric:
    for (mm = 0; mm < (grid->param.sizeX - 1); mm++) {
      for (nn = 0; nn < (grid->param.sizeY - 1); nn++) {
        grid->hz[IDX2(mm, nn, grid->param.sizeY)] =
            grid->chzh[IDX2(mm, nn, grid->param.sizeY)] *
                grid->hz[IDX2(mm, nn, grid->param.sizeY)] +
            grid->chze[IDX2(mm, nn, grid->param.sizeY)] *
                (grid->ex[IDX2(mm, nn + 1, grid->param.sizeY)] -
                 grid->ex[IDX2(mm, nn, grid->param.sizeY)] -
                 grid->ey[IDX2(mm + 1, nn, grid->param.sizeY)] -
                 grid->ey[IDX2(mm, nn, grid->param.sizeY)]);
      }
    }
    return;

  case ThreeDimension:
    printf("updating hx\n");
    for (mm = 0; mm < (grid->param.sizeX); mm++) {
      for (nn = 0; nn < (grid->param.sizeY - 1); nn++) {
        for (pp = 0; pp < (grid->param.sizeZ - 1); pp++) {
          size_t idx_hx =
              IDX3(mm, nn, pp, grid->param.sizeY - 1, grid->param.sizeZ - 1);
          size_t idx_ey =
              IDX3(mm, nn, pp, grid->param.sizeY - 1, grid->param.sizeZ);
          size_t idx_ey_p =
              IDX3(mm, nn, pp + 1, grid->param.sizeY - 1, grid->param.sizeZ);
          size_t idx_ez =
              IDX3(mm, nn, pp, grid->param.sizeY, grid->param.sizeZ - 1);
          size_t idx_ez_p =
              IDX3(mm, nn + 1, pp, grid->param.sizeY, grid->param.sizeZ - 1);
          grid->hx[idx_hx] =
              grid->chxh[idx_hx] * grid->hx[idx_hx] +
              grid->chxe[idx_hx] * ((grid->ey[idx_ey_p] - grid->ey[idx_ey]) -
                                    (grid->ez[idx_ez_p] - grid->ez[idx_ez]));
        }
      }
    }
    printf("updating hy\n");
    for (mm = 0; mm < (grid->param.sizeX - 1); mm++) {
      for (nn = 0; nn < (grid->param.sizeY); nn++) {
        for (pp = 0; pp < (grid->param.sizeZ - 1); pp++) {
          grid->hy[IDX3(mm, nn, pp, grid->param.sizeY, grid->param.sizeZ - 1)] =
              grid->chyh[IDX3(mm, nn, pp, grid->param.sizeY,
                              grid->param.sizeZ - 1)] *
                  grid->hy[IDX3(mm, nn, pp, grid->param.sizeY,
                                grid->param.sizeZ - 1)] +
              grid->chye[IDX3(mm, nn, pp, grid->param.sizeY,
                              grid->param.sizeZ - 1)] *
                  ((grid->ez[IDX3(mm + 1, nn, pp, grid->param.sizeY,
                                  grid->param.sizeZ - 1)] -
                    grid->ez[IDX3(mm, nn, pp, grid->param.sizeY,
                                  grid->param.sizeZ - 1)]) -
                   (grid->ex[IDX3(mm, nn, pp + 1, grid->param.sizeY,
                                  grid->param.sizeZ)] -
                    grid->ex[IDX3(mm, nn, pp, grid->param.sizeY,
                                  grid->param.sizeZ)]));
        }
      }
    }
    printf("updating hz\n");
    for (mm = 0; mm < (grid->param.sizeX - 1); mm++) {
      for (nn = 0; nn < (grid->param.sizeY - 1); nn++) {
        for (pp = 0; pp < (grid->param.sizeZ); pp++) {
          grid->hz[IDX3(mm, nn, pp, grid->param.sizeY - 1, grid->param.sizeZ)] =
              grid->chzh[IDX3(mm, nn, pp, grid->param.sizeY - 1,
                              grid->param.sizeZ)] *
                  grid->hz[IDX3(mm, nn, pp, grid->param.sizeY - 1,
                                grid->param.sizeZ)] +
              grid->chze[IDX3(mm, nn, pp, grid->param.sizeY - 1,
                              grid->param.sizeZ)] *
                  ((grid->ex[IDX3(mm, nn + 1, pp, grid->param.sizeY,
                                  grid->param.sizeZ)] -
                    grid->ex[IDX3(mm, nn, pp, grid->param.sizeY,
                                  grid->param.sizeZ)]) -
                   (grid->ey[IDX3(mm + 1, nn, pp, grid->param.sizeY - 1,
                                  grid->param.sizeZ)] -
                    grid->ey[IDX3(mm, nn, pp, grid->param.sizeY - 1,
                                  grid->param.sizeZ)]));
        }
      }
    }
    return;

  default:
    NOB_UNREACHABLE("updateH");
    return;
  }
}

void updateE(Grid *grid) {
  int mm, nn, pp;
  switch (grid->type) {
  case OneDimension:
    for (mm = 1; mm < (grid->param.sizeX - 1); mm++) {
      grid->ez[mm] = grid->ceze[mm] * grid->ez[mm] +
                     grid->cezh[mm] * (grid->hy[mm] - grid->hy[mm - 1]);
    }
    return;
  case TwoDimensionMagnetic:
    for (mm = 1; mm < (grid->param.sizeX - 1); mm++) {
      for (nn = 1; nn < (grid->param.sizeY - 1); nn++) {
        grid->ez[IDX2(mm, nn, grid->param.sizeY)] =
            grid->ceze[IDX2(mm, nn, grid->param.sizeY)] *
                grid->ez[IDX2(mm, nn, grid->param.sizeY)] +
            grid->cezh[IDX2(mm, nn, grid->param.sizeY)] *
                ((grid->hy[IDX2(mm, nn, grid->param.sizeY)] -
                  grid->hy[IDX2(mm - 1, nn, grid->param.sizeY)]) -
                 (grid->hx[IDX2(mm, nn, (grid->param.sizeY - 1))] -
                  grid->hx[IDX2(mm, nn - 1, (grid->param.sizeY - 1))]));
        // Ez(grid, mm, nn) = Ceze(grid, mm, nn) * Ez(grid, mm, nn) + Cezh(grid,
        // mm, nn) * ((Hy(grid, mm, nn) - Hy(grid, mm - 1, nn)) - (Hx(grid, mm,
        // nn) - Hx(grid, mm, nn - 1)));
      }
    }
    return;
  case TwoDimensionElectric:
    for (mm = 0; mm < (grid->param.sizeY - 1); mm++) {
      for (nn = 1; nn < (grid->param.sizeY - 1); nn++) {
        grid->ex[IDX2(mm, nn, grid->param.sizeY)] =
            grid->cexe[IDX2(mm, nn, grid->param.sizeY)] *
                grid->ex[IDX2(mm, nn, grid->param.sizeY)] +
            grid->cexh[IDX2(mm, nn, grid->param.sizeY)] *
                (grid->hz[IDX2(mm, nn, grid->param.sizeY)] -
                 grid->hz[IDX2(mm, nn - 1, grid->param.sizeY)]);
      }
    }
    for (mm = 1; mm < (grid->param.sizeY - 1); mm++) {
      for (nn = 0; nn < (grid->param.sizeY - 1); nn++) {
        grid->ey[IDX2(mm, nn, grid->param.sizeY)] =
            grid->ceye[IDX2(mm, nn, grid->param.sizeY)] *
                grid->ey[IDX2(mm, nn, grid->param.sizeY)] -
            grid->cezh[IDX2(mm, nn, grid->param.sizeY)] *
                (grid->hz[IDX2(mm, nn, grid->param.sizeY)] -
                 grid->hz[IDX2(mm - 1, nn, grid->param.sizeY)]);
      }
    }
    return;
  case ThreeDimension:
    printf("updating ex\n");
    for (mm = 0; mm < (grid->param.sizeX - 1); mm++) {
      for (nn = 1; nn < (grid->param.sizeY - 1); nn++) {
        for (pp = 1; pp < (grid->param.sizeZ - 1); pp++) {
          grid->ex[IDX3(mm, nn, pp, grid->param.sizeY, grid->param.sizeZ)] =
              grid->cexe[IDX3(mm, nn, pp, grid->param.sizeY,
                              grid->param.sizeZ)] *
                  grid->ex[IDX3(mm, nn, pp, grid->param.sizeY,
                                grid->param.sizeZ)] +
              grid->cexh[IDX3(mm, nn, pp, grid->param.sizeY,
                              grid->param.sizeZ)] *
                  ((grid->hz[IDX3(mm, nn, pp, grid->param.sizeY - 1,
                                  grid->param.sizeZ)] -
                    grid->hz[IDX3(mm, nn - 1, pp, grid->param.sizeY - 1,
                                  grid->param.sizeZ)]) -
                   (grid->hy[IDX3(mm, nn, pp, grid->param.sizeY,
                                  grid->param.sizeZ - 1)] -
                    grid->hy[IDX3(mm, nn, pp - 1, grid->param.sizeY,
                                  grid->param.sizeZ - 1)]));
        }
      }
    }
    printf("updating ey\n");
    for (mm = 1; mm < (grid->param.sizeX - 1); mm++) {
      for (nn = 0; nn < (grid->param.sizeY - 1); nn++) {
        for (pp = 1; pp < (grid->param.sizeZ - 1); pp++) {
          grid->ey[IDX3(mm, nn, pp, grid->param.sizeY - 1, grid->param.sizeZ)] =
              grid->ceye[IDX3(mm, nn, pp, grid->param.sizeY - 1,
                              grid->param.sizeZ)] *
                  grid->ey[IDX3(mm, nn, pp, grid->param.sizeY - 1,
                                grid->param.sizeZ)] +
              grid->ceyh[IDX3(mm, nn, pp, grid->param.sizeY - 1,
                              grid->param.sizeZ)] *
                  ((grid->hx[IDX3(mm, nn, pp, grid->param.sizeY - 1,
                                  grid->param.sizeZ - 1)] -
                    grid->hx[IDX3(mm, nn, pp - 1, grid->param.sizeY - 1,
                                  grid->param.sizeZ - 1)]) -
                   (grid->hz[IDX3(mm, nn, pp, grid->param.sizeY - 1,
                                  grid->param.sizeZ)] -
                    grid->hz[IDX3(mm - 1, nn, pp, grid->param.sizeY - 1,
                                  grid->param.sizeZ)]));
        }
      }
    }
    printf("updating ez\n");
    for (mm = 1; mm < (grid->param.sizeX - 1); mm++) {
      for (nn = 1; nn < (grid->param.sizeY - 1); nn++) {
        for (pp = 0; pp < (grid->param.sizeZ - 1); pp++) {
          grid->ez[IDX3(mm, nn, pp, grid->param.sizeY, grid->param.sizeZ - 1)] =
              grid->ceze[IDX3(mm, nn, pp, grid->param.sizeY,
                              grid->param.sizeZ - 1)] *
                  grid->ez[IDX3(mm, nn, pp, grid->param.sizeY,
                                grid->param.sizeZ - 1)] +
              grid->cezh[IDX3(mm, nn, pp, grid->param.sizeY,
                              grid->param.sizeZ - 1)] *
                  ((grid->hy[IDX3(mm, nn, pp, grid->param.sizeY,
                                  grid->param.sizeZ - 1)] -
                    grid->hy[IDX3(mm - 1, nn, pp, grid->param.sizeY,
                                  grid->param.sizeZ - 1)]) -
                   (grid->hx[IDX3(mm, nn, pp, grid->param.sizeY - 1,
                                  grid->param.sizeZ - 1)] -
                    grid->hx[IDX3(mm, nn - 1, pp, grid->param.sizeY - 1,
                                  grid->param.sizeZ - 1)]));
        }
      }
    }
    return;
  default:
    NOB_UNREACHABLE("updateE");
    return;
  }
}

void snapshotGrid(Grid *grid, Snapshot *snap) {
  int mm, nn;
  float dim, temp;
  char filename[100];
  FILE *out;

  nob_minimal_log_level = NOB_WARNING;
  if (!nob_mkdir_if_not_exists(snap->filename))
    return;
  nob_minimal_log_level = NOB_INFO;

  if (grid->time >= snap->start_time &&
      (grid->time - snap->start_time) % snap->temporalStride == 0) {
    sprintf(filename, "%s/%s.%d", snap->filename, snap->basename,
            snap->frame++);
    out = fopen(filename, "wb");

    dim = (float)(snap->endX - snap->startX) / snap->spatialStrideX + 1;
    fwrite(&dim, sizeof(float), 1, out);
    dim = (float)(snap->endY - snap->startY) / snap->spatialStrideY + 1;
    fwrite(&dim, sizeof(float), 1, out);

    for (nn = snap->endY; nn >= snap->startY; nn -= snap->spatialStrideY) {
      for (mm = snap->startX; mm <= snap->endX; mm += snap->spatialStrideX) {
        temp = (float)grid->ez[IDX2(mm, nn, grid->param.sizeY)];

        // temp = (float)Ez(grid, mm, nn);
        fwrite(&temp, sizeof(float), 1, out);
      }
    }

    fclose(out);
  }
}
#endif // FDTD_IMPLEMENTATION
#endif // !FDTD_H_
