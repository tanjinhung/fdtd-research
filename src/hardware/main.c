#include "naive.c"
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>

extern void fdtd(float *hx, float *hy, float *hz, float *ex, float *ey,
                 float *ez, int max_time);

static float hx_buffer[HX_BUFFER] = {0};
static float hy_buffer[HY_BUFFER] = {0};
static float hz_buffer[HZ_BUFFER] = {0};
static float ex_buffer[EX_BUFFER] = {0};
static float ey_buffer[EY_BUFFER] = {0};
static float ez_buffer[EZ_BUFFER] = {0};

int main(int argc, char *argv[]) {
  fdtd(hx_buffer, hy_buffer, hz_buffer, ex_buffer, ey_buffer, ez_buffer, 100);

  // show_slice_ez(ez_buffer);

  return EXIT_SUCCESS;
}
