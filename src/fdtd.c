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

static inline float map_value(float v, float d0, float d1, float r0, float r1) {
  return r0 + (v - d0) / (d1 - d0) * (r1 - r0);
}

void DrawPlot(Rectangle rect, float xMin, float xMax, float yMin, float yMax,
              int count, const float *data, Color gridColor, Color axisColor,
              Color plotColor) {
  const int xTicks = 10, yTicks = 10;

  // 1) GRID
  for (int i = 0; i <= xTicks; i++) {
    float x = xMin + (xMax - xMin) * (i / (float)xTicks);
    float sx = map_value(x, xMin, xMax, rect.x, rect.x + rect.width);
    DrawLine((int)sx, (int)rect.y, (int)sx, (int)(rect.y + rect.height),
             gridColor);
  }
  for (int j = 0; j <= yTicks; j++) {
    float y = yMin + (yMax - yMin) * (j / (float)yTicks);
    float sy = map_value(y, yMin, yMax, rect.y + rect.height, rect.y);
    DrawLine((int)rect.x, (int)sy, (int)(rect.x + rect.width), (int)sy,
             gridColor);
  }

  if (count < 2)
    return;
  for (int i = 0; i < count; i++) {
    float x0 = xMin + (xMax - xMin) * ((i - 1) / (float)(count - 1));
    float x1 = xMin + (xMax - xMin) * (i / (float)(count - 1));

    float sx0 = map_value(x0, xMin, xMax, rect.x, rect.x + rect.width);
    float sx1 = map_value(x1, xMin, xMax, rect.x, rect.x + rect.width);

    float y0 = data[i - 1];
    float y1 = data[i];

    float sy0 = map_value(y0, yMin, yMax, rect.y + rect.height, rect.y);
    float sy1 = map_value(y1, yMin, yMax, rect.y + rect.height, rect.y);

    DrawLine((int)sx0, (int)sy0, (int)sx1, (int)sy1, plotColor);
  }
}

int main() {
  const int screenWidth = 1200, screenHeight = 900;
  InitWindow(screenWidth, screenHeight, "FDTD Simulation");
  SetTargetFPS(60);

  static float ez[SIZE] = {0.};
  static float hy[SIZE] = {0.};
  static float impedence0 = 377.0;
  int qTime = 0;
  int mm;
  // Source Position
  int src = SIZE / 2;

  // hy[m] = hy[m] + (ez[m + 1] - ez[m]) / impedence0;
  // ez[m] = ez[m] + (hy[m] - hy[m - 1]) * impedence0;
  // ez[0] = exp(-(qTime - 30.) * (qTime - 30.) / 100.0);
  // simulate_fdtd(ez, hy, impedence0);

  while (!WindowShouldClose()) {
    if (qTime > MAX_TIME)
      qTime = 0;

    float maxEz = FP_INFINITE;
    float minEz = -FP_INFINITE;

    for (mm = 0; mm < SIZE - 1; mm++)
      hy[mm] += (ez[mm + 1] - ez[mm]) / impedence0;
    for (mm = 1; mm < SIZE; mm++) {
      ez[mm] += (hy[mm] - hy[mm - 1]) * impedence0;

      float e = ez[mm];
      if (e > maxEz)
        maxEz = e;
      if (e < minEz)
        minEz = e;
    }

    ez[src] = exp(-(qTime - 30.0f) * (qTime - 30.0f) / 100.0f);
    if (ez[src] > maxEz)
      maxEz = ez[src];
    if (ez[src] < minEz)
      minEz = ez[src];

    BeginDrawing();
    ClearBackground(RAYWHITE);

    float padY = (maxEz - minEz) * 0.1f;
    float yMin = minEz - padY;
    float yMax = maxEz + padY;

    DrawPlot((Rectangle){50, 100, screenWidth - 100, screenHeight - 200}, 0,
             SIZE - 1, yMin, yMax, SIZE, ez, LIGHTGRAY, DARKGRAY, RED);

    EndDrawing();
    qTime++;
  }

  CloseWindow();

  return 0;
}
