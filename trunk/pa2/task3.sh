#!/bin/bash

#Script for Part 2 (bagging)
DIR=/afs/ir/class/cs221/data/digits
NUM_BAGS=100

for depth in {3..14}
#for depth in {6..6}
do
  echo "Running depth: $depth"
  ./digit boosting $DIR/training-1k-images.idx3 $DIR/training-1k-labels.idx1 $DIR/test-1k-images.idx3 $DIR/test-1k-labels.idx1 $depth $NUM_BAGS bagD$depth.out
done
