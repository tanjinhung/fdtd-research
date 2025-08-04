#include <math.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "../../nob.h"
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

void Draw2DGrid(int pos_x, int pos_y, int width, int height, double spacing,
                Color color) {
  for (int x = pos_x; x <= pos_x + width; x += spacing) {
    DrawLine((int)x, pos_y, (int)x, pos_y + height, color);
  }
  for (int y = pos_y; y <= pos_y + height; y += spacing) {
    DrawLine(pos_x, (int)y, pos_x + width, (int)y, color);
  }
}

static inline double map_value(double v, double d0, double d1, double r0, double r1) {
  return r0 + (v - d0) / (d1 - d0) * (r1 - r0);
}

void DrawPlot(Rectangle rect, double xMin, double xMax, double yMin, double yMax,
              int count, const double *data, Color gridColor, Color plotColor) {
  const int xTicks = 10, yTicks = 10;

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

  static double ez[SIZE] = {0.};
  static double hy[SIZE] = {0.};
  static double impedence0 = 377.0;
  int qTime = 0;
  int mm;
  // Source Position
  int src = 0;

  // hy[m] = hy[m] + (ez[m + 1] - ez[m]) / impedence0;
  // ez[m] = ez[m] + (hy[m] - hy[m - 1]) * impedence0;
  // ez[0] = exp(-(qTime - 30.) * (qTime - 30.) / 100.0);
  // simulate_fdtd(ez, hy, impedence0);

  while (!WindowShouldClose()) {
    double maxEz = 1.0f;
    double minEz = -1.0f;

    for (mm = 0; mm < SIZE - 1; mm++)
      hy[mm] += (ez[mm + 1] - ez[mm]) / impedence0;

    for (mm = 1; mm < SIZE; mm++)
      ez[mm] += (hy[mm] - hy[mm - 1]) * impedence0;

    ez[src] = exp(-(qTime - 30.0f) * (qTime - 30.0f) / 100.0f);

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawPlot((Rectangle){50, 100, screenWidth - 100, screenHeight - 200}, 0,
             SIZE - 1, minEz, maxEz, SIZE, ez, LIGHTGRAY, RED);

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
