#include "naive.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define NX       42
#define NY       42
#define NZ       42
#define MAX_TIME 300
#define NBUF     (NX * NY * NZ)

static double hx_buffer[NBUF]   = {0};
static double chxh_buffer[NBUF] = {0};
static double chxe_buffer[NBUF] = {0};
static double hy_buffer[NBUF]   = {0};
static double chyh_buffer[NBUF] = {0};
static double chye_buffer[NBUF] = {0};
static double hz_buffer[NBUF]   = {0};
static double chzh_buffer[NBUF] = {0};
static double chze_buffer[NBUF] = {0};
static double ex_buffer[NBUF]   = {0};
static double cexh_buffer[NBUF] = {0};
static double cexe_buffer[NBUF] = {0};
static double ey_buffer[NBUF]   = {0};
static double ceyh_buffer[NBUF] = {0};
static double ceye_buffer[NBUF] = {0};
static double ez_buffer[NBUF]   = {0};
static double cezh_buffer[NBUF] = {0};
static double ceze_buffer[NBUF] = {0};

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  printf("Naive hardware module initialized successfully.\n");

  // clang-format off
  GridIntern internal = {
      hx_buffer, chxe_buffer, chxh_buffer,
      hy_buffer, chye_buffer, chyh_buffer,
      hz_buffer, chze_buffer, chzh_buffer,
      ex_buffer, cexe_buffer, cexh_buffer,
      ey_buffer, ceye_buffer, ceyh_buffer,
      ez_buffer, ceze_buffer, cezh_buffer
  };
  // clang-format on

  GridExtern external = {NX, NY, NZ, 1.0 / sqrt(3.0), 377.0, 0};

  for (int i = 0; i < NBUF; ++i) {
    internal.chxh[i] = 1.0;
    internal.chxe[i] = external.cdtds / external.imp0;
    internal.chyh[i] = 1.0;
    internal.chye[i] = external.cdtds / external.imp0;
    internal.chzh[i] = 1.0;
    internal.chze[i] = external.cdtds / external.imp0;

    internal.cexe[i] = 1.0;
    internal.cexh[i] = external.cdtds * external.imp0;
    internal.ceye[i] = 1.0;
    internal.ceyh[i] = external.cdtds * external.imp0;
    internal.ceze[i] = 1.0;
    internal.cezh[i] = external.cdtds * external.imp0;
  }

  Grid g = {ThreeDimension, external, internal};

  return EXIT_SUCCESS;
}
