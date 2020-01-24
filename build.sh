#!/bin/bash

g++ ./led-wave.cc -L../rpi-rgb-led-matrix/lib -lrgbmatrix -I../rpi-rgb-led-matrix/include -lrt -lm -lpthread -o ./led-wave