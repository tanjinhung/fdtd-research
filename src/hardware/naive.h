#ifndef NAIVE_H
#define NAIVE_H

#define MAX_TIME  300
#define IMP0      377.0
#define CDTDS     0.57735026918962576 // Precomputed value of 1/sqrt(3)
#define NX_0      16
#define NY_0      16
#define NZ_0      16
#define NX_1      (NX_0 - 1)
#define NY_1      (NY_0 - 1)
#define NZ_1      (NZ_0 - 1)
#define HX_BUFFER (NX_0 * NY_1 * NZ_1)
#define HY_BUFFER (NX_1 * NY_0 * NZ_1)
#define HZ_BUFFER (NX_1 * NY_1 * NZ_0)
#define EX_BUFFER (NX_1 * NY_0 * NZ_0)
#define EY_BUFFER (NX_0 * NY_1 * NZ_0)
#define EZ_BUFFER (NX_0 * NY_0 * NZ_1)

typedef struct {
  float *hx;
  float *hy;
  float *hz;
  float *ex;
  float *ey;
  float *ez;
} GridIntern;

typedef struct {
  int    sizeX, sizeY, sizeZ;
  double cdtds;
  double imp0;
  int    time;
} GridExtern;

typedef enum {
  ThreeDimension,
} GridType;

typedef struct {
  GridType   type;
  int        time;
  GridIntern internal;
} Grid;

static float coeff_div = CDTDS / IMP0;
static float coeff_mul = CDTDS * IMP0;

#endif // NAIVE_H
