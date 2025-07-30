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

void Draw2DGrid(int pos_x, int pos_y, int width, int height, float spacing,
                Color color) {
  for (int x = pos_x; x <= pos_x + width; x += spacing) {
    DrawLine((int)x, pos_y, (int)x, pos_y + height, color);
  }
  for (int y = pos_y; y <= pos_y + height; y += spacing) {
    DrawLine(pos_x, (int)y, pos_x + width, (int)y, color);
  }
}

int main() {
  double ez[SIZE] = {0.};
  double hy[SIZE] = {0.};
  double impedence0 = 377.0;
  const int screenWidth = 800;
  const int screenHeight = 600;
  int qTime = 0;

  InitWindow(screenWidth, screenHeight, "FDTD Simulation");
  SetTargetFPS(60);

  // hy[m] = hy[m] + (ez[m + 1] - ez[m]) / impedence0;
  // ez[m] = ez[m] + (hy[m] - hy[m - 1]) * impedence0;
  simulate_fdtd(ez, hy, impedence0);

  while (!WindowShouldClose()) {
    if (qTime > MAX_TIME)
      qTime = 0;
    BeginDrawing();
    ClearBackground(RAYWHITE);
    Draw2DGrid(50, 150, screenWidth - 100, screenHeight - 200, 20.0f, BLACK);
    EndDrawing();
    qTime++;
  }

  CloseWindow();

  return 0;
}
