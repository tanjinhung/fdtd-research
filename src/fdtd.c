#include <math.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "../nob.h"
#include "raylib.h"

#define SIZE 200
#define MAX_TIME 250

void simulate_fdtd(double ez[], double hy[], double impedence0) {
  int qTime, mm;
  for (qTime = 0; qTime < MAX_TIME; qTime++) {
    for (mm = 0; mm < SIZE - 1; mm++) {
      hy[mm] += (ez[mm + 1] - ez[mm]) / impedence0;
    }

    for (mm = 1; mm < SIZE; mm++) {
      ez[mm] += (hy[mm] - hy[mm - 1]) * impedence0;
    }

    ez[0] = exp(-(qTime - 30.) * (qTime - 30.) / 100.0);

    if (qTime % 10 == 0) {
      printf("Time step %d: ", qTime);
      printf("%g\n", ez[50]);
    }
  }
}

int main() {
  double ez[SIZE] = {0.};
  double hy[SIZE] = {0.};
  double impedence0 = 377.0;
  const int screenWidth = 800;
  const int screenHeight = 600;

  InitWindow(screenWidth, screenHeight, "FDTD Simulation");
  SetTargetFPS(60);

  // hy[m] = hy[m] + (ez[m + 1] - ez[m]) / impedence0;
  // ez[m] = ez[m] + (hy[m] - hy[m - 1]) * impedence0;
  simulate_fdtd(ez, hy, impedence0);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
