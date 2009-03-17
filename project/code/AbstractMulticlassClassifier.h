#pragma once
#include "Label.h"
#include <vector>
using namespace std;

class Example;
class TrainingExample;

class AbstractMulticlassClassifier {
public:
  virtual ~AbstractMulticlassClassifier() {};
  virtual bool loadState(const char* filename) = 0;
  virtual bool saveState(const char* filename) const = 0;
  
  virtual Label predict(const Example& example) const = 0;   
  virtual void   train(const vector<TrainingExample*>& examples) = 0;
};

 
