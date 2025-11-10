#include "naive.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
// #include <ap_fixed.h>

#define MAX_TIME  300
#define IMP0      377.0
#define CDTDS     0.57735026918962576 // Precomputed value of 1/sqrt(3)
#define NX_0      42
#define NY_0      42
#define NZ_0      42
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
#pragma HLS PIPELINE II = 1
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
#pragma HLS PIPELINE II = 1
        int m1       = mm + 1;
        int p1       = pp + 1;
        int idx      = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_ez_m = (m1 * NY_0 + nn) * NZ_1 + pp;
        int idx_ez   = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_ex_p = (mm * NY_0 + nn) * NZ_0 + p1;
        int idx_ex   = (mm * NY_0 + nn) * NZ_0 + pp;

        g->internal.hy[idx] =
            g->internal.chyh[idx] * g->internal.hy[idx] +
            g->internal.chye[idx] *
                ((g->internal.ez[idx_ez_m] - g->internal.ez[idx_ez]) -
                 (g->internal.ex[idx_ex_p] - g->internal.ex[idx_ex]));
      }
    }
  }

  for (int mm = 0; mm < NX_1; mm++) {
    for (int nn = 0; nn < NY_1; nn++) {
      for (int pp = 0; pp < NZ_0; pp++) {
#pragma HLS PIPELINE II = 1
        int n1       = nn + 1;
        int m1       = mm + 1;
        int idx      = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_ex_n = (mm * NY_0 + n1) * NZ_0 + pp;
        int idx_ex   = (mm * NY_0 + nn) * NZ_0 + pp;
        int idx_ey_m = (m1 * NY_1 + nn) * NZ_0 + pp;
        int idx_ey   = (mm * NY_1 + nn) * NZ_0 + pp;

        g->internal.hz[idx] =
            g->internal.chzh[idx] * g->internal.hz[idx] +
            g->internal.chze[idx] *
                ((g->internal.ex[idx_ex_n] - g->internal.ex[idx_ex]) -
                 (g->internal.ey[idx_ey_m] - g->internal.ey[idx_ey]));
      }
    }
  }
}

void updateE(Grid *g) {
  for (int mm = 0; mm < NX_1; mm++) {
    for (int nn = 1; nn < NY_1; nn++) {
      for (int pp = 1; pp < NZ_1; pp++) {
#pragma HLS PIPELINE II = 1
        int _n       = nn - 1;
        int _p       = pp - 1;
        int idx      = (mm * NY_0 + nn) * NZ_0 + pp;
        int idx_hz   = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_hz_n = (mm * NY_1 + _n) * NZ_0 + pp;
        int idx_hy   = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_hy_p = (mm * NY_0 + nn) * NZ_1 + _p;

        g->internal.ex[idx] =
            g->internal.cexe[idx] * g->internal.ex[idx] +
            g->internal.cexh[idx] *
                ((g->internal.hz[idx_hz] - g->internal.hz[idx_hz_n]) -
                 (g->internal.hy[idx_hy] - g->internal.hy[idx_hy_p]));
      }
    }
  }

  for (int mm = 1; mm < NX_1; mm++) {
    for (int nn = 0; nn < NY_1; nn++) {
      for (int pp = 1; pp < NZ_1; pp++) {
#pragma HLS PIPELINE II = 1
        int _p       = pp - 1;
        int _m       = mm - 1;
        int idx      = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_hx   = (mm * NY_1 + nn) * NZ_1 + pp;
        int idx_hx_m = (mm * NY_1 + nn) * NZ_1 + _p;
        int idx_hz   = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_hz_m = (_m * NY_1 + nn) * NZ_0 + pp;

        g->internal.ey[idx] =
            g->internal.ceye[idx] * g->internal.ey[idx] +
            g->internal.ceyh[idx] *
                ((g->internal.hx[idx_hx] - g->internal.hx[idx_hx_m]) -
                 (g->internal.hz[idx_hz] - g->internal.hz[idx_hz_m]));
      }
    }
  }

  for (int mm = 1; mm < NX_1; mm++) {
    for (int nn = 1; nn < NY_1; nn++) {
      for (int pp = 0; pp < NZ_1; pp++) {
#pragma HLS PIPELINE II = 1
        int _m       = mm - 1;
        int _n       = nn - 1;
        int idx      = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_hy   = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_hy_m = (_m * NY_0 + nn) * NZ_1 + pp;
        int idx_hx   = (mm * NY_1 + nn) * NZ_1 + pp;
        int idx_hx_n = (mm * NY_1 + _n) * NZ_1 + pp;

        g->internal.ez[idx] =
            g->internal.ceze[idx] * g->internal.ez[idx] +
            g->internal.cezh[idx] *
                ((g->internal.hy[idx_hy] - g->internal.hy[idx_hy_m]) -
                 (g->internal.hx[idx_hx] - g->internal.hx[idx_hx_n]));
      }
    }
  }
}

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

  GridExtern   external  = {NX_0, NY_0, NZ_0, CDTDS, IMP0, 0};
  static float coeff_div = CDTDS / IMP0;
  static float coeff_mul = CDTDS * IMP0;

  for (int i = 0; i < NX_0; ++i) {
#pragma HLS PIPELINE II = 1
    internal.chxh[i] = 1.0;
    internal.chxe[i] = coeff_div;
    internal.cexe[i] = 1.0;
    internal.cexh[i] = coeff_mul;
  }

  for (int i = 0; i < NY_0; ++i) {
#pragma HLS PIPELINE II = 1
    internal.chyh[i] = 1.0;
    internal.chye[i] = coeff_div;
    internal.ceye[i] = 1.0;
    internal.ceyh[i] = coeff_mul;
  }

  for (int i = 0; i < NZ_0; ++i) {
#pragma HLS PIPELINE II = 1
    internal.chzh[i] = 1.0;
    internal.chze[i] = coeff_div;
    internal.ceze[i] = 1.0;
    internal.cezh[i] = coeff_mul;
  }

  Grid g = {ThreeDimension, external, internal};

  for (g.external.time = 0; g.external.time < MAX_TIME; g.external.time++) {
#pragma HLS PIPELINE II = 1
    updateH(&g);
    updateE(&g);
  }

  return EXIT_SUCCESS;
}
