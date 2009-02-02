#include "TestDecisionTree.h"
#include <iostream>

int main(int argc, char** argv) {
  TestDecisionTree::test();
}

StudentAdmissionExample::StudentAdmissionExample(double gpa,
  double univ_tier, double published, double recommendation, 
  bool class_label) {
  m_featureValues.push_back(gpa);
  m_featureValues.push_back(univ_tier);
  m_featureValues.push_back(published);
  m_featureValues.push_back(recommendation);
  m_classLabel = class_label;
}

bool TestDecisionTree::test() {
  DecisionTree dt;
  vector<TrainingExample*> egs;
  // 1, 2, 3...
  egs.push_back(new StudentAdmissionExample(4.0,10,1,1,true));
  egs.push_back(new StudentAdmissionExample(4.0,10,0,1,true));
  egs.push_back(new StudentAdmissionExample(4.0,20,0,0,true));
 
  // 4, 5, 6... 
  egs.push_back(new StudentAdmissionExample(3.7,10,1,1,true));
  egs.push_back(new StudentAdmissionExample(3.7,20,0,1,true));
  egs.push_back(new StudentAdmissionExample(3.7,30,1,1,true));
 
  // 7, 8, 9... 
  egs.push_back(new StudentAdmissionExample(3.7,30,0,1,false));
  egs.push_back(new StudentAdmissionExample(3.7,10,0,1,false));
  egs.push_back(new StudentAdmissionExample(3.5,20,1,0,false));
 
  // 10, 11, 12... 
  egs.push_back(new StudentAdmissionExample(3.5,10,0,0,false));
  egs.push_back(new StudentAdmissionExample(3.5,30,1,0,false));
  egs.push_back(new StudentAdmissionExample(3.5,30,0,1,false));
  
  dt.trainTree(egs);
  
  int wrong = 0;
  for(size_t i=0; i<egs.size(); ++i) {
    if(dt.predictClassLabel(*egs[i])!=egs[i]->getClassLabel()) {
      cerr << "No: " << i+1 << " predicted incorrectly\n";
      wrong++;
    }
  }
  cerr << "Accuracy: " << (double)(wrong)*100/(double)(egs.size()) << "%\n";
  for(size_t i=0; i<egs.size(); ++i) {
    delete egs[i];
  }
  return true; // all passed
}
