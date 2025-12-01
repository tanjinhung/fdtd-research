#include "def.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

extern void fdtd(float *hx, float *hy, float *hz, float *ex, float *ey,
                 float *ez);

void addExcitation(float *__restrict__ ez, int t_step) {
  // float t0     = 20.0f;
  // float spread = 10.0f;
  // float t_norm = (float)t_step - t0;
  // float source = expf(-0.5f * (t_norm / spread) * (t_norm / spread));
  float amplitude = 1.0f;
  float frequency = 0.1f;
  float phase     = 0.0f;
  float source =
      amplitude * sinf(2.0f * M_PI * frequency * (float)t_step + phase);

  int idx = (DIPOLE_CENTER_X * NY_0 + DIPOLE_CENTER_Y) * NZ_1 + DIPOLE_CENTER_Z;
  ez[idx] += source;
}

int main(int argc, char *argv[]) {
  static float hx_buffer[HX_BUFFER] = {0};
  static float hy_buffer[HY_BUFFER] = {0};
  static float hz_buffer[HZ_BUFFER] = {0};
  static float ex_buffer[EX_BUFFER] = {0};
  static float ey_buffer[EY_BUFFER] = {0};
  static float ez_buffer[EZ_BUFFER] = {0};

  for (int t_step = 0; t_step < 50; t_step++) {
    fdtd(hx_buffer, hy_buffer, hz_buffer, ex_buffer, ey_buffer, ez_buffer);
    addExcitation(ez_buffer, t_step);
    if (t_step == 41) {
      for (int mm = 1; mm < NX_1; mm++) {
        for (int nn = 1; nn < NY_1; nn++) {
          int            pp       = 42;
          int            idx      = (mm * NY_0 + nn) * NZ_1 + pp;
          float          point    = ez_buffer[idx];
          unsigned char *byte_ptr = (unsigned char *)&point;
          printf("[%2d, %2d, %2d] float: %9.6lf byte: ", mm, nn, pp, point);
          for (size_t i = 0; i < sizeof(float); i++) {
            printf("%02X ", byte_ptr[i]);
          }
          printf("\n");
        }
      }
    }
  }

  return EXIT_SUCCESS;
}
