#include "naive.h"
// #include <ap_fixed.h>

static float hx_buffer[HX_BUFFER] = {0};
static float hy_buffer[HY_BUFFER] = {0};
static float hz_buffer[HZ_BUFFER] = {0};
static float ex_buffer[EX_BUFFER] = {0};
static float ey_buffer[EY_BUFFER] = {0};
static float ez_buffer[EZ_BUFFER] = {0};

void updateH(float *hx, float *hy, float *hz, float *ex, float *ey, float *ez) {
  int mm, nn, pp;
  for (mm = 0; mm < NX_0; mm++) {
    for (nn = 0; nn < NY_1; nn++) {
      for (pp = 0; pp < NZ_1; pp++) {
#pragma HLS PIPELINE
        int n1       = nn + 1;
        int p1       = pp + 1;
        int idx      = (mm * NY_1 + nn) * NZ_1 + pp;
        int idx_ey_p = (mm * NY_1 + nn) * NZ_0 + p1;
        int idx_ey   = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_ez_n = (mm * NY_0 + n1) * NZ_1 + pp;
        int idx_ez   = (mm * NY_0 + nn) * NZ_1 + pp;

        hx[idx] = 1.0 * hx[idx] + coeff_div * ((ey[idx_ey_p] - ey[idx_ey]) -
                                               (ez[idx_ez_n] - ez[idx_ez]));
      }
    }
  }

  for (mm = 0; mm < NX_1; mm++) {
    for (nn = 0; nn < NY_0; nn++) {
      for (pp = 0; pp < NZ_1; pp++) {
#pragma HLS PIPELINE
        int m1       = mm + 1;
        int p1       = pp + 1;
        int idx      = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_ez_m = (m1 * NY_0 + nn) * NZ_1 + pp;
        int idx_ez   = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_ex_p = (mm * NY_0 + nn) * NZ_0 + p1;
        int idx_ex   = (mm * NY_0 + nn) * NZ_0 + pp;

        hy[idx] = 1.0 * hy[idx] + coeff_div * ((ez[idx_ez_m] - ez[idx_ez]) -
                                               (ex[idx_ex_p] - ex[idx_ex]));
      }
    }
  }

  for (mm = 0; mm < NX_1; mm++) {
    for (nn = 0; nn < NY_1; nn++) {
      for (pp = 0; pp < NZ_0; pp++) {
#pragma HLS PIPELINE
        int n1       = nn + 1;
        int m1       = mm + 1;
        int idx      = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_ex_n = (mm * NY_0 + n1) * NZ_0 + pp;
        int idx_ex   = (mm * NY_0 + nn) * NZ_0 + pp;
        int idx_ey_m = (m1 * NY_1 + nn) * NZ_0 + pp;
        int idx_ey   = (mm * NY_1 + nn) * NZ_0 + pp;

        hz[idx] = 1.0 * hz[idx] + coeff_div * ((ex[idx_ex_n] - ex[idx_ex]) -
                                               (ey[idx_ey_m] - ey[idx_ey]));
      }
    }
  }
}

void updateE(float *hx, float *hy, float *hz, float *ex, float *ey, float *ez) {
  int mm, nn, pp;
  for (mm = 0; mm < NX_1; mm++) {
    for (nn = 1; nn < NY_1; nn++) {
      for (pp = 1; pp < NZ_1; pp++) {
#pragma HLS PIPELINE
        int _n       = nn - 1;
        int _p       = pp - 1;
        int idx      = (mm * NY_0 + nn) * NZ_0 + pp;
        int idx_hz   = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_hz_n = (mm * NY_1 + _n) * NZ_0 + pp;
        int idx_hy   = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_hy_p = (mm * NY_0 + nn) * NZ_1 + _p;

        ex[idx] = 1.0 * ex[idx] + coeff_mul * ((hz[idx_hz] - hz[idx_hz_n]) -
                                               (hy[idx_hy] - hy[idx_hy_p]));
      }
    }
  }

  for (mm = 1; mm < NX_1; mm++) {
    for (nn = 0; nn < NY_1; nn++) {
      for (pp = 1; pp < NZ_1; pp++) {
#pragma HLS PIPELINE
        int _p       = pp - 1;
        int _m       = mm - 1;
        int idx      = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_hx   = (mm * NY_1 + nn) * NZ_1 + pp;
        int idx_hx_m = (mm * NY_1 + nn) * NZ_1 + _p;
        int idx_hz   = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_hz_m = (_m * NY_1 + nn) * NZ_0 + pp;

        ey[idx] = 1.0 * ey[idx] + coeff_mul * ((hx[idx_hx] - hx[idx_hx_m]) -
                                               (hz[idx_hz] - hz[idx_hz_m]));
      }
    }
  }

  for (mm = 1; mm < NX_1; mm++) {
    for (nn = 1; nn < NY_1; nn++) {
      for (pp = 0; pp < NZ_1; pp++) {
#pragma HLS PIPELINE
        int _m       = mm - 1;
        int _n       = nn - 1;
        int idx      = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_hy   = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_hy_m = (_m * NY_0 + nn) * NZ_1 + pp;
        int idx_hx   = (mm * NY_1 + nn) * NZ_1 + pp;
        int idx_hx_n = (mm * NY_1 + _n) * NZ_1 + pp;

        ez[idx] = 1.0 * ez[idx] + coeff_mul * ((hy[idx_hy] - hy[idx_hy_m]) -
                                               (hx[idx_hx] - hx[idx_hx_n]));
      }
    }
  }
}

void fdtd_kernal() {
  for (int t = 0; t < MAX_TIME; t++) {
#pragma HLS PIPELINE
    updateH(hx_buffer, hy_buffer, hz_buffer, ex_buffer, ey_buffer, ez_buffer);
    updateE(hx_buffer, hy_buffer, hz_buffer, ex_buffer, ey_buffer, ez_buffer);
  }
}
