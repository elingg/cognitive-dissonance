./localize -i -e gates1.ppm robot1.log lasers simple nohistory > 4-lasers-simple-nohistory.out
diff -q 4-lasers-simple-nohistory.out sample_outputs/4-lasers-simple-nohistory.out
