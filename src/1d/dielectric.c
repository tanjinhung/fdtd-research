#include <math.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "../../nob.h"
#include "raylib.h"

#define SIZE 200
#define MAX_TIME 450

static inline double map_value(double v, double d0, double d1, double r0, double r1) {
  return r0 + (v - d0) / (d1 - d0) * (r1 - r0);
}

void DrawPlot(Rectangle rect, double xMin, double xMax, double yMin, double yMax,
              int count, const double *data, Color gridColor, Color plotColor) {
  const int xTicks = xMax / 10, yTicks = yMax * 8;

  // 1) GRID
  for (int i = 0; i <= xTicks; i++) {
    double x = xMin + (xMax - xMin) * (i / (double)xTicks);
    double sx = map_value(x, xMin, xMax, rect.x, rect.x + rect.width);
    DrawLine((int)sx, (int)rect.y, (int)sx, (int)(rect.y + rect.height),
             gridColor);
  }
  for (int j = 0; j <= yTicks; j++) {
    double y = yMin + (yMax - yMin) * (j / (double)yTicks);
    double sy = map_value(y, yMin, yMax, rect.y + rect.height, rect.y);
    DrawLine((int)rect.x, (int)sy, (int)(rect.x + rect.width), (int)sy,
             gridColor);
  }

  if (count < 2)
    return;
  for (int i = 0; i < count; i++) {
    double x0 = xMin + (xMax - xMin) * ((i - 1) / (double)(count - 1));
    double x1 = xMin + (xMax - xMin) * (i / (double)(count - 1));

    double sx0 = map_value(x0, xMin, xMax, rect.x, rect.x + rect.width);
    double sx1 = map_value(x1, xMin, xMax, rect.x, rect.x + rect.width);

    double y0 = data[i - 1];
    double y1 = data[i];

    double sy0 = map_value(y0, yMin, yMax, rect.y + rect.height, rect.y);
    double sy1 = map_value(y1, yMin, yMax, rect.y + rect.height, rect.y);

    DrawLine((int)sx0, (int)sy0, (int)sx1, (int)sy1, plotColor);
  }
}

int main() {
  const int screenWidth = 1200, screenHeight = 900;
  InitWindow(screenWidth, screenHeight, "FDTD Simulation");
  SetTargetFPS(60);

  static double ez[SIZE];
  static double hy[SIZE];
  static double epsR[SIZE];
  static double impedence0 = 377.0;
  int qTime = 0, mm;
  int src = 50;

  for (mm = 0; mm < SIZE; mm++) {
    ez[mm] = 0.0f;
  }

  for (mm = 0; mm < SIZE - 1; mm++) {
    hy[mm] = 0.0f;
  }

  for (mm = 0; mm < SIZE; mm++) {
    if (mm < 100)
      epsR[mm] = 1.0f; // Free space
    else
      epsR[mm] = 9.0f;
  }

  while (!WindowShouldClose()) {
    double maxEz = 1.0f;
    double minEz = -maxEz;

    // magnetic field update
    hy[SIZE - 1] = hy[SIZE - 2];

    for (mm = 0; mm < SIZE - 1; mm++)
      hy[mm] += (ez[mm + 1] - ez[mm]) / impedence0;

    hy[src - 1] -=
        exp(-(qTime - 30.0f) * (qTime - 30.0f) / 100.0f) / impedence0;

    // electric field update
    ez[0] = ez[1];
    ez[SIZE - 1] = ez[SIZE - 2];

    for (mm = 1; mm < SIZE; mm++)
      ez[mm] += (hy[mm] - hy[mm - 1]) * impedence0 / epsR[mm];

    // Source
    ez[src] += exp(-(qTime + 0.5f - (-0.5f) - 30.0f) *
                   (qTime + 0.5f - (-0.5f) - 30.0f) / 100.0f);

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawPlot((Rectangle){50, 100, screenWidth - 100, screenHeight - 200}, 0,
             SIZE, minEz, maxEz, SIZE, ez, LIGHTGRAY, RED);

    EndDrawing();
    qTime++;
    if (qTime >= MAX_TIME) {
      qTime = 0;
      for (mm = 0; mm < SIZE; mm++) {
        ez[mm] = 0.0f;
        hy[mm] = 0.0f;
      }
    }
  }

  CloseWindow();

  return 0;
}
