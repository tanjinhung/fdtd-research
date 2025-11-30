#include "def.h"
#include "hls_stream.h"
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
#pragma HLS DEPENDENCE variable = hx class = array inter false
// #pragma HLS DEPENDENCE class = array dependent = true direction = \
//     raw                              distance = 1 type = inter
#pragma HLS PIPELINE II = 2
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
#pragma HLS DEPENDENCE variable = hy class = array inter false
// #pragma HLS DEPENDENCE class = array dependent = true direction = \
//     raw                              distance = 1 type = inter
#pragma HLS PIPELINE II = 2
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
#pragma HLS DEPENDENCE variable = hz class = array inter false
// #pragma HLS DEPENDENCE class = array dependent = true direction = \
//     raw                              distance = 1 type = inter
#pragma HLS PIPELINE II = 2
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
#pragma HLS DEPENDENCE variable = ex class = array inter false
// #pragma HLS DEPENDENCE class = array dependent = true direction = \
//     raw                              distance = 1 type = inter
#pragma HLS PIPELINE II = 2
        float updateVal = 0;
        int   idx       = (mm * NY_0 + nn) * NZ_0 + pp;

        if (!isDipole(mm, nn, pp)) {
          int _n       = nn - 1;
          int _p       = pp - 1;
          int idx_hz   = (mm * NY_1 + nn) * NZ_0 + pp;
          int idx_hz_n = (mm * NY_1 + _n) * NZ_0 + pp;
          int idx_hy   = (mm * NY_0 + nn) * NZ_1 + pp;
          int idx_hy_p = (mm * NY_0 + nn) * NZ_1 + _p;

          updateVal = ex[idx] + coeff_mul * ((hz[idx_hz] - hz[idx_hz_n]) -
                                             (hy[idx_hy] - hy[idx_hy_p]));
        }

        ex[idx] = updateVal;
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
#pragma HLS DEPENDENCE variable = ey class = array inter false
// #pragma HLS DEPENDENCE class = array dependent = true direction = \
//     raw                              distance = 1 type = inter
#pragma HLS PIPELINE II = 2
        float updateVal = 0;
        int   idx       = (mm * NY_1 + nn) * NZ_0 + pp;

        if (!isDipole(mm, nn, pp)) {
          int _p       = pp - 1;
          int _m       = mm - 1;
          int idx_hx   = (mm * NY_1 + nn) * NZ_1 + pp;
          int idx_hx_m = (mm * NY_1 + nn) * NZ_1 + _p;
          int idx_hz   = (mm * NY_1 + nn) * NZ_0 + pp;
          int idx_hz_m = (_m * NY_1 + nn) * NZ_0 + pp;

          updateVal = ey[idx] + coeff_mul * ((hx[idx_hx] - hx[idx_hx_m]) -
                                             (hz[idx_hz] - hz[idx_hz_m]));
        }

        ey[idx] = updateVal;
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
#pragma HLS DEPENDENCE variable = ez class = array inter false
// #pragma HLS DEPENDENCE class = array dependent = true direction = \
//     raw                              distance = 1 type = inter
#pragma HLS PIPELINE II = 2
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
  // #pragma HLS DATAFLOW
  updateHx(hx, ey, ez);
  updateHy(hy, ez, ex);
  updateHz(hz, ex, ey);
}

void updateE(float *__restrict__ hx, float *__restrict__ hy,
             float *__restrict__ hz, float *__restrict__ ex,
             float *__restrict__ ey, float *__restrict__ ez) {
  // #pragma HLS DATAFLOW
  updateEx(ex, hz, hy);
  updateEy(ey, hx, hz);
  updateEz(ez, hy, hx);
}

///////////////////////////////////////////////////////////////////////////////
// HLS::stream implementations
///////////////////////////////////////////////////////////////////////////////
const char bundle_name_0[] = "gmem0";
const char bundle_name_1[] = "gmem1";
const char bundle_name_2[] = "gmem2";
const char bundle_name_3[] = "gmem3";
const char bundle_name_4[] = "gmem4";
const char bundle_name_5[] = "gmem5";

template <typename T, const char *BundleName, int Size>
void                        gmem_to_stream_template(T *__restrict__ gmem_ptr,
                                                    hls::stream<T> &strm_ptr) {
#pragma HLS PIPELINE        II   = 1
#pragma HLS INTERFACE m_axi port = gmem_ptr offset = slave bundle = BundleName
  for (int i = 0; i < Size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = Size max = Size
    strm_ptr.write(gmem_ptr[i]);
  }
}

template <typename T, const char *BundleName, int Size>
void                        stream_to_gmem_template(T *__restrict__ gmem_ptr,
                                                    hls::stream<T> &strm_ptr) {
#pragma HLS PIPELINE        II   = 1
#pragma HLS INTERFACE m_axi port = gmem_ptr offset = slave bundle = BundleName
  for (int i = 0; i < Size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = Size max = Size
    gmem_ptr[i] = strm_ptr.read();
  }
}

void read_to_stream(float *__restrict__ hx_gmem, float *__restrict__ hy_gmem,
                    float *__restrict__ hz_gmem, float *__restrict__ ex_gmem,
                    float *__restrict__ ey_gmem, float *__restrict__ ez_gmem,
                    hls::stream<float> &hx_strm, hls::stream<float> &hy_strm,
                    hls::stream<float> &hz_strm, hls::stream<float> &ex_strm,
                    hls::stream<float> &ey_strm, hls::stream<float> &ez_strm) {
#pragma HLS INLINE off
#pragma HLS DATAFLOW

  gmem_to_stream_template<float, bundle_name_0, HX_BUFFER>(hx_gmem, hx_strm);
  gmem_to_stream_template<float, bundle_name_1, HY_BUFFER>(hy_gmem, hy_strm);
  gmem_to_stream_template<float, bundle_name_2, HZ_BUFFER>(hz_gmem, hz_strm);
  gmem_to_stream_template<float, bundle_name_3, EX_BUFFER>(ex_gmem, ex_strm);
  gmem_to_stream_template<float, bundle_name_4, EY_BUFFER>(ey_gmem, ey_strm);
  gmem_to_stream_template<float, bundle_name_5, EZ_BUFFER>(ez_gmem, ez_strm);
}

void write_to_gmemd(float *__restrict__ hx_gmem, float *__restrict__ hy_gmem,
                    float *__restrict__ hz_gmem, float *__restrict__ ex_gmem,
                    float *__restrict__ ey_gmem, float *__restrict__ ez_gmem,
                    hls::stream<float> &hx_strm, hls::stream<float> &hy_strm,
                    hls::stream<float> &hz_strm, hls::stream<float> &ex_strm,
                    hls::stream<float> &ey_strm, hls::stream<float> &ez_strm) {
#pragma HLS INLINE off
#pragma HLS DATAFLOW
  stream_to_gmem_template<float, bundle_name_0, HX_BUFFER>(hx_gmem, hx_strm);
  stream_to_gmem_template<float, bundle_name_1, HY_BUFFER>(hy_gmem, hy_strm);
  stream_to_gmem_template<float, bundle_name_2, HZ_BUFFER>(hz_gmem, hz_strm);
  stream_to_gmem_template<float, bundle_name_3, EX_BUFFER>(ex_gmem, ex_strm);
  stream_to_gmem_template<float, bundle_name_4, EY_BUFFER>(ey_gmem, ey_strm);
  stream_to_gmem_template<float, bundle_name_5, EZ_BUFFER>(ez_gmem, ez_strm);
}

void fdtd(float *hx_gmem, float *hy_gmem, float *hz_gmem, float *ex_gmem,
          float *ey_gmem, float *ez_gmem) {
#pragma HLS DATAFLOW

  hls::stream<float, 64> hx_initial_stream("hx_initial_stream");
  hls::stream<float, 64> hy_initial_stream("hy_initial_stream");
  hls::stream<float, 64> hz_initial_stream("hz_initial_stream");
  hls::stream<float, 64> ex_initial_stream("ex_initial_stream");
  hls::stream<float, 64> ey_initial_stream("ey_initial_stream");
  hls::stream<float, 64> ez_initial_stream("ez_initial_stream");
  hls::stream<float, 64> hx_to_ex_stream("hx_to_ex_stream");
  hls::stream<float, 64> hy_to_ey_stream("hy_to_ey_stream");
  hls::stream<float, 64> hz_to_ez_stream("hz_to_ez_stream");
  hls::stream<float, 64> hx_output_stream("hx_output_stream");
  hls::stream<float, 64> hy_output_stream("hy_output_stream");
  hls::stream<float, 64> hz_output_stream("hz_output_stream");
  hls::stream<float, 64> ex_output_stream("ex_output_stream");
  hls::stream<float, 64> ey_output_stream("ey_output_stream");
  hls::stream<float, 64> ez_output_stream("ez_output_stream");

  // clang-format off
#pragma HLS INTERFACE m_axi port = hx_gmem offset = slave bundle = gmem0 depth = HX_BUFFER
#pragma HLS INTERFACE m_axi port = hy_gmem offset = slave bundle = gmem1 depth = HY_BUFFER
#pragma HLS INTERFACE m_axi port = hz_gmem offset = slave bundle = gmem2 depth = HZ_BUFFER
#pragma HLS INTERFACE m_axi port = ex_gmem offset = slave bundle = gmem3 depth = EX_BUFFER
#pragma HLS INTERFACE m_axi port = ey_gmem offset = slave bundle = gmem4 depth = EY_BUFFER
#pragma HLS INTERFACE m_axi port = ez_gmem offset = slave bundle = gmem5 depth = EZ_BUFFER
  // clang-format on

#pragma HLS INTERFACE s_axilite port = hx_gmem bundle = control
#pragma HLS INTERFACE s_axilite port = hy_gmem bundle = control
#pragma HLS INTERFACE s_axilite port = hz_gmem bundle = control
#pragma HLS INTERFACE s_axilite port = ex_gmem bundle = control
#pragma HLS INTERFACE s_axilite port = ey_gmem bundle = control
#pragma HLS INTERFACE s_axilite port = ez_gmem bundle = control

#pragma HLS INTERFACE s_axilite port = return bundle = control

  read_to_stream(hx_gmem, hy_gmem, hz_gmem, ex_gmem, ey_gmem, ez_gmem,
                 hx_initial_stream, hy_initial_stream, hz_initial_stream,
                 ex_initial_stream, ey_initial_stream, ez_initial_stream);

  write_to_gmemd(hx_gmem, hy_gmem, hz_gmem, ex_gmem, ey_gmem, ez_gmem,
                 hx_output_stream, hy_output_stream, hz_output_stream,
                 ex_output_stream, ey_output_stream, ez_output_stream);
}
