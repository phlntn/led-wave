// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Small example how to use the library.
// For more examples, look at demo-main.cc
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"
#include "threaded-canvas-manipulator.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>

using namespace rgb_matrix;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

class GrayScaleBlock : public ThreadedCanvasManipulator {
public:
  GrayScaleBlock(Canvas *m) : ThreadedCanvasManipulator(m) {}
  void Run() {
    const int sub_blocks = 16;
    const int width = canvas()->width();
    const int height = canvas()->height();
    const int x_step = max(1, width / sub_blocks);
    const int y_step = max(1, height / sub_blocks);
    uint8_t count = 0;
    while (running() && !interrupt_received) {
      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
          int c = sub_blocks * (y / y_step) + x / x_step;
          switch (count % 4) {
          case 0: canvas()->SetPixel(x, y, c, c, c); break;
          case 1: canvas()->SetPixel(x, y, c, 0, 0); break;
          case 2: canvas()->SetPixel(x, y, 0, c, 0); break;
          case 3: canvas()->SetPixel(x, y, 0, 0, c); break;
          }
        }
      }
      count++;
      sleep(2);
    }
  }
};

int main(int argc, char *argv[]) {
  RGBMatrix::Options defaults;
  defaults.hardware_mapping = "adafruit-hat-pwm";  // or e.g. "adafruit-hat"
  defaults.cols = 64;
  defaults.rows = 64;
  defaults.chain_length = 3;
  defaults.parallel = 1;
  defaults.show_refresh_rate = true;

  Canvas *canvas = CreateMatrixFromFlags(&argc, &argv, &defaults);
  if (canvas == NULL)
    return 1;

  // It is always good to set up a signal handler to cleanly exit when we
  // receive a CTRL-C for instance. The DrawOnCanvas() routine is looking
  // for that.
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  image_gen = new GrayScaleBlock(canvas);
  image_gen->Start();

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}