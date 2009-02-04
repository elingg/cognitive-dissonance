// Author: Cognitive-Dissonance team

#pragma once
#include "DecisionTree.h"

using namespace std;

class TestDecisionTree {
public:
  static bool test();
};

class TestImages {
public:
  static bool test();
};

class ImagesExample : public TrainingExample {
public:
  ImagesExample(
          vector<double> hfeatures,
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


// Corresponds to Problem 5 of Exercise Set 4. Using continuous
// variables here rather than discrete ones for testing our decision tree...
class StudentAdmissionExample : public TrainingExample {
public:
  StudentAdmissionExample(double gpa, // 4.0, 3.7, 3.5
           double univ_tier, // 10, 20, 30
           double published, // 0, 1
           double recommendation, // normal (0), good (1)
           bool classlabel); // N(negative)(0), P(positive)(1)

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


