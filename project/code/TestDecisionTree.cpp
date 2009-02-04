#include "TestDecisionTree.h"
#include <iostream>
#include <sstream>
#include "ImagesExample.h"

int main(int argc, char** argv) {
  TestDecisionTree::test();
  TestImages::test();
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

bool TestImages::test() {
  vector<string> feature_names;

  string temp;
  for(int i=0; i<57; i++)
  {
     std::string s;
     std::stringstream out;
     out << i;
     s = out.str();
     temp = "h"+ s;
     feature_names.push_back(temp);
  }

  cerr << "Creating DecisionTree:\n";

  DecisionTree dt(feature_names);

  cerr << "Creating TrainingExamples:\n";


  // Here is a set of test cases currently not being used
  vector<TrainingExample*> egs;


   vector<double> hfeaturestest;

   for(int i=0; i<57; i++)
   {
        hfeaturestest.push_back(0);
   }

  // These tests are based on haar features...

  egs.push_back(new ImagesExample(hfeaturestest, true));

  egs.push_back(new ImagesExample(hfeaturestest, true));




  cerr << "Training tree:\n";
  dt.trainTree(egs);

  cerr << "Predicting labels:\n";
  int correct = egs.size();
  for(size_t i=0; i<egs.size(); ++i) {
    if(dt.predictClassLabel(*egs[i])!=egs[i]->getClassLabel()) {
      cerr << "No: " << i+1 << " predicted incorrectly\n";
      correct--;
    }
  }
  cerr << "Accuracy: " << (double)(correct)*100/(double)(egs.size()) << "%\n";
  for(size_t i=0; i<egs.size(); ++i) {
    delete egs[i];
  }
  return true; // all passed
}

bool TestDecisionTree::test() {
  vector<string> feature_names; 
  feature_names.push_back("gpa");
  feature_names.push_back("univ_tier");
  feature_names.push_back("published");
  feature_names.push_back("recommendation");

  cerr << "Creating DecisionTree:\n";

  DecisionTree dt(feature_names);

  cerr << "Creating TrainingExamples:\n";


  // Here is a set of test cases currently not being used
  vector<TrainingExample*> egs;

//  egs.push_back(new StudentAdmissionExample(4.0,10,0,0, true));
//  egs.push_back(new StudentAdmissionExample(4.0,10,0,0, false));
 // egs.push_back(new StudentAdmissionExample(3.7,20,0,1, false));
 // egs.push_back(new StudentAdmissionExample(3.7,30,1,1, true));



  // These tests are based on Exercise 4, Problem 5...
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

 
  cerr << "Training tree:\n";
  dt.trainTree(egs);
  
  cerr << "Predicting labels:\n";
  int correct = egs.size();
  for(size_t i=0; i<egs.size(); ++i) {
    if(dt.predictClassLabel(*egs[i])!=egs[i]->getClassLabel()) {
      cerr << "No: " << i+1 << " predicted incorrectly\n";
      correct--;
    }
  }
  cerr << "Accuracy: " << (double)(correct)*100/(double)(egs.size()) << "%\n";
  for(size_t i=0; i<egs.size(); ++i) {
    delete egs[i];
  }

  cerr << "Testing decision tree serialization..." << endl;
  dt.saveState("dttree.save");
  DecisionTree dtest;
  dtest.loadState("dttree.save");
  dtest.printTree();

  return true; // all passed
}
