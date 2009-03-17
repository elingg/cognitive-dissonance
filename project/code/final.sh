make
make tune
./tune -fold 3 -depth 2 -trees 1000 /afs/ir/class/cs221/vision/data/vision_all
#./train -c trained_classifier/config_small.dat ../data_small
#./train -c trained_classifier/config.dat /afs/ir/class/cs221/vision/data/vision_all
#./test -v -c trained_classifier/config.dat -g /afs/ir/class/cs221/vision/data/easy.xml -o test.xml /afs/ir/class/cs221/vision/data/easy.avi
#./evaluate -m test.xml -g /afs/ir/class/cs221/vision/data/easy.xml
