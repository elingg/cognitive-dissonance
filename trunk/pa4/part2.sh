./localize -i -e gates1.ppm robot1.log bump random  > 2-bump-random.out
diff -q 2-bump-random.out sample_outputs/2-bump-random.out
# 2-bump-simple.out
./localize -i -e gates1.ppm robot1.log bump simple > 2-bump-simple.out
diff -q 2-bump-simple.out sample_outputs/2-bump-simple.out
# 2-lasers-random.out
./localize -i -e gates1.ppm robot1.log lasers random > 2-lasers-random.out
diff -q 2-lasers-random.out sample_outputs/2-lasers-random.out
# 2-lasers-simple.out
./localize -i -e gates1.ppm robot1.log lasers simple > 2-lasers-simple.out
diff -q 2-lasers-simple.out sample_outputs/2-lasers-simple.out
