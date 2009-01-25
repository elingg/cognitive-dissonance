make
./rpplan -text -file world1.config -seed 100 -points 20 > output1
echo
echo Difference between our output and expected output:
echo '<Difference>' 
diff output1 sample_output/world1seed100points20.out
echo '</Difference>'
echo 
echo Output of this run:
echo '<Output>'
cat output1
echo '</Output>'
