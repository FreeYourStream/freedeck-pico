#!/bin/bash
PI_DIRECTORY="/run/media/$USER/RPI-RP2"
./build.sh
stty -F /dev/ttyACM0 1200
while [ ! -d "$PI_DIRECTORY" ];
do
  sleep 0.1
done

sleep 1.5

cp build/freedeck-pico.uf2 $PI_DIRECTORY
