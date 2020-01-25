#include "led-matrix.h"
#include "graphics.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>

using std::min;
using std::max;
using std::abs;
using std::cos;
using std::sin;
using std::round;

using namespace rgb_matrix;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

int main(int argc, char *argv[]) {
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  RGBMatrix::Options matrix_options;
  matrix_options.hardware_mapping = "adafruit-hat-pwm";
  matrix_options.cols = 64;
  matrix_options.rows = 64;
  matrix_options.chain_length = 4;
  matrix_options.parallel = 1;
  matrix_options.brightness = 100;
  matrix_options.pwm_lsb_nanoseconds = 130;
  matrix_options.show_refresh_rate = true;

  rgb_matrix::RuntimeOptions runtime_options;
  runtime_options.gpio_slowdown = 4;

  // 
  // 
  // 

  RGBMatrix *matrix = CreateMatrixFromOptions(matrix_options, runtime_options);
  if (matrix == NULL) return 1;

  FrameCanvas *offscreen_canvas = matrix->CreateFrameCanvas();

  const int width = matrix->width();
  const int height = matrix->height();

  uint32_t count = 0;

  while (!interrupt_received) {

    offscreen_canvas->Clear();

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {

        float a = sin( ( (float)y + (float)count * 0.5f) / (float)height * M_PI);
        a = abs(a);
        a *= a;

        int cx = (count + x * 2) % (3 * 255);
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

        offscreen_canvas->SetPixel(
          x, y, 
          r * a, g * a, b * a
        );

      }
    }
    
    count++;

    offscreen_canvas = matrix->SwapOnVSync(offscreen_canvas);

    usleep(1000 * 20);

  }

  // 
  // 
  // 

  matrix->Clear();
  delete matrix;

  return 0;
}