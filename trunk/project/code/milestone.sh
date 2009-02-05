#Milestone - shortcut for running train
./train -c config /afs/ir/class/cs221/vision/data/
#Milestone - shortcut script for running test
#This is copied from pg. 7 of the handout
./test -c config -g /afs/ir/class/cs221/vision/data/easy.xml -o test.xml /afs/ir/class/cs221/vision/data/easy.avi
#Milestone - shortcut script for evaluate
#This is copied from pg. 9 of the handout
./evaluate -m test.xml -g /afs/ir/class/cs221/vision/data/easy.xml
