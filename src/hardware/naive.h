#ifndef NAIVE_H
#define NAIVE_H

typedef struct {
  double *hx, *chxh, *chxe;
  double *hy, *chyh, *chye;
  double *hz, *chzh, *chze;
  double *ex, *cexe, *cexh;
  double *ey, *ceye, *ceyh;
  double *ez, *ceze, *cezh;
} GridIntern;

typedef struct {
  int sizeX, sizeY, sizeZ;
  double cdtds;
  double imp0;
  int time;
} GridExtern;

typedef enum {
  ThreeDimension,
} GridType;

typedef struct {
  GridType type;
  GridExtern external;
  GridIntern internal;
} Grid;

#endif // NAIVE_H
