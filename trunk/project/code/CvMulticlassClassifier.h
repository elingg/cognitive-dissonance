#pragma once
#include "AbstractMulticlassClassifier.h"
#include "CvBoostedClassifier.h"
#include "Label.h"

class CvMulticlassClassifier : public AbstractMulticlassClassifier {
public:
  CvMulticlassClassifier(const vector<Label>& classes, 
                         size_t numtrees, size_t depth, bool verbose); 
  ~CvMulticlassClassifier();
  bool loadState(const char* filename);
  bool saveState(const char* filename) const;
  
  Label predict(const Example& example) const;   
  void   train(const vector<TrainingExample*>& examples);
private:
  vector<Label> m_classes;
  size_t        m_numtrees;
  size_t        m_depth;
  vector<CvBoostedClassifier*> m_classifiers;
  bool          m_verbose;
};

 
