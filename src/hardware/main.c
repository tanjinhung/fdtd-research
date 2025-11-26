#include "naive.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

extern void fdtd(float *hx, float *hy, float *hz, float *ex, float *ey,
                 float *ez);

void addExcitation(float *__restrict__ ez, int t_step) {
  float t0     = 20.0f;
  float spread = 10.0f;
  float t_norm = (float)t_step - t0;
  float source = expf(-0.5f * (t_norm / spread) * (t_norm / spread));

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

  for (int t_step = 0; t_step < 100; t_step++) {
    fdtd(hx_buffer, hy_buffer, hz_buffer, ex_buffer, ey_buffer, ez_buffer);
    addExcitation(ez_buffer, t_step);
    if (t_step == 41) {
      FILE *out;
      out = fopen("out.b", "wb");
      if (!out) {
        perror("fopen");
        return 1;
      }

      fwrite("fdtd", sizeof(char), 4, out);
      int nx = NX_0;
      int ny = NY_0;
      int nz = NZ_0;
      fwrite(&nx, sizeof(int), 1, out);
      fwrite(&ny, sizeof(int), 1, out);
      fwrite(&nz, sizeof(int), 1, out);

      for (int mm = 1; mm < NX_1; mm++) {
        for (int nn = 1; nn < NY_1; nn++) {
          int   pp    = 42;
          int   idx   = (mm * NY_0 + nn) * NZ_1 + pp;
          float point = ez_buffer[idx];
          fwrite(&point, sizeof(float), 1, out);
        }
      }

      fclose(out);
    }
  }

  return EXIT_SUCCESS;
}
