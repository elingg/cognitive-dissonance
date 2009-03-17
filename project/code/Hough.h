// Author: Cognitive-Dissonance team

#pragma once
#include <vector>
#include "cv.h"

using namespace std;

class Hough {
public:
  Hough();
  ~Hough();

  void getCircles(vector<double>& feature_values,
                        const IplImage* img) const;

  void getEdges(vector<double>& feature_values,
                        const IplImage* img) const;


private:

};

