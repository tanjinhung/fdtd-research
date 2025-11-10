#include "naive.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define NX_0      42
#define NY_0      42
#define NZ_0      42
#define MAX_TIME  300
#define NX_1      (NX_0 - 1)
#define NY_1      (NY_0 - 1)
#define NZ_1      (NZ_0 - 1)
#define HX_BUFFER (NX_0 * NY_1 * NZ_1)
#define HY_BUFFER (NX_1 * NY_0 * NZ_1)
#define HZ_BUFFER (NX_1 * NY_1 * NZ_0)
#define EX_BUFFER (NX_1 * NY_0 * NZ_0)
#define EY_BUFFER (NX_0 * NY_1 * NZ_0)
#define EZ_BUFFER (NX_0 * NY_0 * NZ_1)

static double hx_buffer[HX_BUFFER]   = {0};
static double chxh_buffer[HX_BUFFER] = {0};
static double chxe_buffer[HX_BUFFER] = {0};
static double hy_buffer[HY_BUFFER]   = {0};
static double chyh_buffer[HY_BUFFER] = {0};
static double chye_buffer[HY_BUFFER] = {0};
static double hz_buffer[HZ_BUFFER]   = {0};
static double chzh_buffer[HZ_BUFFER] = {0};
static double chze_buffer[HZ_BUFFER] = {0};
static double ex_buffer[EX_BUFFER]   = {0};
static double cexh_buffer[EX_BUFFER] = {0};
static double cexe_buffer[EX_BUFFER] = {0};
static double ey_buffer[EY_BUFFER]   = {0};
static double ceyh_buffer[EY_BUFFER] = {0};
static double ceye_buffer[EY_BUFFER] = {0};
static double ez_buffer[EZ_BUFFER]   = {0};
static double cezh_buffer[EZ_BUFFER] = {0};
static double ceze_buffer[EZ_BUFFER] = {0};

void updateH(Grid *g) {
  for (int mm = 0; mm < NX_0; mm++) {
    for (int nn = 0; nn < NY_1; nn++) {
      for (int pp = 0; pp < NZ_1; pp++) {
        int n1       = nn + 1;
        int p1       = pp + 1;
        int idx      = (mm * NY_1 + nn) * NZ_1 + pp;
        int idx_ey_p = (mm * NY_1 + nn) * NZ_0 + p1;
        int idx_ey   = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_ez_n = (mm * NY_0 + n1) * NZ_1 + pp;
        int idx_ez   = (mm * NY_0 + nn) * NZ_1 + pp;

        g->internal.hx[idx] =
            g->internal.chxh[idx] * g->internal.hx[idx] +
            g->internal.chxe[idx] *
                ((g->internal.ey[idx_ey_p] - g->internal.ey[idx_ey]) -
                 (g->internal.ez[idx_ez_n] - g->internal.ez[idx_ez]));
      }
    }
  }

  for (int mm = 0; mm < NX_1; mm++) {
    for (int nn = 0; nn < NY_0; nn++) {
      for (int pp = 0; pp < NZ_1; pp++) {
        int m1       = mm + 1;
        int p1       = pp + 1;
        int idx      = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_ez_m = 1;
      }
    }
  }
}
void updateE(Grid *g) {}

int main(void) {
  printf("Naive hardware module initialized successfully.\n");

  // clang-format off
  GridIntern internal = {
      hx_buffer, chxe_buffer, chxh_buffer,
      hy_buffer, chye_buffer, chyh_buffer,
      hz_buffer, chze_buffer, chzh_buffer,
      ex_buffer, cexe_buffer, cexh_buffer,
      ey_buffer, ceye_buffer, ceyh_buffer,
      ez_buffer, ceze_buffer, cezh_buffer
  };
  // clang-format on

  GridExtern external = {NX_0, NY_0, NZ_0, 1.0 / sqrt(3.0), 377.0, 0};

  for (int i = 0; i < NX_0; ++i) {
#pragma HLS PIPELINE II = 1
    internal.chxh[i] = 1.0;
    internal.chxe[i] = external.cdtds / external.imp0;
    internal.cexe[i] = 1.0;
    internal.cexh[i] = external.cdtds * external.imp0;
  }

  for (int i = 0; i < NY_0; ++i) {
#pragma HLS PIPELINE II = 1
    internal.chyh[i] = 1.0;
    internal.chye[i] = external.cdtds / external.imp0;
    internal.ceye[i] = 1.0;
    internal.ceyh[i] = external.cdtds * external.imp0;
  }

  for (int i = 0; i < NZ_0; ++i) {
#pragma HLS PIPELINE II = 1
    internal.chzh[i] = 1.0;
    internal.chze[i] = external.cdtds / external.imp0;
    internal.ceze[i] = 1.0;
    internal.cezh[i] = external.cdtds * external.imp0;
  }

  Grid g = {ThreeDimension, external, internal};

  for (g.external.time = 0; g.external.time < MAX_TIME; g.external.time++) {
    updateH(&g);
    updateE(&g);
  }

  return EXIT_SUCCESS;
}
