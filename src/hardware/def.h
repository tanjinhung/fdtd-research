#ifndef NAIVE_H_
#define NAIVE_H_

#define IMP0  377.0
#define CDTDS 0.57735026918962576 // Precomputed value of 1/sqrt(3)
#define NX_0  32
#define NY_0  32
#define NZ_0  70
#define NX_1  (NX_0 - 1)
#define NY_1  (NY_0 - 1)
#define NZ_1  (NZ_0 - 1)

#define HX_PLANER (NX_0 * NY_1)
#define HY_PLANER (NX_1 * NY_0)
#define HZ_PLANER (NX_1 * NY_1)

#define EX_PLANER (NX_1 * NY_0)
#define EY_PLANER (NX_0 * NY_1)
#define EZ_PLANER (NX_0 * NY_0)

#define HX_BUFFER (HX_PLANER * NZ_1)
#define HY_BUFFER (HY_PLANER * NZ_1)
#define HZ_BUFFER (HZ_PLANER * NZ_0)

#define EX_BUFFER (EX_PLANER * NZ_0)
#define EY_BUFFER (EY_PLANER * NZ_0)
#define EZ_BUFFER (EZ_PLANER * NZ_1)

#define TOTAL_BUF (NX_0 * NY_0 * NZ_0)
#define PLANE_BUF (NX_0 * NY_0)

// Dipole Constant
#define DIPOLE_CENTER_X (NX_0 / 2)
#define DIPOLE_CENTER_Y (NY_0 / 2)
#define DIPOLE_CENTER_Z (NZ_0 / 2)
#define DIPOLE_LENGTH   12
#define DIPOLE_RADIUS   2
#define FEED_GAP        2

#endif // !NAIVE_H_
