#pragma once
#include "DecisionTree.h"
#include <string>
#include "Label.h"
using namespace std;


class ImagesExample : public TrainingExample {
public:
  ImagesExample(
           const vector<double>& hfeatures);

  ImagesExample(
           const vector<double>& hfeatures,
           const Label& classlabel);

  size_t getNumberOfFeatures() const {
            return m_featureValues.size();
          }
  Label getLabel() const {
            return m_classLabel;
          }
  double getFeatureDoubleValue(size_t feature_index) const {
            return m_featureValues[feature_index];
          }
private:
  vector<double> m_featureValues;
  Label m_classLabel;
};

