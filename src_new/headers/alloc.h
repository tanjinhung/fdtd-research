#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <stdio.h>
#include <stdlib.h>

// memory allocation macros
#define CALLOC_1D(PNTR, TYPE, SIZE)                                            \
  PNTR = (TYPE *)calloc(SIZE, sizeof(TYPE));                                   \
  if (!PNTR) {                                                                 \
    perror("CALLOC_1D");                                                       \
    fprintf(stderr, "Memory allocation failed for %s at line %d\n", #PNTR,     \
            __LINE__);                                                         \
    exit(EXIT_FAILURE);                                                        \
  }

#define CALLOC_2D(PNTR, TYPE, SIZE1, SIZE2)                                    \
  PNTR = (TYPE *)calloc((SIZE1) * (SIZE2), sizeof(TYPE));                      \
  if (!PNTR) {                                                                 \
    perror("CALLOC_2D");                                                       \
    fprintf(stderr, "Memory allocation failed for %s at line %d\n", #PNTR,     \
            __LINE__);                                                         \
    exit(EXIT_FAILURE);                                                        \
  }

#endif // !_ALLOC_H_
