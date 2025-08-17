#ifndef _GRID_H_
#define _GRID_H_

typedef enum {
  oneDimensional,
  twoDimensionalElectric,
  twoDimensionalMagnetic,
  threeDimensional,
} Grid_Type;

typedef struct {
  double *hx, *chxh, *chxe;
  double *hy, *chyh, *chye;
  double *hz, *chzh, *chze;
  double *ex, *cexh, *cexe;
  double *ey, *ceyh, *ceye;
  double *ez, *cezh, *ceze;
  int sizeX, sizeY, sizeZ;
  int time, maxTime;
  double cdtds;
  Grid_Type type;
} Grid;

#endif // !_GRID_H_
