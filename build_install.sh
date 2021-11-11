#!/bin/bash

PI_DIRECTORY="/run/media/koriwi/RPI-RP2"
pwd
cd build &&
  cmake ../ &&
  cmake --build . -j 20 &&
  stty -F /dev/ttyACM0 1200

while [ ! -d "$PI_DIRECTORY" ];
do
  sleep 0.1
done

sleep 1.5

cp freedeck-pico.uf2 $PI_DIRECTORY
