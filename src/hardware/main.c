#include "naive.h"
#include <math.h>
#include <stdlib.h>

extern void fdtd(float *hx, float *hy, float *hz, float *ex, float *ey,
                 float *ez);

static float hx_buffer[HX_BUFFER] = {0};
static float hy_buffer[HY_BUFFER] = {0};
static float hz_buffer[HZ_BUFFER] = {0};
static float ex_buffer[EX_BUFFER] = {0};
static float ey_buffer[EY_BUFFER] = {0};
static float ez_buffer[EZ_BUFFER] = {0};

void addExcitation(float *__restrict__ ez, int t_step) {
  float t0     = 20.0f;
  float spread = 10.0f;
  float t_norm = (float)t_step - t0;
  float source = expf(-0.5f * (t_norm / spread) * (t_norm / spread));

  int idx = (DIPOLE_CENTER_X * NY_0 + DIPOLE_CENTER_Y) * NZ_1 + DIPOLE_CENTER_Z;
  ez[idx] += source;
}

int main(int argc, char *argv[]) {

  for (int t_step = 0; t_step < 100; t_step++) {
    fdtd(hx_buffer, hy_buffer, hz_buffer, ex_buffer, ey_buffer, ez_buffer);
    addExcitation(ez_buffer, t_step);
    if (t_step % 10 == 0) {}
  }

  return EXIT_SUCCESS;
}
