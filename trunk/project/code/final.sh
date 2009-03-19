make
make tune
# These settings are after performing analysis (we'll bump these up in the final submission):
./tune -fold 4 -onefold -examples 16000 -depth 1 -trees 400 -files /afs/ir/class/cs221/vision/data/vision_all
#./train -c trained_classifier/config_small.dat ../data_small
#./train -c trained_classifier/config.dat /afs/ir/class/cs221/vision/data/vision_all
#./test -v -c trained_classifier/config.dat -g /afs/ir/class/cs221/vision/data/easy.xml -o test.xml /afs/ir/class/cs221/vision/data/easy.avi
#./evaluate -m test.xml -g /afs/ir/class/cs221/vision/data/easy.xml
