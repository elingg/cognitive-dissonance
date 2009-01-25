make
./rpplan -text -file world2.config -seed 100 -points 40 > output2
echo
echo Difference between our output and expected output:
echo '<Difference>' 
diff output2 sample_output/world2seed100points40.out
echo '</Difference>'
echo 
echo Output of this run:
echo '<Output>'
cat output2
echo '</Output>'
