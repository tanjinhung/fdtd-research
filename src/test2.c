#include "raylib.h"
#include <math.h>
#include <stdlib.h>

// Map v ∈ [d0,d1] → [r0,r1]
static float MapValue(float v, float d0, float d1, float r0, float r1) {
  return r0 + (v - d0) / (d1 - d0) * (r1 - r0);
}

// Draw a grid + axes + line-plot of `data[0..count-1]`
// x runs from xMin→xMax, y from yMin→yMax.
void DrawPlotData(Rectangle rect, float xMin, float xMax, float yMin,
                  float yMax, int count, const float *data, Color gridColor,
                  Color axisColor, Color plotColor) {
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

  // 2) AXES at x=0 or y=0 if visible
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

  // 3) PLOT DATA
  if (count < 2)
    return;
  for (int i = 1; i < count; i++) {
    // data index → x value
    float x0 = xMin + (xMax - xMin) * ((i - 1) / (float)(count - 1));
    float x1 = xMin + (xMax - xMin) * (i / (float)(count - 1));
    // map to screen
    float sx0 = MapValue(x0, xMin, xMax, rect.x, rect.x + rect.width);
    float sy0 = MapValue(data[i - 1], yMin, yMax, rect.y + rect.height, rect.y);
    float sx1 = MapValue(x1, xMin, xMax, rect.x, rect.x + rect.width);
    float sy1 = MapValue(data[i], yMin, yMax, rect.y + rect.height, rect.y);
    DrawLine((int)sx0, (int)sy0, (int)sx1, (int)sy1, plotColor);
  }
}

#define MAX_SAMPLES 200

int main(void) {
  const int W = 800, H = 600;
  InitWindow(W, H, "Plot From Array");
  SetTargetFPS(60);

  // circular buffer of last MAX_SAMPLES values
  float samples[MAX_SAMPLES] = {0};
  int sampleCount = 0;

  while (!WindowShouldClose()) {
    // --- update your samples here ---
    // e.g. push a new sine-wave value:
    float t = (float)GetTime();
    float v = sinf(t * 2.0f);
    if (sampleCount < MAX_SAMPLES) {
      samples[sampleCount++] = v;
    } else {
      // shift left by one
      for (int i = 0; i < MAX_SAMPLES - 1; i++)
        samples[i] = samples[i + 1];

      samples[MAX_SAMPLES - 1] = v;
      sampleCount = MAX_SAMPLES;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawPlotData((Rectangle){50, 50, 700, 500}, 0,
                 (sampleCount > 1 ? sampleCount - 1 : 1), // x domain
                 -1, 1,                                   // y domain
                 sampleCount, samples, LIGHTGRAY, DARKGRAY, RED);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
