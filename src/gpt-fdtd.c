#include "raylib.h"
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

// Linear map: v ∈ [d0,d1] → [r0,r1]
static float MapValue(float v, float d0, float d1, float r0, float r1) {
  float t = (v - d0) / (d1 - d0);
  return r0 + t * (r1 - r0);
}

// Draws grid + axes + line‐plot of `data[0..count-1]`
// x spans [xMin,xMax], y spans [yMin,yMax].
static void DrawPlotData(Rectangle rect, float xMin, float xMax, float yMin,
                         float yMax, int count, const float *data,
                         Color gridColor, Color axisColor, Color plotColor) {
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

  // 2) AXES at x=0 or y=0 if in range
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

  // 3) CONNECT DATA POINTS
  if (count < 2)
    return;
  for (int i = 1; i < count; i++) {
    // map indices → x
    float x0 = xMin + (xMax - xMin) * ((i - 1) / (float)(count - 1));
    float x1 = xMin + (xMax - xMin) * (i / (float)(count - 1));
    // map y-values
    float sx0 = MapValue(x0, xMin, xMax, rect.x, rect.x + rect.width);
    float sy0 = MapValue(data[i - 1], yMin, yMax, rect.y + rect.height, rect.y);
    float sx1 = MapValue(x1, xMin, xMax, rect.x, rect.x + rect.width);
    float sy1 = MapValue(data[i], yMin, yMax, rect.y + rect.height, rect.y);
    DrawLine((int)sx0, (int)sy0, (int)sx1, (int)sy1, plotColor);
  }
}

#define NX 200
#define MAX_SAMPLES 250

int main(void) {
  const int screenW = 1200, screenH = 900;
  InitWindow(screenW, screenH, "1D FDTD + Real-Time Plot");
  SetTargetFPS(60);

  // --- FDTD fields and buffers ---
  static float Ez[NX] = {0};
  static float Hy[NX] = {0};
  static float samples[MAX_SAMPLES] = {0};
  int sampleCount = 0;

  const float dx = 1.0f;
  const float dt = 0.5f; // for stability dt/dx ≤ 1
  float timeAccum = 0.0f;
  int stepCount = 0;

  while (!WindowShouldClose()) {
    // --- 1D FDTD update ---
    for (int i = 0; i < NX - 1; i++)
      Hy[i] += (Ez[i + 1] - Ez[i]) * (dt / dx);

    for (int i = 1; i < NX; i++)
      Ez[i] += (Hy[i] - Hy[i - 1]) * (dt / dx);

    // inject a sinusoidal source at the center
    int src = NX / 2;
    float freq = 0.01f; // adjust to taste
    Ez[0] = exp(-((stepCount - 30.0f) * (stepCount - 30.0f)) / 100.0f);

    // --- record time-history at src ---
    if (sampleCount < MAX_SAMPLES) {
      samples[sampleCount++] = Ez[src];
    } else {
      // shift left
      for (int i = 0; i < MAX_SAMPLES - 1; i++)
        samples[i] = samples[i + 1];
      samples[MAX_SAMPLES - 1] = Ez[src];
    }

    // advance simulation time
    stepCount++;
    timeAccum += dt;

    // --- DRAW ---
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // 1) spatial profile of Ez
    DrawPlotData((Rectangle){50, 50, 700, 200}, 0,
                 NX - 1,      // x from cell-0 to cell-(NX-1)
                 -0.5f, 0.5f, // y range
                 NX, Ez, LIGHTGRAY, DARKGRAY, BLUE);

    DrawText("Ez(x)", 60, 55, 12, DARKGRAY);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
