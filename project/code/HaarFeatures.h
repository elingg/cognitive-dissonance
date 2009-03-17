// Author: Cognitive-Dissonance team

#pragma once
#include <vector>
#include "cv.h"

using namespace std;

class HaarFeatures {
public:
  HaarFeatures(bool verbose);
  ~HaarFeatures();

  void getFeatureValues(vector<double>& feature_values,
                        const IplImage* img) const;
  void getFeatureValuesBrute(vector<double>& feature_values,
                        const IplImage* img) const;

private:
  bool verbose;
};

