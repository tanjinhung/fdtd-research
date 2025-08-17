#include "../headers/fdtd.h"

static int temporalStride = -2, frame = 0, startTime, startNodeX, endNodeX,
           spatialStrideX, startNodeY, endNodeY, spatialStrideY;
static char basename[80];

void snapshotInit2d(Grid *g) {
  int choice;

  printf("2D Snapshot? (1=Yes, 0=No) ");
  scanf("%d", &choice);
  if (choice != 1) {
    temporalStride = -1;
    return;
  }

  printf("Duration of simulation is %d steps.\n", MaxTime);
  printf("Enter the start time and temporalStride: ");
  scanf(" %d %d", &startTime, &temporalStride);
  printf("In x direction grid has %d total nodes (ranging from 0 .. %d).\n",
         SizeX, SizeX - 1);
  printf("Enter the first and last node and spatialStride: ");
  scanf(" %d %d %d", &startNodeX, &endNodeX, &spatialStrideX);
  printf("In y direction grid has %d total nodes (ranging from 0 .. %d).\n",
         SizeY, SizeY - 1);
  printf("Enter the first and last node and spatialStride: ");
  scanf(" %d %d %d", &startNodeY, &endNodeY, &spatialStrideY);
  printf("Enter the base filename: ");
  scanf(" %s", basename);

  return;
}

void snapshot2d(Grid *g) {
  int mm = 0, nn = 0;
  float dim1, dim2, temp;
  char filename[100];
  FILE *out;

  if (temporalStride == -1)
    return;
  if (temporalStride < -1) {
    fprintf(stderr,
            "[snapshot2d] snapshotInit2d must be called before screenshot.\n"
            "             Temporal Stride must be set to positive value.\n");
    exit(EXIT_FAILURE);
  }

  if (Time >= startTime && (Time - startTime) % temporalStride == 0) {
    sprintf(filename, "%s.%d.dat", basename, frame++);
    out = fopen(filename, "wb");

    dim1 = (endNodeX - startNodeX) / spatialStrideX + 1;
    dim2 = (endNodeY - startNodeY) / spatialStrideY + 1;
    fwrite(&dim1, sizeof(float), 1, out);
    fwrite(&dim2, sizeof(float), 1, out);

    // printf("First cell: %f\n", Ez(mm, nn));
    // printf("Grid cell number: %d\n", (mm) * (SizeY - 1) + nn);
    // printf("%d %d %d\n", endNodeY, startNodeY, spatialStrideY);
    // printf("%d %d %d\n", endNodeX, startNodeX, spatialStrideX);

    for (nn = endNodeY; nn >= startNodeY; nn -= spatialStrideY) {
      for (mm = startNodeX; mm <= endNodeX; mm += spatialStrideX) {
        temp = (float)Ez(mm, nn);
        // printf("mm: %d nn: %d\n", mm, nn);
        // printf("Grid cell number: %d\n", (mm) * (SizeY - 1) + nn);
        // printf("%f ", temp);
        fwrite(&temp, sizeof(float), 1, out);
      }
    }
    printf("\n");

    fclose(out);
  }

  return;
}
