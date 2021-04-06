#!/bin/bash

PI_DIRECTORY="/run/media/koriwi/RPI-RP2"

cd build &&
  make &&

while [ ! -d "$PI_DIRECTORY" ];
do
  sleep 0.1
done

cp i2c_7seg.uf2 $PI_DIRECTORY
