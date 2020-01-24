#include "led-matrix.h"

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
  defaults.show_refresh_rate = true;

  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  // 
  // 
  // 

  Canvas *canvas = CreateMatrixFromFlags(&argc, &argv, &defaults);
  if (canvas == NULL) return 1;

  const int width = canvas()->width();
  const int height = canvas()->height();
  uint8_t count = 0;

  while (!interrupt_received) {

    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {

        int c = (count + y) % 255;
        if (c > 128) c = 255 - c;

        canvas()->SetPixel(x, y, c, c, c);

      }
    }
    
    count++;
    sleep(1 / 30);

  }

  // 
  // 
  // 

  canvas->Clear();
  delete image_gen;
  delete canvas;

  return 0;
}