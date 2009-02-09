#!/bin/bash

#Script for Part 1 
DIR=/afs/ir/class/cs221/data/digits

datasize="1k"
startdepth=4
stopdepth=14
stepdepth=2

for ((depth=$startdepth; depth<=$stopdepth; depth=depth+$stepdepth))
do
  echo "Depth: $depth, Trained on $datasize"
  cmd="./digit singletree $DIR/training-$datasize-images.idx3 $DIR/training-$datasize-labels.idx1 $DIR/test-1k-images.idx3 $DIR/test-1k-labels.idx1 $depth singleTree$datasize$depth.out"
  echo $cmd
  $cmd
done

LOGFILE=tmp.log
rm -rf $LOGFILE
echo -e "\nAccuracies for 1k:" >> $LOGFILE
echo "On train data:" >>  $LOGFILE
for ((depth=$startdepth; depth<=$stopdepth; depth=depth+$stepdepth))
do
  echo "Depth: $depth" >> $LOGFILE
  cat results.singletree.training.d$depth.txt >> $LOGFILE
done
echo "On test data:" >> $LOGFILE
for ((depth=$startdepth; depth<=$stopdepth; depth=depth+$stepdepth))
do
  echo "Depth: $depth" >> $LOGFILE
  cat results.singletree.test.d$depth.txt >> $LOGFILE
done

datasize="10k"
for ((depth=$startdepth; depth<=$stopdepth; depth=depth+$stepdepth))
do
  echo "Depth: $depth, Trained on $datasize"
  cmd="./digit singletree $DIR/training-$datasize-images.idx3 $DIR/training-$datasize-labels.idx1 $DIR/test-1k-images.idx3 $DIR/test-1k-labels.idx1 $depth singleTree$datasize$depth.out"
  echo $cmd
  $cmd
done
echo -e "\nAccuracies for 10k:" >> $LOGFILE
echo "On train data:" >> $LOGFILE
for ((depth=$startdepth; depth<=$stopdepth; depth=depth+$stepdepth))
do
  echo "Depth: $depth" >> $LOGFILE
  cat results.singletree.training.d$depth.txt >> $LOGFILE
done
echo "On test data:" >> $LOGFILE
for ((depth=$startdepth; depth<=$stopdepth; depth=depth+$stepdepth))
do
  echo "Depth: $depth" >> $LOGFILE
  cat results.singletree.test.d$depth.txt >> $LOGFILE
done
cat $LOGFILE
echo "Accuracies also logged to $LOGFILE"
