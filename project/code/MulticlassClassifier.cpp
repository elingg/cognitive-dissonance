#include "MulticlassClassifier.h"
#include "BoostedClassifier.h"
#include "CvBoostedClassifier.h"
#include <iostream>
#include <fstream>
#include "Timer.h"

MulticlassClassifier::MulticlassClassifier(const vector<Label>& classes, 
                                           size_t numtrees, size_t depth, 
                                           bool homegrown, bool verbose)
:m_classes(classes), m_numtrees(numtrees), m_depth(depth), 
m_homegrown(homegrown), m_verbose(verbose) {
  for(size_t ic=0; ic<m_classes.size(); ic++) {
    if(homegrown) {
      m_classifiers.push_back(new BoostedClassifier(m_classes[ic], m_numtrees, m_depth));
    } else {
      m_classifiers.push_back(new CvBoostedClassifier(m_classes[ic], m_numtrees, m_depth));
    }
  }
}
 
MulticlassClassifier::~MulticlassClassifier() {
  for(size_t ic=0; ic<m_classifiers.size(); ic++) {
    delete m_classifiers[ic];
  }
}

Label MulticlassClassifier::predict(const Example& example) const {
  double max_class_prob = 0;
  Label predicted_class = getLabel("other");
  for(size_t ic=0; ic< m_classifiers.size(); ic++) {
    double prediction_for_class = m_classifiers[ic]->predict(example);
    // cerr << "Prediction for class " << m_classes[ic] << " is " << prediction_for_class << endl;
    if(prediction_for_class>max_class_prob) {
      predicted_class = m_classes[ic];
      max_class_prob = prediction_for_class;
    }
  }
  // cerr << "Predicted class: " << getLabelString(predicted_class) << endl;
  return predicted_class;
}

void MulticlassClassifier::train(const vector<TrainingExample*>& examples) {
  if(m_verbose) cerr << "Training multiclass classifier..."<< endl;
  for(size_t ic=0; ic<m_classes.size(); ic++) {
    Timer t(string("training boosted classifier for ")+getLabelString(m_classes[ic]), m_verbose);
    m_classifiers[ic]->train(examples);
  } 
}

bool MulticlassClassifier::loadState(const char* filename) {
  for(size_t ic=0;ic<m_classifiers.size(); ic++) {
    delete m_classifiers[ic];
  } 
  m_classifiers.clear();
  if(m_verbose) cerr << "MulticlassClassifier loadState\n";
  ifstream ifs(filename);
  if(ifs.fail()) {
    cerr << "Failed loading file : " << filename << endl;
    return false;
  }
  ifs >> m_homegrown;
  ifs >> m_numtrees;
  ifs >> m_depth;
  size_t num_classes;
  ifs >> num_classes;
  bool ret_val = true;
  for(size_t ic=0; ic<num_classes; ic++) {
    string class_name; 
    ifs >> class_name;
    Label label = getLabel(class_name);
    string cfname = getClassifierFileName(label,"");
    AbstractBinaryClassifier* abc = 0;
    if(m_homegrown) {
      // cerr << "Creating home grown classifier\n";
      abc = new BoostedClassifier(label, m_numtrees, m_depth);
    } else {
      // cerr << "Creating CvBoost classifier\n";
      abc = new CvBoostedClassifier(label, m_numtrees, m_depth);
    }
    ret_val&=abc->loadState(cfname.c_str());
    m_classifiers.push_back(abc);
  }
  ifs.close();
  return ret_val;
}

bool MulticlassClassifier::saveState(const char* filename) const {
  if(m_verbose) cerr << "MulticlassClassifier saveState\n";
  ofstream ofs(filename);
  if(ofs.fail()) {
    cerr << "Failed loading file : " << filename << endl;
    return false;
  }
  ofs << m_homegrown << " ";
  ofs << m_numtrees << " ";
  ofs << m_depth << " ";
  ofs << m_classes.size() << " ";
  bool ret_val = true;
  for(size_t ic=0; ic<m_classes.size(); ic++) {
    ofs << getLabelString(m_classes[ic]) << " ";
    string cfname = getClassifierFileName(m_classes[ic],"");
    ret_val&=m_classifiers[ic]->saveState(cfname.c_str());
  }
  ofs.close();
  return ret_val;
} 
