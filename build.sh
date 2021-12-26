#!/bin/bash
# this is just a convenience script
# no guarantee that it works!
pwd
mkdir build
cd build &&
  cmake ../ &&
  cmake --build . -j 20