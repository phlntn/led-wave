#include "led-matrix.h"
#include "graphics.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>

using namespace rgb_matrix;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

int main(int argc, char *argv[]) {
  RGBMatrix::Options defaults;

  defaults.hardware_mapping = "adafruit-hat-pwm";
  defaults.cols = 64;
  defaults.rows = 64;
  defaults.chain_length = 3;
  defaults.parallel = 1;
  defaults.show_refresh_rate = false;

  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  // 
  // 
  // 

  RGBMatrix *canvas = CreateMatrixFromFlags(&argc, &argv, &defaults);
  if (canvas == NULL) return 1;

  canvas->SetBrightness(100);

  FrameCanvas *offscreen_canvas = canvas->CreateFrameCanvas();

  const int width = canvas->width();
  const int height = canvas->height();

  uint32_t count = 0;

  while (!interrupt_received) {

    offscreen_canvas->Clear();

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {

        int cy = (count + y + x / 10) % (height * 2);

        if (cy > height) {
          cy = (height * 2) - cy;
        }

        float a = (float)cy / (float)height;
        a *= a;

        int cx = (count + x) % (3 * 255);
        int r = 0, g = 0, b = 0;

        if (cx <= 255) {
          b = 255 - cx;
          r = cx;
        } else if (cx <= 511) {
          r = 511 - cx;
          g = cx - 256;
        } else {
          g = 765 - cx;
          b = cx - 512;
        }

        offscreen_canvas->SetPixel(x, y, r * a, g * a, b * a);

      }
    }
    
    count++;

    offscreen_canvas = canvas->SwapOnVSync(offscreen_canvas);

    usleep(1000f / 30f);

  }

  // 
  // 
  // 

  canvas->Clear();
  delete canvas;

  return 0;
}