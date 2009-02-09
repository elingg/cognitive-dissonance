#!/bin/bash

#Script for Part 2 (bagging)
DIR=/usr/class/cs221/data/digits
NUM_BAGS=100

for depth in in {3..14}
do
  echo "Running depth: $depth"
  ./digit bagging $DIR/training-1k-images.idx3 $DIR/training-1k-labels.idx1 $DIR/test-1k-images.idx3 $DIR/test-1k-labels.idx1 $depth $NUM_BAGS baggingDepth$depth.out
done
