#include "raylib.h"
#include <math.h>
#include <stdlib.h>

// Map v∈[d0,d1] → [r0,r1]
static float MapValue(float v, float d0, float d1, float r0, float r1) {
  return r0 + (v - d0) / (d1 - d0) * (r1 - r0);
}

// Draw a grid, axes, and the curve y=func(x)
void DrawPlot(Rectangle rect, float xMin, float xMax, float yMin, float yMax,
              int samples, Color gridColor, Color axisColor, Color plotColor,
              float (*func)(float)) {
  const int xTicks = 10, yTicks = 10;

  // 1) GRID
  for (int i = 0; i <= xTicks; i++) {
    float x = xMin + (xMax - xMin) * (i / (float)xTicks);
    float sx = MapValue(x, xMin, xMax, rect.x, rect.x + rect.width);
    DrawLine((int)sx, (int)rect.y, (int)sx, (int)(rect.y + rect.height),
             gridColor);
  }
  for (int j = 0; j <= yTicks; j++) {
    float y = yMin + (yMax - yMin) * (j / (float)yTicks);
    float sy = MapValue(y, yMin, yMax, rect.y + rect.height, rect.y);
    DrawLine((int)rect.x, (int)sy, (int)(rect.x + rect.width), (int)sy,
             gridColor);
  }

  // 2) AXES (if 0 lies inside range)
  if (yMin < 0 && yMax > 0) {
    float sy0 = MapValue(0, yMin, yMax, rect.y + rect.height, rect.y);
    DrawLine((int)rect.x, (int)sy0, (int)(rect.x + rect.width), (int)sy0,
             axisColor);
  }
  if (xMin < 0 && xMax > 0) {
    float sx0 = MapValue(0, xMin, xMax, rect.x, rect.x + rect.width);
    DrawLine((int)sx0, (int)rect.y, (int)sx0, (int)(rect.y + rect.height),
             axisColor);
  }

  // 3) SAMPLE THE FUNCTION AND DRAW
  Vector2 *pts = malloc(sizeof(Vector2) * (samples + 1));
  if (!pts)
    return;
  for (int i = 0; i <= samples; i++) {
    float t = i / (float)samples;
    float x = xMin + (xMax - xMin) * t;
    float y = func(x);
    float sx = MapValue(x, xMin, xMax, rect.x, rect.x + rect.width);
    float sy = MapValue(y, yMin, yMax, rect.y + rect.height, rect.y);
    pts[i] = (Vector2){sx, sy};
  }
  DrawLineStrip(pts, samples + 1, plotColor);
  free(pts);
}

int main(void) {
  const int W = 800, H = 600;
  InitWindow(W, H, "Pure C Raylib Plot");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Plot y=sin(x) over x∈[-PI,PI] in a box at (50,50) sized 700×500
    DrawPlot((Rectangle){50, 50, 700, 500}, -PI, PI, // x-range
             -1, 1,                                  // y-range
             500,                                    // # samples
             LIGHTGRAY, BLACK, RED, sinf);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
