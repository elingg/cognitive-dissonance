// Author: Cognitive-Dissonance team

#pragma once
#include "DecisionTree.h"

using namespace std;

class TestImages {
public:
  static bool test();
};

// 
// 
class ImagesExample : public TrainingExample {
public:
  ImagesExample( 
          double hfeatures[57],
           bool classlabel); 

  size_t getNumberOfFeatures() const { 
            return m_featureValues.size(); 
          }
  bool   getClassLabel() const { 
            return m_classLabel; 
          }
  double getFeatureDoubleValue(int feature_index) const { 
            return m_featureValues[feature_index]; 
          }
private:
  vector<double> m_featureValues;
  bool m_classLabel;
};


