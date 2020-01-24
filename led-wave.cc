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

        // float a = 1.0f;

        int c = (count + x) % 3 * 255;
        int r = 0, g = 0, b = 0;

        if (c >= 0 && c <= 255) {
          r = c;
          b = 255 - c;
        } else if (c > 255 & c <= 511) {
          r = 512 - c;
          g = c - 255;
        } else if (c > 511 & c <= 765) {
          g = 766 - c;
          b = c - 511;
        }

        offscreen_canvas->SetPixel(x, y, r, g, b);

      }
    }
    
    count++;

    offscreen_canvas = canvas->SwapOnVSync(offscreen_canvas);

    sleep(1 / 10);

  }

  // 
  // 
  // 

  canvas->Clear();
  delete canvas;

  return 0;
}