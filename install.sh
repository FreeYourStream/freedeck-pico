#!/bin/bash
./build.sh
stty -F /dev/ttyACM0 1200
while [ ! -d "$PI_DIRECTORY" ];
do
  sleep 0.1
done

sleep 1.5

cp freedeck-pico.uf2 $PI_DIRECTORY
