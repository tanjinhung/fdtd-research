#include "naive.h"
// #include <ap_fixed.h>

static float coeff_div = CDTDS / IMP0;
static float coeff_mul = CDTDS * IMP0;

static inline int isDipole(int x, int y, int z) {
#pragma HLS INLINE
  int dx = x - DIPOLE_CENTER_X;
  int dy = y - DIPOLE_CENTER_Y;
  int dz = z - DIPOLE_CENTER_Z;

  if (dx * dx + dy * dy > DIPOLE_RADIUS * DIPOLE_RADIUS) return 0;

  int half_length = DIPOLE_LENGTH / 2;
  if (dz >= -half_length && dz <= half_length &&
      (dz < -FEED_GAP / 2 || dz > FEED_GAP / 2))
    return 1;

  return 0;
}

void updateHx(float *__restrict__ hx, float *__restrict__ ey,
              float *__restrict__ ez) {
#pragma HLS INLINE off
  int mm, nn, pp;
  for (mm = 0; mm < NX_0; mm++) {
    for (nn = 0; nn < NY_1; nn++) {
      for (pp = 0; pp < NZ_1; pp++) {
#pragma HLS DEPENDENCE class = array dependent = true direction =              \
    raw                              distance = 1 type = inter
#pragma HLS PIPELINE                 II                = 1
        int n1       = nn + 1;
        int p1       = pp + 1;
        int idx      = (mm * NY_1 + nn) * NZ_1 + pp;
        int idx_ey_p = (mm * NY_1 + nn) * NZ_0 + p1;
        int idx_ey   = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_ez_n = (mm * NY_0 + n1) * NZ_1 + pp;
        int idx_ez   = (mm * NY_0 + nn) * NZ_1 + pp;

        hx[idx] = hx[idx] + coeff_div * ((ey[idx_ey_p] - ey[idx_ey]) -
                                         (ez[idx_ez_n] - ez[idx_ez]));
      }
    }
  }
};

void updateHy(float *__restrict__ hy, float *__restrict__ ez,
              float *__restrict__ ex) {
#pragma HLS INLINE off
  int mm, nn, pp;
  for (mm = 0; mm < NX_1; mm++) {
    for (nn = 0; nn < NY_0; nn++) {
      for (pp = 0; pp < NZ_1; pp++) {
#pragma HLS DEPENDENCE class = array dependent = true direction =              \
    raw                              distance = 1 type = inter
#pragma HLS PIPELINE                 II                = 1
        int m1       = mm + 1;
        int p1       = pp + 1;
        int idx      = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_ez_m = (m1 * NY_0 + nn) * NZ_1 + pp;
        int idx_ez   = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_ex_p = (mm * NY_0 + nn) * NZ_0 + p1;
        int idx_ex   = (mm * NY_0 + nn) * NZ_0 + pp;

        hy[idx] = hy[idx] + coeff_div * ((ez[idx_ez_m] - ez[idx_ez]) -
                                         (ex[idx_ex_p] - ex[idx_ex]));
      }
    }
  }
}

void updateHz(float *__restrict__ hz, float *__restrict__ ex,
              float *__restrict__ ey) {
#pragma HLS INLINE off
  int mm, nn, pp;
  for (mm = 0; mm < NX_1; mm++) {
    for (nn = 0; nn < NY_1; nn++) {
      for (pp = 0; pp < NZ_0; pp++) {
#pragma HLS DEPENDENCE class = array dependent = true direction =              \
    raw                              distance = 1 type = inter
#pragma HLS PIPELINE                 II                = 1
        int n1       = nn + 1;
        int m1       = mm + 1;
        int idx      = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_ex_n = (mm * NY_0 + n1) * NZ_0 + pp;
        int idx_ex   = (mm * NY_0 + nn) * NZ_0 + pp;
        int idx_ey_m = (m1 * NY_1 + nn) * NZ_0 + pp;
        int idx_ey   = (mm * NY_1 + nn) * NZ_0 + pp;

        hz[idx] = hz[idx] + coeff_div * ((ex[idx_ex_n] - ex[idx_ex]) -
                                         (ey[idx_ey_m] - ey[idx_ey]));
      }
    }
  }
};

void updateEx(float *__restrict__ ex, float *__restrict__ hz,
              float *__restrict__ hy) {
#pragma HLS INLINE off
  int mm, nn, pp;
  for (mm = 0; mm < NX_1; mm++) {
    for (nn = 1; nn < NY_1; nn++) {
      for (pp = 1; pp < NZ_1; pp++) {
#pragma HLS DEPENDENCE class = array dependent = true direction =              \
    raw                              distance = 1 type = inter
#pragma HLS PIPELINE                 II                = 1
        int idx = (mm * NY_0 + nn) * NZ_0 + pp;
        if (isDipole(mm, nn, pp)) {
          ex[idx] = 0.0f;
          continue;
        }

        int _n       = nn - 1;
        int _p       = pp - 1;
        int idx_hz   = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_hz_n = (mm * NY_1 + _n) * NZ_0 + pp;
        int idx_hy   = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_hy_p = (mm * NY_0 + nn) * NZ_1 + _p;

        ex[idx] = ex[idx] + coeff_mul * ((hz[idx_hz] - hz[idx_hz_n]) -
                                         (hy[idx_hy] - hy[idx_hy_p]));
      }
    }
  }
}

void updateEy(float *__restrict__ ey, float *__restrict__ hx,
              float *__restrict__ hz) {
#pragma HLS INLINE off
  int mm, nn, pp;
  for (mm = 1; mm < NX_1; mm++) {
    for (nn = 0; nn < NY_1; nn++) {
      for (pp = 1; pp < NZ_1; pp++) {
#pragma HLS DEPENDENCE class = array dependent = true direction =              \
    raw                              distance = 1 type = inter
#pragma HLS PIPELINE                 II                = 1
        int idx = (mm * NY_1 + nn) * NZ_0 + pp;
        if (isDipole(mm, nn, pp)) {
          ey[idx] = 0.0f;
          continue;
        }

        int _p       = pp - 1;
        int _m       = mm - 1;
        int idx_hx   = (mm * NY_1 + nn) * NZ_1 + pp;
        int idx_hx_m = (mm * NY_1 + nn) * NZ_1 + _p;
        int idx_hz   = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_hz_m = (_m * NY_1 + nn) * NZ_0 + pp;

        ey[idx] = ey[idx] + coeff_mul * ((hx[idx_hx] - hx[idx_hx_m]) -
                                         (hz[idx_hz] - hz[idx_hz_m]));
      }
    }
  }
}

void updateEz(float *__restrict__ ez, float *__restrict__ hy,
              float *__restrict__ hx) {
#pragma HLS INLINE off
  int mm, nn, pp;
  for (mm = 1; mm < NX_1; mm++) {
    for (nn = 1; nn < NY_1; nn++) {
      for (pp = 0; pp < NZ_1; pp++) {
#pragma HLS DEPENDENCE class = array dependent = true direction =              \
    raw                              distance = 1 type = inter
#pragma HLS PIPELINE                 II                = 1
        int _m       = mm - 1;
        int _n       = nn - 1;
        int idx      = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_hy   = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_hy_m = (_m * NY_0 + nn) * NZ_1 + pp;
        int idx_hx   = (mm * NY_1 + nn) * NZ_1 + pp;
        int idx_hx_n = (mm * NY_1 + _n) * NZ_1 + pp;

        ez[idx] = ez[idx] + coeff_mul * ((hy[idx_hy] - hy[idx_hy_m]) -
                                         (hx[idx_hx] - hx[idx_hx_n]));
      }
    }
  }
}

void updateH(float *__restrict__ hx, float *__restrict__ hy,
             float *__restrict__ hz, float *__restrict__ ex,
             float *__restrict__ ey, float *__restrict__ ez) {
  updateHx(hx, ey, ez);
  updateHy(hy, ez, ex);
  updateHz(hz, ex, ey);
}

void updateE(float *__restrict__ hx, float *__restrict__ hy,
             float *__restrict__ hz, float *__restrict__ ex,
             float *__restrict__ ey, float *__restrict__ ez) {
  updateEx(ex, hz, hy);
  updateEy(ey, hx, hz);
  updateEz(ez, hy, hx);
}

void fdtd(float *hx, float *hy, float *hz, float *ex, float *ey, float *ez) {
// #pragma HLS DATAFLOW

#pragma HLS INTERFACE m_axi port = hx offset = slave bundle = gmem0 depth=HX_BUFFER
#pragma HLS INTERFACE m_axi port = hy offset = slave bundle = gmem1 depth=HY_BUFFER
#pragma HLS INTERFACE m_axi port = hz offset = slave bundle = gmem2 depth=HZ_BUFFER
#pragma HLS INTERFACE m_axi port = ex offset = slave bundle = gmem3 depth=EX_BUFFER
#pragma HLS INTERFACE m_axi port = ey offset = slave bundle = gmem4 depth=EY_BUFFER
#pragma HLS INTERFACE m_axi port = ez offset = slave bundle = gmem5 depth=EZ_BUFFER

#pragma HLS INTERFACE s_axilite port = hx bundle = control
#pragma HLS INTERFACE s_axilite port = hy bundle = control
#pragma HLS INTERFACE s_axilite port = hz bundle = control
#pragma HLS INTERFACE s_axilite port = ex bundle = control
#pragma HLS INTERFACE s_axilite port = ey bundle = control
#pragma HLS INTERFACE s_axilite port = ez bundle = control

  updateH(hx, hy, hz, ex, ey, ez);
  updateE(hx, hy, hz, ex, ey, ez);
}
