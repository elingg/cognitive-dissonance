#!/bin/bash
#Script that nicely formats the training and testing accuracy.
#Run this after task2.sh
echo "Training Accuracy from Depth 3 to 14"
echo "BagsIndex Accuracy"
for depth in {3..14}
do
  logfile="results.bagdt.training.d$depth.txt"
  tail -n 1 $logfile
done
echo
echo "Testing Accuracy from Depth 3 to 14"
echo "BagsIndex Accuracy"
for depth in {3..14}
do
  logfile="results.bagdt.test.d$depth.txt"
  tail -n 1 $logfile
done
