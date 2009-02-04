#include "TestImages.h"
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {
  TestImages::test();
}

ImagesExample::ImagesExample(double hfeatures[], bool class_label)
   {
    for(int i=0; i<57; i++)
    {
	m_featureValues.push_back(hfeatures[i]);
    }
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
     cout<<temp<<endl;
     feature_names.push_back(temp);
  }

  cerr << "Creating DecisionTree:\n";

  DecisionTree dt(feature_names);

  cerr << "Creating TrainingExamples:\n";


  // Here is a set of test cases currently not being used
  vector<TrainingExample*> egs;


   double hfeaturestest[57]; 

   for(int i=0; i<57; i++)
   {
	hfeaturestest[i]=0;
   }

  // These tests are based on haar features...

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
