#ifndef _MACRO_H_
#define _MACRO_H_

#include "grid.h"

// macros that permit the "Grid" to be specified
// 1D
#define Hy1G(G, M) G->hy[M]
#define Chyh1G(G, M) G->chyh[M]
#define Chye1G(G, M) G->chye[M]

#define Ez1G(G, M) G->ez[M]
#define Cezh1G(G, M) G->cezh[M]
#define Ceze1G(G, M) G->ceze[M]

// 2D TMz
#define HxG(G, M, N) G->hx[(M) * (SizeYG(G) - 1) + (N)]
#define ChxhG(G, M, N) G->chxh[(M) * (SizeYG(G) - 1) + (N)]
#define ChxeG(G, M, N) G->chxe[(M) * (SizeYG(G) - 1) + (N)]

#define HyG(G, M, N) G->hy[(M) * SizeYG(G) + (N)]
#define ChyhG(G, M, N) G->chyh[(M) * SizeYG(G) + (N)]
#define ChyeG(G, M, N) G->chye[(M) * SizeYG(G) + (N)]

#define EzG(G, M, N) G->ez[(M) * SizeYG(G) + (N)]
#define CezeG(G, M, N) G->ceze[(M) * SizeYG(G) + (N)]
#define CezhG(G, M, N) G->cezh[(M) * SizeYG(G) + (N)]

#define SizeXG(G) (G->sizeX)
#define SizeYG(G) (G->sizeY)
#define SizeZG(G) (G->sizeZ)
#define TimeG(G) (G->time)
#define MaxTimeG(G) (G->maxTime)
#define CdtdsG(G) (G->cdtds)
#define TypeG(G) (G->type)

// macros that assume the "Grid" is "g"
// 1D
#define Hy1(M) Hy1G(g, M)
#define Chyh1(M) Chyh1G(g, M)
#define Chye1(M) Chye1G(g, M)

#define Ez1(M) Ez1G(g, M)
#define Cezh1(M) Cezh1G(g, M)
#define Ceze1(M) Ceze1G(g, M)

// 2D TMz
#define Hx(M, N) HxG(g, M, N)
#define Chxh(M, N) ChxhG(g, M, N)
#define Chxe(M, N) ChxeG(g, M, N)

#define Hy(M, N) HyG(g, M, N)
#define Chyh(M, N) ChyhG(g, M, N)
#define Chye(M, N) ChyeG(g, M, N)

#define Ez(M, N) EzG(g, M, N)
#define Ceze(M, N) CezeG(g, M, N)
#define Cezh(M, N) CezhG(g, M, N)

#define SizeX SizeXG(g)
#define SizeY SizeYG(g)
#define SizeZ SizeZG(g)
#define Time TimeG(g)
#define MaxTime MaxTimeG(g)
#define Cdtds CdtdsG(g)
#define Type TypeG(g)

#endif // !_MACRO_H_
