#pragma once
#include "AbstractMulticlassClassifier.h"
#include "AbstractBinaryClassifier.h"
#include "Label.h"

class MulticlassClassifier : public AbstractMulticlassClassifier {
public:
  MulticlassClassifier(const vector<Label>& classes, 
                       size_t numtrees, size_t depth, 
                       bool homegrown, bool verbose); 
  ~MulticlassClassifier();
  bool loadState(const char* filename);
  bool saveState(const char* filename) const;
  
  Label predict(const Example& example) const;   
  void  train(const vector<TrainingExample*>& examples);
private:
  vector<Label> m_classes;
  vector<AbstractBinaryClassifier*> m_classifiers;
  size_t m_numtrees;
  size_t m_depth;
  bool m_homegrown;
  bool m_verbose;
};

 
