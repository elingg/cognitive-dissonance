#!/bin/bash

#Script for Part 2 (bagging)
DIR=/usr/class/cs221/data/digits
NUM_BAGS=9
DEPTH=4

CMD="./digit bagging $DIR/training-1k-images.idx3 $DIR/training-1k-labels.idx1 $DIR/test-1k-images.idx3 $DIR/test-1k-labels.idx1 $DEPTH $NUM_BAGS bagging9depth4"

echo $CMD
$CMD


