#pragma once
#include "AbstractBinaryClassifier.h"

#include <vector>
#include <string>
#include "Label.h"

using namespace std;

class TrainingExample;
class DecisionTree;
class Example;

class BoostedClassifier : public AbstractBinaryClassifier {
  public: 
    BoostedClassifier(const Label& label, size_t numtrees, size_t depth); 
    ~BoostedClassifier(); 
    void train(const vector<TrainingExample*>& examples);
    double predict(const Example& example) const;
    bool loadState(const char* file);
    bool saveState(const char* file) const;
  private:
    vector<double> m_tree_alpha;
    double m_sum_alphas;
    vector<DecisionTree*> m_trees; // weak classifiers
    Label m_label;
    size_t m_num_max_trees; // B   
    size_t m_depth;
};

