#include "def.h"

static const float ce = CDTDS * IMP0;
static const float ch = CDTDS / IMP0;

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
    g[base + i] = p[i];
  }
}

static void update_H_crit(
    float *__restrict__ hx_plane, float *__restrict__ hy_plane,
    float *__restrict__ hz_plane, const float *__restrict__ ex_plane,
    const float *__restrict__ ey_plane, const float *__restrict__ ez_plane,
    const float *__restrict__ ex_plus1, const float *__restrict__ ey_plus1) {
#pragma HLS INLINE off
  // HX
  for (int y = 0; y < NY_1; ++y) {
    for (int x = 0; x < NX_0; ++x) {
#pragma HLS PIPELINE II = 1
      const int   ihx  = y * NX_0 + x;
      const int   iezy = (y + 1) * NX_0 + x;
      const float dey  = ey_plus1[ihx] - ey_plane[ihx];
      const float dez  = ez_plane[iezy] - ez_plane[ihx];
      hx_plane[ihx] += ch * (dey - dez);
    }
  }

  // HY
  for (int y = 0; y < NY_0; ++y) {
    for (int x = 0; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
      const int   ihy  = y * NX_1 + x;
      const int   iez  = y * NX_0 + x;
      const int   iezx = y * NX_0 + (x + 1);
      const float dez  = ez_plane[iezx] - ez_plane[iez];
      const float dex  = ex_plus1[ihy] - ex_plane[ihy];
      hy_plane[ihy] += ch * (dez - dex);
    }
  }

  // HZ
  for (int y = 0; y < NY_1; ++y) {
    for (int x = 0; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
      const int   ihz  = y * NX_1 + x;
      const int   iexy = (y + 1) * NX_1 + x;
      const int   iey  = y * NX_0 + x;
      const int   ieyx = y * NX_0 + (x + 1);
      const float dex  = ex_plane[iexy] - ex_plane[ihz];
      const float dey  = ey_plane[ieyx] - ey_plane[iey];
      hz_plane[ihz] += ch * (dex - dey);
    }
  }
}

static void update_E_crit(
    float *__restrict__ ex_plane, float *__restrict__ ey_plane,
    float *__restrict__ ez_plane, const float *__restrict__ hx_plane,
    const float *__restrict__ hy_plane, const float *__restrict__ hz_plane,
    const float *__restrict__ hx_prev1, const float *__restrict__ hy_prev1) {
#pragma HLS INLINE off

  // EX
  for (int y = 1; y < NY_1; ++y) {
    for (int x = 0; x < NX_1; ++x) {
      const int   iex  = y * NX_1 + x;
      const int   ihzy = (y - 1) * NX_1 + x;
      const float dhz  = hz_plane[iex] - hz_plane[ihzy];
      const float dhy  = hy_plane[iex] - hy_prev1[iex];
      ex_plane[iex] += ce * (dhz - dhy);
    }
  }

  // EY
  for (int y = 0; y < NY_1; ++y) {
    for (int x = 1; x < NX_1; ++x) {
      const int   iey  = y * NX_0 + x;
      const int   ihz  = y * NX_1 + x;
      const int   ihzx = y * NX_1 + (x - 1);
      const float dhx  = hx_plane[iey] - hx_prev1[iey];
      const float dhz  = hz_plane[ihz] - hz_plane[ihzx];
      ey_plane[iey] += ce * (dhx - dhz);
    }
  }

  // EZ
  for (int y = 1; y < NY_1; ++y) {
    for (int x = 1; x < NX_1; ++x) {
      const int   iez  = y * NX_0 + x;
      const int   ihy  = y * NX_1 + x;
      const int   ihyx = y * NX_1 + (x - 1);
      const int   ihxy = (y - 1) * NX_0 + x;
      const float dhy  = hy_plane[ihy] - hy_plane[ihyx];
      const float dhx  = hx_plane[iez] - hx_plane[ihxy];
      ez_plane[iez] += ce * (dhy - dhx);
    }
  }
}

static void update_H_tail(float *__restrict__ hz_plane,
                          const float *__restrict__ ex_plane,
                          const float *__restrict__ ey_plane) {
#pragma HLS INLINE off
  // HZ
  for (int y = 0; y < NY_1; ++y) {
    for (int x = 0; x < NX_1; ++x) {
#pragma HLS PIPELINE II = 1
      const int   ihz  = y * NX_1 + x;
      const int   iexy = (y + 1) * NX_1 + x;
      const int   iey  = y * NX_0 + x;
      const int   ieyx = y * NX_0 + (x + 1);
      const float dex  = ex_plane[iexy] - ex_plane[ihz];
      const float dey  = ey_plane[ieyx] - ey_plane[iey];
      hz_plane[ihz] += ch * (dex - dey);
    }
  }
}

static void update_E_tail(float *__restrict__ ex_plane,
                          float *__restrict__ ey_plane,
                          const float *__restrict__ hz_plane,
                          const float *__restrict__ hx_prev1,
                          const float *__restrict__ hy_prev1) {
#pragma HLS INLINE off

  // EX
  for (int y = 1; y < NY_1; ++y) {
    for (int x = 0; x < NX_1; ++x) {
      const int   iex  = y * NX_1 + x;
      const int   ihzy = (y - 1) * NX_1 + x;
      const float dhz  = hz_plane[iex] - hz_plane[ihzy];
      const float dhy  = -hy_prev1[iex];
      ex_plane[iex] += ce * (dhz - dhy);
    }
  }

  // EY
  for (int y = 0; y < NY_1; ++y) {
    for (int x = 1; x < NX_1; ++x) {
      const int   iey  = y * NX_0 + x;
      const int   ihz  = y * NX_1 + x;
      const int   ihzx = y * NX_1 + (x - 1);
      const float dhx  = -hx_prev1[iey];
      const float dhz  = hz_plane[ihz] - hz_plane[ihzx];
      ey_plane[iey] += ce * (dhx - dhz);
    }
  }
}

void fdtd(float *hx_gmem, float *hy_gmem, float *hz_gmem, float *ex_gmem,
          float *ey_gmem, float *ez_gmem) {

  // clang-format off
#pragma HLS INTERFACE m_axi port = hx_gmem offset = slave bundle= gmem0 depth = TOTAL_BUF
#pragma HLS INTERFACE m_axi port = hy_gmem offset = slave bundle= gmem0 depth = TOTAL_BUF 
#pragma HLS INTERFACE m_axi port = hz_gmem offset = slave bundle= gmem0 depth = TOTAL_BUF
#pragma HLS INTERFACE m_axi port = ex_gmem offset = slave bundle= gmem0 depth = TOTAL_BUF
#pragma HLS INTERFACE m_axi port = ey_gmem offset = slave bundle= gmem0 depth = TOTAL_BUF
#pragma HLS INTERFACE m_axi port = ez_gmem offset = slave bundle= gmem0 depth = TOTAL_BUF

#pragma HLS INTERFACE s_axilite port = return   bundle = control
  // clang-format on

  static float hx_plane[HX_PLANER];
  static float hy_plane[HY_PLANER];
  static float hz_plane[HZ_PLANER];
  static float ex_plane[EX_PLANER];
  static float ey_plane[EY_PLANER];
  static float ez_plane[EZ_PLANER];

  static float ex_plus1[EX_PLANER];
  static float ey_plus1[EY_PLANER];
  static float hx_prev1[HX_PLANER];
  static float hy_prev1[HY_PLANER];

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

  for (int i = 0; i < HX_PLANER; ++i) {
#pragma HLS PIPELINE II = 1
    hx_prev1[i] = 0.0f;
  }

  for (int i = 0; i < HY_PLANER; ++i) {
#pragma HLS PIPELINE II = 1
    hy_prev1[i] = 0.0f;
  }

  for (int z = 0; z < NZ_1; ++z) {
    // #pragma HLS DATAFLOW
    rd_plane(ex_gmem, ex_plane, z, EX_PLANER);
    rd_plane(ey_gmem, ey_plane, z, EY_PLANER);
    rd_plane(ez_gmem, ez_plane, z, EZ_PLANER);
    rd_plane(ex_gmem, ex_plus1, z + 1, EX_PLANER);
    rd_plane(ey_gmem, ey_plus1, z + 1, EY_PLANER);

    rd_plane(hx_gmem, hx_plane, z, HX_PLANER);
    rd_plane(hy_gmem, hy_plane, z, HY_PLANER);
    rd_plane(hz_gmem, hz_plane, z, HZ_PLANER);

    update_H_crit(hx_plane, hy_plane, hz_plane, ex_plane, ey_plane, ez_plane,
                  ex_plus1, ey_plus1);

    wr_plane(hx_gmem, hx_plane, z, HX_PLANER);
    wr_plane(hy_gmem, hy_plane, z, HY_PLANER);
    wr_plane(hz_gmem, hz_plane, z, HZ_PLANER);

    update_E_crit(ex_plane, ey_plane, ez_plane, hx_plane, hy_plane, hz_plane,
                  hx_prev1, hy_prev1);

    wr_plane(ex_gmem, ex_plane, z, EX_PLANER);
    wr_plane(ey_gmem, ey_plane, z, EY_PLANER);
    wr_plane(ez_gmem, ez_plane, z, EZ_PLANER);

    for (int i = 0; i < HX_PLANER; ++i) {
#pragma HLS PIPELINE II = 1
      hx_prev1[i] = hx_plane[i];
    }

    for (int i = 0; i < HY_PLANER; ++i) {
#pragma HLS PIPELINE II = 1
      hy_prev1[i] = hy_plane[i];
    }
  }

  {
    const int z = NZ_1;

    rd_plane(ex_gmem, ex_plane, z, EX_PLANER);
    rd_plane(ey_gmem, ey_plane, z, EY_PLANER);
    rd_plane(hz_gmem, hz_plane, z, HZ_PLANER);

    update_H_tail(hz_plane, ex_plane, ey_plane);

    wr_plane(hz_gmem, hz_plane, z, HZ_PLANER);

    update_E_tail(ex_plane, ey_plane, hz_plane, hx_prev1, hy_prev1);

    wr_plane(ex_gmem, ex_plane, z, EX_PLANER);
    wr_plane(ey_gmem, ey_plane, z, EY_PLANER);
  }
}
