#pragma once
#include <vector>
#include <string>
#include "Label.h"

using namespace std;

class TrainingExample;
class Example;

class AbstractBinaryClassifier {
public:
  virtual ~AbstractBinaryClassifier() {}
  virtual void train(const vector<TrainingExample*>& examples) = 0;
  virtual double predict(const Example& example) const = 0;
  virtual bool loadState(const char* file) = 0;
  virtual bool saveState(const char* file) const = 0;
};

