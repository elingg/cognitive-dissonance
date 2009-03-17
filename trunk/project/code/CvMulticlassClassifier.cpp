#include "CvMulticlassClassifier.h"
#include <iostream>
#include <fstream>
#include "Timer.h"

CvMulticlassClassifier::CvMulticlassClassifier(const vector<Label>& classes, 
                                               size_t numtrees, size_t depth, 
                                               bool verbose)
:m_classes(classes), m_numtrees(numtrees), m_depth(depth), m_verbose(verbose) {
  if(m_verbose) cerr << "Creating CvMulticlassClassifier with " << classes.size() << " classes\n";
  for(size_t ic=0; ic<m_classes.size(); ic++) {
    m_classifiers.push_back(new CvBoostedClassifier(m_classes[ic], m_numtrees, m_depth));
  }
}
 
CvMulticlassClassifier::~CvMulticlassClassifier() {
  for(size_t ic=0; ic<m_classifiers.size(); ic++) {
    delete m_classifiers[ic];
  }
}

Label CvMulticlassClassifier::predict(const Example& example) const {
  double max_class_prob = 0;
  Label predicted_class=getLabel("other");
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

void CvMulticlassClassifier::train(const vector<TrainingExample*>& examples) {
  Timer t ("multiclass classifier",m_verbose);
  if(m_verbose) cerr << "Training multiclass classifier..."<< endl;
  for(size_t ic=0; ic<m_classes.size(); ic++) {
    if(m_verbose) cerr << "Training boosted classifier for " << getLabelString(m_classes[ic]) << endl;
    m_classifiers[ic]->train(examples);
  } 
}

bool CvMulticlassClassifier::loadState(const char* filename) {
  if(m_verbose) cerr << "CvMulticlassClassifier loadState\n";
  ifstream ifs(filename);
  if(ifs.fail()) {
    cerr << "Failed loading file : " << filename << endl;
    return false;
  }
  size_t num_classes;
  ifs >> num_classes;
  ifs >> m_numtrees; 
  ifs >> m_depth; 
  bool ret_val = true;
  for(size_t ic=0; ic<num_classes; ic++) {
    string class_name; 
    ifs >> class_name;
    Label label = getLabel(class_name);
    CvBoostedClassifier* bc = new CvBoostedClassifier(class_name, 
                                      m_numtrees, m_depth);
    string cfname = getClassifierFileName(label, "");
    ret_val&=bc->loadState(cfname.c_str());
    m_classifiers.push_back(bc);
  }
  ifs.close();
  return ret_val;
}

bool CvMulticlassClassifier::saveState(const char* filename) const {
  if(m_verbose) cerr << "CvMulticlassClassifier saveState\n";
  ofstream ofs(filename);
  if(ofs.fail()) {
    cerr << "Failed loading file : " << filename << endl;
    return false;
  }
  ofs << m_classes.size() << " ";
  bool ret_val = true;
  ofs << " " << m_numtrees; 
  ofs << " " << m_depth; 
  for(size_t ic=0; ic<m_classes.size(); ic++) {
    ofs << m_classes[ic] << " ";
    ret_val&=m_classifiers[ic]->saveState(ofs);
  }
  ofs.close();
  return ret_val;
} 
