// #include <ap_fixed.h>

#define MAX_TIME  50
#define IMP0      377.0
#define CDTDS     0.57735026918962576 // Precomputed value of 1/sqrt(3)
#define NX_0      8
#define NY_0      8
#define NZ_0      8
#define NX_1      (NX_0 - 1)
#define NY_1      (NY_0 - 1)
#define NZ_1      (NZ_0 - 1)
#define HX_BUFFER (NX_0 * NY_1 * NZ_1)
#define HY_BUFFER (NX_1 * NY_0 * NZ_1)
#define HZ_BUFFER (NX_1 * NY_1 * NZ_0)
#define EX_BUFFER (NX_1 * NY_0 * NZ_0)
#define EY_BUFFER (NX_0 * NY_1 * NZ_0)
#define EZ_BUFFER (NX_0 * NY_0 * NZ_1)

static float coeff_div = CDTDS / IMP0;
static float coeff_mul = CDTDS * IMP0;

void updateH(float *__restrict__ hx, float *__restrict__ hy,
             float *__restrict__ hz, float *__restrict__ ex,
             float *__restrict__ ey, float *__restrict__ ez) {
#pragma HLS INLINE
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

        hx[idx] = hx[idx] + coeff_div * ((ey[idx_ey_p] - ey[idx_ey]) -
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

        hy[idx] = hy[idx] + coeff_div * ((ez[idx_ez_m] - ez[idx_ez]) -
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

        hz[idx] = hz[idx] + coeff_div * ((ex[idx_ex_n] - ex[idx_ex]) -
                                         (ey[idx_ey_m] - ey[idx_ey]));
      }
    }
  }
}

void updateE(float *__restrict__ hx, float *__restrict__ hy,
             float *__restrict__ hz, float *__restrict__ ex,
             float *__restrict__ ey, float *__restrict__ ez) {
#pragma HLS INLINE
  int mm, nn, pp;

  for (mm = 0; mm < NX_1; mm++) {
    for (nn = 1; nn < NY_1; nn++) {
      for (pp = 1; pp < NZ_1; pp++) {
#pragma HLS PIPELINE II = 1
        int _n       = nn - 1;
        int _p       = pp - 1;
        int idx      = (mm * NY_0 + nn) * NZ_0 + pp;
        int idx_hz   = (mm * NY_1 + nn) * NZ_0 + pp;
        int idx_hz_n = (mm * NY_1 + _n) * NZ_0 + pp;
        int idx_hy   = (mm * NY_0 + nn) * NZ_1 + pp;
        int idx_hy_p = (mm * NY_0 + nn) * NZ_1 + _p;

        ex[idx] = ex[idx] + coeff_mul * ((hz[idx_hz] - hz[idx_hz_n]) -
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

        ey[idx] = ey[idx] + coeff_mul * ((hx[idx_hx] - hx[idx_hx_m]) -
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

        ez[idx] = ez[idx] + coeff_mul * ((hy[idx_hy] - hy[idx_hy_m]) -
                                         (hx[idx_hx] - hx[idx_hx_n]));
      }
    }
  }
}

void fdtd(float *hx, float *hy, float *hz, float *ex, float *ey, float *ez,
          int n_time) {
#pragma HLS INTERFACE m_axi port = hx offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = hy offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = hz offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = ex offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = ey offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = ez offset = slave bundle = gmem

#pragma HLS INTERFACE s_axilite port = hx bundle = control
#pragma HLS INTERFACE s_axilite port = hy bundle = control
#pragma HLS INTERFACE s_axilite port = hz bundle = control
#pragma HLS INTERFACE s_axilite port = ex bundle = control
#pragma HLS INTERFACE s_axilite port = ey bundle = control
#pragma HLS INTERFACE s_axilite port = ez bundle = control
#pragma HLS INTERFACE s_axilite port = n_time bundle = control

#pragma HLS INTERFACE s_axilite port = return bundle = control

  for (int t = 0; t < n_time; t++) {
#pragma HLS PIPELINE
    updateH(hx, hy, hz, ex, ey, ez);
    updateE(hx, hy, hz, ex, ey, ez);
  }
}
