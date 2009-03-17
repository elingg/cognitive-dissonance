// Author: Cognitive-Dissonance team

#pragma once
#include "Label.h"

using namespace std;

// an example is just a bunch of features... (could be a test or training
// example). generic enough to just call 'data', but might have name clashes 
// so using 'Example' for now (but applies to test data as well)...
class Example {
public:
  Example() {}
  virtual ~Example() {}

  virtual size_t getNumberOfFeatures() const = 0;

  // features could be any type, so if adding int/string features
  // provide other functions, for now just double... 
  virtual double getFeatureDoubleValue(size_t feature_index) const = 0;
};

// training example is an example for which we know the class label...
class TrainingExample : public Example {
public:
  TrainingExample() {}
  virtual ~TrainingExample() {}

  virtual Label getLabel() const = 0; 
};
