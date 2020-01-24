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

using std::min;
using std::max;

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

          int c = (count + y) % 255;
          canvas()->SetPixel(x, y, c, c, c);

          // int c = sub_blocks * (y / y_step) + x / x_step;
          // switch (count % 4) {
          //   case 0: canvas()->SetPixel(x, y, c, c, c); break;
          //   case 1: canvas()->SetPixel(x, y, c, 0, 0); break;
          //   case 2: canvas()->SetPixel(x, y, 0, c, 0); break;
          //   case 3: canvas()->SetPixel(x, y, 0, 0, c); break;
          // }

        }
      }
      count++;
      sleep(1 / 60);
    }

  }
};

int main(int argc, char *argv[]) {
  RGBMatrix::Options defaults;

  // These are the defaults when no command-line flags are given.
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

  // The ThreadedCanvasManipulator objects are filling
  // the matrix continuously.
  ThreadedCanvasManipulator *image_gen = new GrayScaleBlock(canvas);

  // Image generating demo is crated. Now start the thread.
  image_gen->Start();

  // Now, the image generation runs in the background. We can do arbitrary
  // things here in parallel. In this demo, we're essentially just
  // waiting for one of the conditions to exit.
  printf("Press <CTRL-C> to exit and reset LEDs\n");
  while (!interrupt_received) {
    sleep(1); // Time doesn't really matter. The syscall will be interrupted.
  }

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete image_gen;
  delete canvas;

  return 0;
}