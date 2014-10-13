#!/bin/bash

for i in `seq 1 $1`;
do
    out/flash on
    sleep 0.2
    out/flash off
    sleep 0.3
done
