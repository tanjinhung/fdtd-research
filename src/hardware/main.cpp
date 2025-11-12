#include "naive.c"
#include <cmath>
#include <cstdlib>
#include <matplot/matplot.h>
#include <stdlib.h>

extern void fdtd(float *hx, float *hy, float *hz, float *ex, float *ey,
                 float *ez, int max_time);

static float hx_buffer[HX_BUFFER] = {0};
static float hy_buffer[HY_BUFFER] = {0};
static float hz_buffer[HZ_BUFFER] = {0};
static float ex_buffer[EX_BUFFER] = {0};
static float ey_buffer[EY_BUFFER] = {0};
static float ez_buffer[EZ_BUFFER] = {0};

void show_slice_ez(float *ez) {
  using namespace matplot;
  int  z_mid  = NZ_1 / 2;
  auto xv     = iota(0, 1.0, NX_1);
  auto yv     = iota(0, 1.0, NY_1);
  auto [X, Y] = meshgrid(xv, yv);

  double scale = 1e5;

  auto Z = transform(X, Y, [&](double xd, double yd) {
    int    x   = static_cast<int>(std::lround(xd));
    int    y   = static_cast<int>(std::lround(yd));
    size_t idx = (x * NY_0 + y) * NZ_1 + z_mid;
    return static_cast<double>(std::abs(ez[idx]) * scale);
  });

  surf(X, Y, Z);
  colorbar();
  colormap(palette::jet());
  axis("image");
  xlabel("X");
  ylabel("Y");
  title("Ez field magnitude at Z mid-plane (scaled by " +
        std::to_string(static_cast<int>(scale)) + ")");
  view(45, 30);

  show();
}

int main(int argc, char *argv[]) {
  fdtd(hx_buffer, hy_buffer, hz_buffer, ex_buffer, ey_buffer, ez_buffer, 50);

  show_slice_ez(ez_buffer);

  return EXIT_SUCCESS;
}
