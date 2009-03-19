make
make tune
# ./tune -v -fold 4 -onefold -examples 16000 -classifier ours -depth 1 -trees 400 -files /afs/ir/class/cs221/vision/data/vision_all
./tune -v -fold 4 -onefold -examples 16000 -classifier cv -depth 1 -trees 400 -files /afs/ir/class/cs221/vision/data/vision_all
