#include "def.h"

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

static void update_H_crit(
    float hx_plane[NY_1][NX_0], float hy_plane[NY_0][NX_1],
    float hz_plane[NY_1][NX_1], const float ex_plane[NY_0][NX_1],
    const float ey_plane[NY_1][NX_0], const float ez_plane[NY_0][NX_0],
    const float ex_plus1[NY_0][NX_1], const float ey_plus1[NY_1][NX_0]) {
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

  // HY
  for (int y = 0; y < NY_0; ++y) {
    for (int x = 0; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
      const float dez = ez_plane[y][x + 1] - ez_plane[y][x];
      const float dex = ex_plus1[y][x] - ex_plane[y][x];
      hy_plane[y][x] += ch * (dez - dex);
    }
  }

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

static void update_E_crit(
    float ex_plane[NY_0][NX_1], float ey_plane[NY_1][NX_0],
    float ez_plane[NY_0][NX_0], const float hx_plane[NY_1][NX_0],
    const float hy_plane[NY_0][NX_1], const float hz_plane[NY_1][NX_1],
    const float hx_prev1[NY_1][NX_0], const float hy_prev1[NY_0][NX_1]) {
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

  // EY
  for (int y = 0; y < NY_1; ++y) {
    for (int x = 1; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
      const float dhx = hx_plane[y][x] - hx_prev1[y][x];
      const float dhz = hz_plane[y][x] - hz_plane[y][x - 1];
      ey_plane[y][x] += ce * (dhx - dhz);
    }
  }

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

static void update_H_tail(float       hz_plane[NY_1][NX_1],
                          const float ex_plane[NY_0][NX_1],
                          const float ey_plane[NY_1][NX_0]) {
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

static void update_E_tail(float       ex_plane[NY_0][NX_1],
                          float       ey_plane[NY_1][NX_0],
                          const float hz_plane[NY_1][NX_1],
                          const float hx_prev1[NY_1][NX_0],
                          const float hy_prev1[NY_0][NX_1]) {
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

  static float hx_plane[NY_1][NX_0];
  static float hy_plane[NY_0][NX_1];
  static float hz_plane[NY_1][NX_1];
  static float ex_plane[NY_0][NX_1];
  static float ey_plane[NY_1][NX_0];
  static float ez_plane[NY_0][NX_0];

  static float ex_plus1[NY_0][NX_1];
  static float ey_plus1[NY_1][NX_0];
  static float hx_prev1[NY_1][NX_0];
  static float hy_prev1[NY_0][NX_1];

#pragma HLS BIND_STORAGE variable = hx_plane type = ram_2p impl = bram
#pragma HLS BIND_STORAGE variable = hy_plane type = ram_2p impl = bram
#pragma HLS BIND_STORAGE variable = hz_plane type = ram_2p impl = bram
#pragma HLS BIND_STORAGE variable = ex_plane type = ram_2p impl = bram
#pragma HLS BIND_STORAGE variable = ey_plane type = ram_2p impl = bram
#pragma HLS BIND_STORAGE variable = ez_plane type = ram_2p impl = bram
#pragma HLS BIND_STORAGE variable = ex_plus1 type = ram_2p impl = bram
#pragma HLS BIND_STORAGE variable = ey_plus1 type = ram_2p impl = bram
#pragma HLS BIND_STORAGE variable = hx_prev1 type = ram_2p impl = bram
#pragma HLS BIND_STORAGE variable = hy_prev1 type = ram_2p impl = bram

  // clang-format off
  // #pragma HLS ARRAY_PARTITION variable = ex_plane cyclic factor = PAR_FACTOR dim = 2
  // #pragma HLS ARRAY_PARTITION variable = ey_plane cyclic factor = PAR_FACTOR dim = 2
  // #pragma HLS ARRAY_PARTITION variable = ez_plane cyclic factor = PAR_FACTOR dim = 2
  // #pragma HLS ARRAY_PARTITION variable = hx_plane cyclic factor = PAR_FACTOR dim = 2
  // #pragma HLS ARRAY_PARTITION variable = hy_plane cyclic factor = PAR_FACTOR dim = 2
  // #pragma HLS ARRAY_PARTITION variable = hz_plane cyclic factor = PAR_FACTOR dim = 2
  // #pragma HLS ARRAY_PARTITION variable = ex_plus1 cyclic factor = PAR_FACTOR dim = 2
  // #pragma HLS ARRAY_PARTITION variable = ey_plus1 cyclic factor = PAR_FACTOR dim = 2
  // #pragma HLS ARRAY_PARTITION variable = hx_prev1 cyclic factor = PAR_FACTOR dim = 2 
  // #pragma HLS ARRAY_PARTITION variable = hy_prev1 cyclic factor = PAR_FACTOR dim = 2
  // clang-format on

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

  for (int z = 0; z < NZ_1; ++z) {
    rd_plane(ex_gmem, &ex_plane[0][0], z, EX_PLANER);
    rd_plane(ey_gmem, &ey_plane[0][0], z, EY_PLANER);
    rd_plane(ez_gmem, &ez_plane[0][0], z, EZ_PLANER);
    rd_plane(ex_gmem, &ex_plus1[0][0], z + 1, EX_PLANER);
    rd_plane(ey_gmem, &ey_plus1[0][0], z + 1, EY_PLANER);

    rd_plane(hx_gmem, &hx_plane[0][0], z, HX_PLANER);
    rd_plane(hy_gmem, &hy_plane[0][0], z, HY_PLANER);
    rd_plane(hz_gmem, &hz_plane[0][0], z, HZ_PLANER);

    update_H_crit(hx_plane, hy_plane, hz_plane, ex_plane, ey_plane, ez_plane,
                  ex_plus1, ey_plus1);

    wr_plane(hx_gmem, &hx_plane[0][0], z, HX_PLANER);
    wr_plane(hy_gmem, &hy_plane[0][0], z, HY_PLANER);
    wr_plane(hz_gmem, &hz_plane[0][0], z, HZ_PLANER);

    update_E_crit(ex_plane, ey_plane, ez_plane, hx_plane, hy_plane, hz_plane,
                  hx_prev1, hy_prev1);

    for (int y = 1; y < NY_1; ++y) {
      for (int x = 1; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
        if (isDipole(x, y, z)) {
          ex_plane[y][x] = 0.0f;
          ey_plane[y][x] = 0.0f;
        }
      }
    }

    wr_plane(ex_gmem, &ex_plane[0][0], z, EX_PLANER);
    wr_plane(ey_gmem, &ey_plane[0][0], z, EY_PLANER);
    wr_plane(ez_gmem, &ez_plane[0][0], z, EZ_PLANER);

    {
      float *hx_p1loc = &hx_prev1[0][0];
      float *hx_plloc = &hx_plane[0][0];
      float *hy_p1loc = &hy_prev1[0][0];
      float *hy_plloc = &hy_plane[0][0];
      for (int i = 0; i < HX_PLANER; ++i) {
#pragma HLS PIPELINE II = 1
        hx_p1loc[i] = hx_plloc[i];
      }
      for (int i = 0; i < HY_PLANER; ++i) {
#pragma HLS PIPELINE II = 1
        hy_p1loc[i] = hy_plloc[i];
      }
    }
  }

  {
    const int z = NZ_1;

    rd_plane(ex_gmem, &ex_plane[0][0], z, EX_PLANER);
    rd_plane(ey_gmem, &ey_plane[0][0], z, EY_PLANER);
    rd_plane(hz_gmem, &hz_plane[0][0], z, HZ_PLANER);

    update_H_tail(hz_plane, ex_plane, ey_plane);

    wr_plane(hz_gmem, &hz_plane[0][0], z, HZ_PLANER);

    update_E_tail(ex_plane, ey_plane, hz_plane, hx_prev1, hy_prev1);

    wr_plane(ex_gmem, &ex_plane[0][0], z, EX_PLANER);
    wr_plane(ey_gmem, &ey_plane[0][0], z, EY_PLANER);
  }
}
