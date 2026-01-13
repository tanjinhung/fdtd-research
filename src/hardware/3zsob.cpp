#include "def.h"
#include "hls_streamofblocks.h"

typedef struct PlaneBlock {
  float hx_plane[NY_1][NX_0];
  float hy_plane[NY_0][NX_1];
  float hz_plane[NY_1][NX_1];
  float ex_plane[NY_0][NX_1];
  float ey_plane[NY_1][NX_0];
  float ez_plane[NY_0][NX_0];
  float ex_plus1[NY_0][NX_1];
  float ey_plus1[NY_1][NX_0];
} PlaneBlock;

constexpr float ce = CDTDS * IMP0;
constexpr float ch = CDTDS / IMP0;

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

static void rd_plane(const float *g, float *p, int z, int len) {
#pragma HLS INLINE off
  const int base = z * len;
  for (int i = 0; i < len; ++i) {
#pragma HLS PIPELINE II = 1
    p[i] = g[base + i];
  }
}

static void wr_plane(float *g, const float *p, int z, int len) {
#pragma HLS INLINE off
  const int base = z * len;
  for (int i = 0; i < len; ++i) {
#pragma HLS PIPELINE II = 1
    g[base + i] = p[i];
  }
}

static void update_HX(float hx_plane[NY_1][NX_0], float ey_plus1[NY_1][NX_0],
                      float ey_plane[NY_1][NX_0], float ez_plane[NY_0][NX_0]) {
#pragma HLS INLINE off

  // HX
  for (int y = 0; y < NY_1; ++y) {
    for (int x = 0; x < NX_0; ++x) {
#pragma HLS PIPELINE II = 1
      const float dey = ey_plus1[y][x] - ey_plane[y][x];
      const float dez = ez_plane[y + 1][x] - ez_plane[y][x];
      hx_plane[y][x] += ch * (dey - dez);
    }
  }
}

static void update_HY(float hy_plane[NY_0][NX_1], float ez_plane[NY_0][NX_0],
                      float ex_plus1[NY_0][NX_1], float ex_plane[NY_0][NX_1]) {
#pragma HLS INLINE off

  // HY
  for (int y = 0; y < NY_0; ++y) {
    for (int x = 0; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
      const float dez = ez_plane[y][x + 1] - ez_plane[y][x];
      const float dex = ex_plus1[y][x] - ex_plane[y][x];
      hy_plane[y][x] += ch * (dez - dex);
    }
  }
}

static void update_HZ(float hz_plane[NY_1][NX_1], float ex_plane[NY_0][NX_1],
                      float ey_plane[NY_1][NX_0]) {
#pragma HLS INLINE off

  // HZ
  for (int y = 0; y < NY_1; ++y) {
    for (int x = 0; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
      const float dex = ex_plane[y + 1][x] - ex_plane[y][x];
      const float dey = ey_plane[y][x + 1] - ey_plane[y][x];
      hz_plane[y][x] += ch * (dex - dey);
    }
  }
}

static void update_H_crit(float hx[NY_1][NX_0], float hy[NY_0][NX_1],
                          float hz[NY_1][NX_1], float ex[NY_0][NX_1],
                          float ex1[NY_0][NX_1], float ey[NY_1][NX_0],
                          float ey1[NY_1][NX_0], float ez[NY_0][NX_0]) {
#pragma HLS INLINE off
  update_HX(hx, ey1, ey, ez);
  update_HY(hy, ez, ex1, ex);
  update_HZ(hz, ex, ey);
}

static void update_EX(float ex_plane[NY_0][NX_1], float hz_plane[NY_1][NX_1],
                      float hy_plane[NY_0][NX_1], float hy_prev1[NY_0][NX_1]) {
#pragma HLS INLINE off

  // EX
  for (int y = 1; y < NY_1; ++y) {
    for (int x = 0; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
      const float dhz = hz_plane[y][x] - hz_plane[y - 1][x];
      const float dhy = hy_plane[y][x] - hy_prev1[y][x];
      ex_plane[y][x] += ce * (dhz - dhy);
    }
  }
}

static void update_EY(float ey_plane[NY_1][NX_0], float hx_plane[NY_1][NX_0],
                      float hx_prev1[NY_1][NX_0], float hz_plane[NY_1][NX_1]) {
#pragma HLS INLINE off

  // EY
  for (int y = 0; y < NY_1; ++y) {
    for (int x = 1; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
      const float dhx = hx_plane[y][x] - hx_prev1[y][x];
      const float dhz = hz_plane[y][x] - hz_plane[y][x - 1];
      ey_plane[y][x] += ce * (dhx - dhz);
    }
  }
}

static void update_EZ(float ez_plane[NY_0][NX_0], float hy_plane[NY_0][NX_1],
                      float hx_plane[NY_1][NX_0]) {
#pragma HLS INLINE off

  // EZ
  for (int y = 1; y < NY_1; ++y) {
    for (int x = 1; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
      const float dhy = hy_plane[y][x] - hy_plane[y][x - 1];
      const float dhx = hx_plane[y][x] - hx_plane[y - 1][x];
      ez_plane[y][x] += ce * (dhy - dhx);
    }
  }
}

static void update_E_crit(float ex[NY_0][NX_1], float ey[NY_1][NX_0],
                          float ez[NY_0][NX_0], float hz[NY_1][NX_1],
                          float hy[NY_0][NX_1], float hy1[NY_0][NX_1],
                          float hx[NY_1][NX_0], float hx1[NY_1][NX_0]) {
#pragma HLS INLINE off
  update_EX(ex, hz, hy, hy1);
  update_EY(ey, hx, hx1, hz);
  update_EZ(ez, hy, hx);
}

static void update_H_tail(float hz[NY_1][NX_1], float ex[NY_0][NX_1],
                          float ey[NY_1][NX_0]) {
#pragma HLS INLINE off
  update_HZ(hz, ex, ey);
}

static void update_EX_tail(float ex_plane[NY_0][NX_1],
                           float hz_plane[NY_1][NX_1],
                           float hy_prev1[NY_0][NX_1]) {
#pragma HLS INLINE off

  // EX
  for (int y = 1; y < NY_1; ++y) {
    for (int x = 0; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
      const float dhz = hz_plane[y][x] - hz_plane[y - 1][x];
      const float dhy = -hy_prev1[y][x];
      ex_plane[y][x] += ce * (dhz - dhy);
    }
  }
}

static void update_EY_tail(float ey_plane[NY_1][NX_0],
                           float hx_prev1[NY_1][NX_0],
                           float hz_plane[NY_1][NX_1]) {
#pragma HLS INLINE off

  // EY
  for (int y = 0; y < NY_1; ++y) {
    for (int x = 1; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
      const float dhx = -hx_prev1[y][x];
      const float dhz = hz_plane[y][x] - hz_plane[y][x - 1];
      ey_plane[y][x] += ce * (dhx - dhz);
    }
  }
}

static void update_E_tail(float ex[NY_0][NX_1], float ey[NY_1][NX_0],
                          float hz[NY_1][NX_1], float hy1[NY_0][NX_1],
                          float hx1[NY_1][NX_0]) {
#pragma HLS INLINE off
  update_EX_tail(ex, hz, hy1);
  update_EY_tail(ey, hx1, hz);
}

static void check_dipole(float ex_plane[NY_0][NX_1], float ey_plane[NY_1][NX_0],
                         const int z) {
#pragma HLS INLINE off
  for (int y = 1; y < NY_1; ++y) {
    for (int x = 1; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
      if (isDipole(x, y, z)) {
        ex_plane[y][x] = 0.0f;
        ey_plane[y][x] = 0.0f;
      }
    }
  }
}

static void
stage_read(const float *__restrict__ hx_gmem, const float *__restrict__ hy_gmem,
           const float *__restrict__ hz_gmem, const float *__restrict__ ex_gmem,
           const float *__restrict__ ey_gmem, const float *__restrict__ ez_gmem,
           hls::stream_of_blocks<PlaneBlock> &read2comp, const int z) {
  if (z >= NZ_0 + 1) return;
  hls::write_lock<PlaneBlock> wlock(read2comp);
  PlaneBlock                 &out_data = wlock;

  if (z < NZ_1) {
    rd_plane(ex_gmem, &out_data.ex_plane[0][0], z, EX_PLANER);
    rd_plane(ey_gmem, &out_data.ey_plane[0][0], z, EY_PLANER);
    rd_plane(ez_gmem, &out_data.ez_plane[0][0], z, EZ_PLANER);
    rd_plane(ex_gmem, &out_data.ex_plus1[0][0], z + 1, EX_PLANER);
    rd_plane(ey_gmem, &out_data.ey_plus1[0][0], z + 1, EY_PLANER);
    rd_plane(hx_gmem, &out_data.hx_plane[0][0], z, HX_PLANER);
    rd_plane(hy_gmem, &out_data.hy_plane[0][0], z, HY_PLANER);
    rd_plane(hz_gmem, &out_data.hz_plane[0][0], z, HZ_PLANER);
  } else {
    rd_plane(ex_gmem, &out_data.ex_plane[0][0], z, EX_PLANER);
    rd_plane(ey_gmem, &out_data.ey_plane[0][0], z, EY_PLANER);
    rd_plane(hz_gmem, &out_data.hz_plane[0][0], z, HZ_PLANER);
  }
}

static void stage_compute(hls::stream_of_blocks<PlaneBlock> &read2comp,
                          hls::stream_of_blocks<PlaneBlock> &comp2wite,
                          float hx_prev1[NY_1][NX_0],
                          float hy_prev1[NY_0][NX_1], const int z) {
  if (z < 0 || z >= NZ_0 + 1) return;

  hls::read_lock<PlaneBlock>  rlock(read2comp);
  hls::write_lock<PlaneBlock> wlock(comp2wite);

  PlaneBlock &in_data  = rlock;
  PlaneBlock &out_data = wlock;

  out_data = in_data;

  if (z < NZ_1) {
    update_H_crit(out_data.hx_plane, out_data.hy_plane, out_data.hz_plane,
                  out_data.ex_plane, out_data.ex_plus1, out_data.ey_plane,
                  out_data.ey_plus1, out_data.ez_plane);
    update_E_crit(out_data.ex_plane, out_data.ey_plane, out_data.ez_plane,
                  out_data.hz_plane, out_data.hy_plane, hy_prev1,
                  out_data.hx_plane, hx_prev1);
    check_dipole(out_data.ex_plane, out_data.ey_plane, z);

    {
      float *hx_p1loc = &hx_prev1[0][0];
      float *hx_plloc = &out_data.hx_plane[0][0];
      float *hy_p1loc = &hy_prev1[0][0];
      float *hy_plloc = &out_data.hy_plane[0][0];
      for (int i = 0; i < HX_PLANER; ++i) {
#pragma HLS PIPELINE II = 1
        hx_p1loc[i] = hx_plloc[i];
      }
      for (int i = 0; i < HY_PLANER; ++i) {
#pragma HLS PIPELINE II = 1
        hy_p1loc[i] = hy_plloc[i];
      }
    }
  } else {
    update_H_tail(out_data.hz_plane, out_data.ex_plane, out_data.ey_plane);
    update_E_tail(out_data.ex_plane, out_data.ey_plane, out_data.hz_plane,
                  hy_prev1, hx_prev1);
  }
}

static void
stage_write(float *__restrict__ hx_gmem, float *__restrict__ hy_gmem,
            float *__restrict__ hz_gmem, float *__restrict__ ex_gmem,
            float *__restrict__ ey_gmem, float *__restrict__ ez_gmem,
            hls::stream_of_blocks<PlaneBlock> &comp2wite, const int z) {
  if (z < 0 || z >= NZ_0 + 1) return;
  hls::read_lock<PlaneBlock> rlock(comp2wite);
  PlaneBlock                 in_data = rlock;

  if (z < NZ_1) {
    wr_plane(hx_gmem, &in_data.hx_plane[0][0], z, HX_PLANER);
    wr_plane(hy_gmem, &in_data.hy_plane[0][0], z, HY_PLANER);
    wr_plane(hz_gmem, &in_data.hz_plane[0][0], z, HZ_PLANER);
    wr_plane(ex_gmem, &in_data.ex_plane[0][0], z, EX_PLANER);
    wr_plane(ey_gmem, &in_data.ey_plane[0][0], z, EY_PLANER);
    wr_plane(ez_gmem, &in_data.ez_plane[0][0], z, EZ_PLANER);
  } else {
    wr_plane(hz_gmem, &in_data.hz_plane[0][0], z, HZ_PLANER);
    wr_plane(ex_gmem, &in_data.ex_plane[0][0], z, EX_PLANER);
    wr_plane(ey_gmem, &in_data.ey_plane[0][0], z, EY_PLANER);
  }
}

void fdtd(float *__restrict__ hx_gmem, float *__restrict__ hy_gmem,
          float *__restrict__ hz_gmem, float *__restrict__ ex_gmem,
          float *__restrict__ ey_gmem, float *__restrict__ ez_gmem) {
  // clang-format off
#pragma HLS INTERFACE m_axi port = hx_gmem offset = slave bundle = gmemH depth = HX_BUFFER
#pragma HLS INTERFACE m_axi port = hy_gmem offset = slave bundle = gmemH depth = HY_BUFFER
#pragma HLS INTERFACE m_axi port = hz_gmem offset = slave bundle = gmemH depth = HZ_BUFFER
#pragma HLS INTERFACE m_axi port = ex_gmem offset = slave bundle = gmemE depth = EX_BUFFER
#pragma HLS INTERFACE m_axi port = ey_gmem offset = slave bundle = gmemE depth = EY_BUFFER
#pragma HLS INTERFACE m_axi port = ez_gmem offset = slave bundle = gmemE depth = EZ_BUFFER

#pragma HLS INTERFACE s_axilite port = return bundle = control
  // clang-format on

  hls::stream_of_blocks<PlaneBlock, 2> read2comp;
  hls::stream_of_blocks<PlaneBlock, 2> comp2wite;

  static float hx_prev1[NY_1][NX_0];
  static float hy_prev1[NY_0][NX_1];

  {
    float *hx_p1loc = &hx_prev1[0][0];
    float *hy_p1loc = &hy_prev1[0][0];
    for (int i = 0; i < HX_PLANER; ++i) {
#pragma HLS PIPELINE II = 1
      hx_p1loc[i] = 0.0f;
    }

    for (int i = 0; i < HY_PLANER; ++i) {
#pragma HLS PIPELINE II = 1
      hy_p1loc[i] = 0.0f;
    }
  }

  for (int z = 0; z < NZ_0 + 2; ++z) {
#pragma HLS DATAFLOW
    stage_read(hx_gmem, hy_gmem, hz_gmem, ex_gmem, ey_gmem, ez_gmem, read2comp,
               z);
    stage_compute(read2comp, comp2wite, hx_prev1, hy_prev1, z - 1);
    stage_write(hx_gmem, hy_gmem, hz_gmem, ex_gmem, ey_gmem, ez_gmem, comp2wite,
                z - 2);
  }
}
