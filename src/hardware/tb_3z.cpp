#include "def.h"
#include <cmath>
#include <cstdio>

extern void fdtd(float *hx_gmem, float *hy_gmem, float *hz_gmem, float *ex_gmem,
                 float *ey_gmem, float *ez_gmem);

static inline void addExcitation(float ez[NZ_1][NY_0][NX_0], int t_step) {
  const float amplitude = 1.0f;
  const float frequency = 0.1f;
  const float phase_sin = 0.0f;
  const float sourceing =
      amplitude *
      sinf(2.0f * (float)M_PI * frequency * (float)t_step + phase_sin);

  ez[DIPOLE_CENTER_Z][DIPOLE_CENTER_Y][DIPOLE_CENTER_X] += sourceing;
}

int main() {
  static float HX[NZ_1][NY_1][NX_0] = {0};
  static float HY[NZ_1][NY_0][NX_1] = {0};
  static float HZ[NZ_0][NY_1][NX_1] = {0};
  static float EX[NZ_0][NY_0][NX_1] = {0};
  static float EY[NZ_0][NY_1][NX_0] = {0};
  static float EZ[NZ_1][NY_0][NX_0] = {0};

  for (int t = 0; t < TIME_M; ++t) {
    fdtd(&HX[0][0][0], &HY[0][0][0], &HZ[0][0][0], &EX[0][0][0], &EY[0][0][0],
         &EZ[0][0][0]);
    addExcitation(EZ, t);
    if (t == 41) {
      int pp = 42;
      for (int mm = 1; mm < NX_1; ++mm) {
        for (int nn = 1; nn < NY_1; ++nn) {
          float          point    = EZ[pp][nn][mm];
          unsigned char *byte_ptr = (unsigned char *)&point;
          std::printf("[%2d, %2d, %2d] float: %9.6f byte ", mm, nn, pp, point);
          for (size_t i = 0; i < sizeof(float); ++i) {
            std::printf("%02X ", byte_ptr[i]);
          }
          std::printf("\n");
        }
      }
    }
  }

  return 0;
}
