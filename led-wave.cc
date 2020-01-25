#include "led-matrix.h"
#include "graphics.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>

using std::min;
using std::max;
using std::abs;
using std::sin;
using std::cos;
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

  uint32_t frame = 0;

  while (!interrupt_received) {
    offscreen_canvas->Clear();

    for (int y = 0; y < height; ++y) {
      float yProg = (float)y / (float)height;

      for (int x = 0; x < width; ++x) {
        float xProg = (float)x / (float)width;

        // Alpha

        float alpha = sin(
          yProg * M_PI // Vertical gradient
          + (float)frame * 0.03f // Constant vertical scrolling
          + xProg * 30.0f // Wave
        );
        alpha = abs(alpha * alpha);

        // Color

        int cx = (int)(xProg * 256) % (3 * 255);
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

        // Output

        offscreen_canvas->SetPixel(
          x, y, 
          r * alpha, g * alpha, b * alpha
        );

      }
    }
    
    frame++;
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