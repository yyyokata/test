#!/bin/bash
set -x
for input_format in $(seq 1 2); do
  for output_format in $(seq 1 6); do
    for pad_method in $(seq 0 1 2); do
      for input_num in 1; do
        command echo "Run cpu yuv2rgb failed"; cd -; exit -1;
      done
    done
  done
done
