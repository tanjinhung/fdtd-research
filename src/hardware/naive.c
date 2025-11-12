#include "naive.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
// #include <ap_fixed.h>

static float hx_buffer[HX_BUFFER] = {0};
static float hy_buffer[HY_BUFFER] = {0};
static float hz_buffer[HZ_BUFFER] = {0};
static float ex_buffer[EX_BUFFER] = {0};
static float ey_buffer[EY_BUFFER] = {0};
static float ez_buffer[EZ_BUFFER] = {0};

void updateH(Grid *g) {
  for (int mm = 0; mm < NX_0; mm++) {
    for (int nn = 0; nn < NY_1; nn++) {
      for (int pp = 0; pp < NZ_1; pp++) {
#pragma HLS PIPELINE
        int n1       = nn + 1;
        int p1       = pp + 1;
        int idx      = (mm * NY_1 + nn) * NZ_1 + pp;
        int idx_ey_p = (mm * NY_1 + nn) * NZ_0 + p1;
        int idx_ey   = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_ez_n = (mm * NY_0 + n1) * NZ_1 + pp;
        int idx_ez   = (mm * NY_0 + nn) * NZ_1 + pp;

        g->internal.hx[idx] =
            1.0 * g->internal.hx[idx] +
            coeff_div * ((g->internal.ey[idx_ey_p] - g->internal.ey[idx_ey]) -
                         (g->internal.ez[idx_ez_n] - g->internal.ez[idx_ez]));
      }
    }
  }

  for (int mm = 0; mm < NX_1; mm++) {
    for (int nn = 0; nn < NY_0; nn++) {
      for (int pp = 0; pp < NZ_1; pp++) {
#pragma HLS PIPELINE
        int m1       = mm + 1;
        int p1       = pp + 1;
        int idx      = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_ez_m = (m1 * NY_0 + nn) * NZ_1 + pp;
        int idx_ez   = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_ex_p = (mm * NY_0 + nn) * NZ_0 + p1;
        int idx_ex   = (mm * NY_0 + nn) * NZ_0 + pp;

        g->internal.hy[idx] =
            1.0 * g->internal.hy[idx] +
            coeff_div * ((g->internal.ez[idx_ez_m] - g->internal.ez[idx_ez]) -
                         (g->internal.ex[idx_ex_p] - g->internal.ex[idx_ex]));
      }
    }
  }

  for (int mm = 0; mm < NX_1; mm++) {
    for (int nn = 0; nn < NY_1; nn++) {
      for (int pp = 0; pp < NZ_0; pp++) {
#pragma HLS PIPELINE
        int n1       = nn + 1;
        int m1       = mm + 1;
        int idx      = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_ex_n = (mm * NY_0 + n1) * NZ_0 + pp;
        int idx_ex   = (mm * NY_0 + nn) * NZ_0 + pp;
        int idx_ey_m = (m1 * NY_1 + nn) * NZ_0 + pp;
        int idx_ey   = (mm * NY_1 + nn) * NZ_0 + pp;

        g->internal.hz[idx] =
            1.0 * g->internal.hz[idx] +
            coeff_div * ((g->internal.ex[idx_ex_n] - g->internal.ex[idx_ex]) -
                         (g->internal.ey[idx_ey_m] - g->internal.ey[idx_ey]));
      }
    }
  }
}
void updateE(Grid *g) {
  for (int mm = 0; mm < NX_1; mm++) {
    for (int nn = 1; nn < NY_1; nn++) {
      for (int pp = 1; pp < NZ_1; pp++) {
#pragma HLS PIPELINE
        int _n       = nn - 1;
        int _p       = pp - 1;
        int idx      = (mm * NY_0 + nn) * NZ_0 + pp;
        int idx_hz   = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_hz_n = (mm * NY_1 + _n) * NZ_0 + pp;
        int idx_hy   = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_hy_p = (mm * NY_0 + nn) * NZ_1 + _p;

        g->internal.ex[idx] =
            1.0 * g->internal.ex[idx] +
            coeff_mul * ((g->internal.hz[idx_hz] - g->internal.hz[idx_hz_n]) -
                         (g->internal.hy[idx_hy] - g->internal.hy[idx_hy_p]));
      }
    }
  }

  for (int mm = 1; mm < NX_1; mm++) {
    for (int nn = 0; nn < NY_1; nn++) {
      for (int pp = 1; pp < NZ_1; pp++) {
#pragma HLS PIPELINE
        int _p       = pp - 1;
        int _m       = mm - 1;
        int idx      = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_hx   = (mm * NY_1 + nn) * NZ_1 + pp;
        int idx_hx_m = (mm * NY_1 + nn) * NZ_1 + _p;
        int idx_hz   = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_hz_m = (_m * NY_1 + nn) * NZ_0 + pp;

        g->internal.ey[idx] =
            1.0 * g->internal.ey[idx] +
            coeff_mul * ((g->internal.hx[idx_hx] - g->internal.hx[idx_hx_m]) -
                         (g->internal.hz[idx_hz] - g->internal.hz[idx_hz_m]));
      }
    }
  }

  for (int mm = 1; mm < NX_1; mm++) {
    for (int nn = 1; nn < NY_1; nn++) {
      for (int pp = 0; pp < NZ_1; pp++) {
#pragma HLS PIPELINE
        int _m       = mm - 1;
        int _n       = nn - 1;
        int idx      = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_hy   = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_hy_m = (_m * NY_0 + nn) * NZ_1 + pp;
        int idx_hx   = (mm * NY_1 + nn) * NZ_1 + pp;
        int idx_hx_n = (mm * NY_1 + _n) * NZ_1 + pp;

        g->internal.ez[idx] =
            1.0 * g->internal.ez[idx] +
            coeff_mul * ((g->internal.hy[idx_hy] - g->internal.hy[idx_hy_m]) -
                         (g->internal.hx[idx_hx] - g->internal.hx[idx_hx_n]));
      }
    }
  }
}

int main(void) {
  printf("Naive hardware module initialized successfully.\n");

  GridIntern internal = {
      hx_buffer, hy_buffer, hz_buffer, ex_buffer, ey_buffer, ez_buffer,
  };

  Grid g = {ThreeDimension, 0, internal};

  for (g.time = 0; g.time < MAX_TIME; g.time++) {
#pragma HLS PIPELINE II = 1
    updateH(&g);
    updateE(&g);
  }

  return EXIT_SUCCESS;
}
