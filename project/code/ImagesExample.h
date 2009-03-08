#pragma once
#include "DecisionTree.h"

using namespace std;


class ImagesExample : public TrainingExample {
public:
  ImagesExample(
           const vector<double>& hfeatures);
  ImagesExample(
           const vector<double>& hfeatures,
           const string& classlabel);

  size_t getNumberOfFeatures() const {
            return m_featureValues.size();
          }
  string getLabel() const {
            return m_classLabel;
          }
  bool   getClassLabel() const {
            if(m_classLabel.compare("mug")==0)  {
              return true;
            }
            return false;
          }
  double getFeatureDoubleValue(int feature_index) const {
            return m_featureValues[feature_index];
          }
private:
  vector<double> m_featureValues;
  string m_classLabel;
};

