#ifndef _FDTD_H_
#define _FDTD_H_

#include "alloc.h"
#include "grid.h"
#include "macro.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* function prototypes */
void grid_init(Grid *g);

void snapshotInit2d(Grid *g);
void snapshot2d(Grid *g);

void updateE2d(Grid *g);
void updateH2d(Grid *g);

/* function for ezinc */
void ezIncInit(Grid *g);
double ezInc(double time, double location);

#endif // !_FDTD_H_
