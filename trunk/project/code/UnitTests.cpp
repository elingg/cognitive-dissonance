#include <iostream>
#include <sstream>
#include "assert.h"
#include "UnitTests.h"
#include "Stats.h"

int main(int argc, char** argv) {
  TestStats::test();
}

bool TestStats::test() {
 /* sample from my run in weka:
       a     b     c     d     e   f <-- classified as
  8701    70   331     0    33    0 |     a  
     0 96429     0     0     0    0 |     b
   318    14  4682     0     0    0 |     c 
     0     6     0  1232     0    0 |     d 
   843    17    61     0    52    0 |     e 
    0      0     0     0     0    0 |     f
  Rate   FP Rate   Precision   Recall  F-Measure   Class
  0.952     0.011      0.882     0.952     0.916    a
  1         0.007      0.999     1         0.999    b
  0.934     0.004      0.923     0.934     0.928    c
  0.995     0          1         0.995     0.998    d
  0.053     0          0.612     0.053     0.098    e
 */
  size_t i=0;
  size_t table[6][6];
  table[i][0] = 8701;
  table[i][1] = 70;
  table[i][2] = 331;
  table[i][3] = 0;
  table[i][4] = 33;
  table[i][5] = 0;
  
  i=1;
  table[i][0] = 0;
  table[i][1] = 96429;
  table[i][2] = 0;
  table[i][3] = 0;
  table[i][4] = 0;
  table[i][5] = 0;
  
  i=2;
  table[i][0] = 318;
  table[i][1] = 14;
  table[i][2] = 4682;
  table[i][3] = 0;
  table[i][4] = 0;
  table[i][5] = 0;
  
  i=3;
  table[i][0] = 0;
  table[i][1] = 6;
  table[i][2] = 0;
  table[i][3] = 1232;
  table[i][4] = 0;
  table[i][5] = 0;
  
  i=4;
  table[i][0] = 843;
  table[i][1] = 17;
  table[i][2] = 61;
  table[i][3] = 0;
  table[i][4] = 52;
  table[i][5] = 0;
  
  i=5;
  table[i][0] = 0;
  table[i][1] = 0;
  table[i][2] = 0;
  table[i][3] = 0;
  table[i][4] = 0;
  table[i][5] = 0;
  
  
  Stats stats;
  for(size_t i=0; i<6; i++) {
    for(size_t j=0; j<6; j++) {
      for(size_t n=0; n<table[i][j]; ++n) {
        stats.log_prediction_result(i,j);
      }
    }
  }
  stats.print_prediction_result();
  /* Rate   FP Rate   Precision   Recall  F-Measure   Class
  0.952     0.011      0.882     0.952     0.916    a
  1         0.007      0.999     1         0.999    b
  0.934     0.004      0.923     0.934     0.928    c
  0.995     0          1         0.995     0.998    d
  0.053     0          0.612     0.053     0.098    e
 */
  
  return true; // all passed
}
