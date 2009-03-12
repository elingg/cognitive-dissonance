#for ((i=5; i<6; i++));
#do
#  for((j=1; j<9; j++));
#  do
#    ./localize -i -e gates1.ppm robot1.log custom simple -sensormodelweight 0.9$i -unexpectedmodelweight 0.0001 -failuremodelweight 0.00001 > 5-custom-simple-reference_error.out
#    head -4 5-custom-simple-reference_error.out 
#    tail -2 5-custom-simple-reference_error.out 
#  done
#done

echo
./localize -i -e gates1.ppm robot1.log custom simple -sensormodelweight 0.968 -unexpectedmodelweight 0.001 -failuremodelweight 0.001 > 5-custom-simple-reference_error.out
head -4 5-custom-simple-reference_error.out 
tail -2 5-custom-simple-reference_error.out 
#echo
#./localize -i -e gates1.ppm robot1.log custom simple -sensormodelweight 0.968 -unexpectedmodelweight 0.001 -failuremodelweight 0.001 > 5-custom-simple-reference_error.out
#head -4 5-custom-simple-reference_error.out 
#tail -2 5-custom-simple-reference_error.out 
echo 
echo Compare to reference error:
tail -2 sample_outputs/5-custom-simple-reference_error.out
