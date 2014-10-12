#!/bin/sh
mkdir -p out
gcc flash.c `pkg-config --libs --cflags libusb-1.0` -o out/flash
