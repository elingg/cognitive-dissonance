#pragma once
#include "AbstractBinaryClassifier.h"
#include "ml.h"

using namespace std;

class TrainingExample;
class DecisionTree;
class Example;

class CvBoostedClassifier: public AbstractBinaryClassifier {
  public: 
    CvBoostedClassifier(const Label& label, size_t nummaxtrees, size_t depth);
    ~CvBoostedClassifier(); 
    void train(const vector<TrainingExample*>& examples);
    double predict(const Example& example) const;
    bool loadState(const char* file);
    bool saveState(const char* file) const;
  private:
    Label m_label;
    size_t m_num_max_trees;
    size_t m_depth;
    CvBoost m_cvimpl;
};

