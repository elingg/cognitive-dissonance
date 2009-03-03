// Author: Cognitive-Dissonance team

#pragma once
#include <vector>
#include "cv.h"

using namespace std;

class EdgeDetectionFeatures {
public:
  EdgeDetectionFeatures();
  ~EdgeDetectionFeatures();

  void getFeatureValues(vector<cvHistogram>& feature_values,
                        const IplImage* img) const;

private:

};

