#!/bin/bash
# this is just a convenience script
# no guarantee that it works!
PI_DIRECTORY="/run/media/$USER/RPI-RP2"
pwd
mkdir build
cd build &&
  cmake ../ &&
  cmake --build . -j 20