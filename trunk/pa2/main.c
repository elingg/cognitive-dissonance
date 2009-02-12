#include "Digit.h"
#include "DecisionTree.h"
#include "BaggedDecisionTree.h"
#include "BoostedDecisionTree.h"
#include "OneAgainstMany.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define SOLUTION 1

void ErrorIntro(int argc, char** argv) {
  printf("Usage: %s <singletree|bagging>\n", argv[0]);
  printf("          <training-images> <training-labels> <test-images> <test-labels>\n");
}

bool EndsWith(char* str, char* ending) {
  int strLength = strlen(str);
  int endingLength = strlen(ending);

  if(endingLength > strLength) {
    return false;
  }

  str += strLength - endingLength;

  return strcmp(str, ending) == 0;
}

void CheckClassifierOut(char* name) {
  if(EndsWith(name, ".c") || EndsWith(name, ".h")) {
    printf("Error: %s looks like a source file name.\n", name);
    exit(1);
  }
}


void RunSingleTree(int argc, char** argv) {
  if(argc < 8) {
    ErrorIntro(argc, argv);
    printf("          <tree-depth> <classifier-out>\n");
    exit(1);
  }

  DigitSet* trainingSet = LoadDigits(argv[2], argv[3]);
  DigitSet* testSet = LoadDigits(argv[4], argv[5]);
	   
  int depth = atoi(argv[6]);

  char* classifierOutFileName = argv[7];
  CheckClassifierOut(classifierOutFileName);

  DecisionTreeSet* classifiers = NewDecisionTreeSet(10);

  // Grow trees, output accuracy
  stringstream trainingOutStream;
  trainingOutStream << "results.singletree.training.d" << depth
		    << ".txt";
  string trainingOutFileName = trainingOutStream.str();
  
  stringstream testOutStream;
  testOutStream << "results.singletree.test.d" << depth
		<< ".txt";
  string testOutFileName = testOutStream.str();
  
  ofstream trainingOut(trainingOutFileName.c_str());
  ofstream testOut(testOutFileName.c_str());
  
  float* exampleWeights = new float[trainingSet->numDigits];
  for(int digitNum = 0; digitNum < trainingSet->numDigits; digitNum++) {
    exampleWeights[digitNum] = 1;
  }
  
  // DONETODO: Grow the decision trees for each digit as positive label.
  //       classifiers->trees should be initialized after this.
  for(int digitLabel=0; digitLabel<10; ++digitLabel) {
    DecisionTree* grownTree = GrowDecisionTree(trainingSet, exampleWeights,
                                             digitLabel, depth); 
    classifiers->trees[digitLabel] = grownTree;  
  }
  
  trainingOut << 0 << " "
	      << DecisionTreeAccuracy(classifiers, trainingSet) << endl;
  testOut << 0 << " "
	  << DecisionTreeAccuracy(classifiers, testSet) << endl;
  
  trainingOut.flush();
  testOut.flush();

  FILE* file;
  file = fopen(classifierOutFileName, "w");
  WriteDecisionTreeSet(classifiers, file);

  FreeDecisionTreeSet(classifiers, true);
}

// Computes log_2(x).
static float LogBaseTwo(float x) {
  return log(x) / log(2.0);
}

void RunBoostedDecisionTree(int argc, char** argv) {
  if(argc < 9) {
    ErrorIntro(argc, argv);
    printf("          <tree-depth> <max-ensemble-size> <classifier-out>\n");
    exit(1);
  }
  
  DigitSet* trainingSet = LoadDigits(argv[2], argv[3]);
  DigitSet* testSet = LoadDigits(argv[4], argv[5]);
	   
  int depth = atoi(argv[6]);

  int maxEnsembleSize = atoi(argv[7]);
  
  char* classifierOutFileName = argv[8];
  CheckClassifierOut(classifierOutFileName);
  
  BoostedDecisionTree** classifiers = 
    (BoostedDecisionTree**) malloc(10 * sizeof(BoostedDecisionTree*));;
  for(int positiveLabel = 0; positiveLabel < 10; positiveLabel++) {
    classifiers[positiveLabel] = 
      NewBoostedDecisionTree(trainingSet, maxEnsembleSize, positiveLabel, depth);
  }
  // Tell each classifier to learn, track performance.
  stringstream trainingOutStream;
  trainingOutStream << "results.boostedt.training.d" << depth 
		    << ".txt";
  string trainingOutFileName = trainingOutStream.str();
  
  stringstream testOutStream;
  testOutStream << "results.boostedt.test.d" << depth 
		<< ".txt";
  string testOutFileName = testOutStream.str();
  
  ofstream trainingOut(trainingOutFileName.c_str());
  ofstream testOut(testOutFileName.c_str());
  
  // Iterate over number of bags, or however much gives 100% accuracy
  bool peaked = false;
  for(int numClassifiers = 0; (!peaked) && (numClassifiers < maxEnsembleSize); numClassifiers++
      ) {
    
    //cout << "Number of bags: " << numClassifiers << endl;
    // Use bagging to add a decision tree to each of classifiers[0..9], which 
    // corresponds to classifiers treating that digit as the positive class
    // this method creates and adds a DecisionTree to this, using the...
    // exampleWeights associated with the BaggedDecisionTree passed in.
    // so update each bagged tree passed in with the new weights...
    for(int i = 0; i < 10; i++) {
      AddDecisionTree(classifiers[i]);
    }
    vector<vector<float> > newWeights(10); 
    vector<float> newAlphas(10); 
    for(int i = 0; i < 10; i++) {
      float epsilonb=0;
      for(int iex = 0; iex < trainingSet->numDigits; iex++) {
        if(GetBoostedTreePrediction(classifiers,trainingSet->digits[iex])!=trainingSet->digits[iex]->label) {
          epsilonb+=classifiers[i]->exampleWeights[iex];
        } 
      }
      if(epsilonb<1e-14) {
        peaked = true;
      }
      newWeights[i].resize(trainingSet->numDigits,0);
      float alphab = 0;
      if(epsilonb>0 && epsilonb<1.0) { 
        alphab = 0.5*LogBaseTwo((1-epsilonb)/(epsilonb));
      }
      //if(i==0) cerr << "epsilon: " << epsilonb << ", " << alphab << endl;
      float weightsum = 0;
      for(int iex = 0; iex < trainingSet->numDigits; iex++) {
        if(GetBoostedTreePrediction(classifiers,trainingSet->digits[iex])!=trainingSet->digits[iex]->label) { 
          newWeights[i][iex] = classifiers[i]->exampleWeights[iex]
                                                 *exp(alphab);
        } else {
          newWeights[i][iex] = classifiers[i]->exampleWeights[iex]
                                                 *exp(-alphab);
        }
        weightsum+=newWeights[i][iex];
      }
      if(weightsum<1e-14) {
        peaked = true;
        continue;
      }
      // printf("Weight sum: %f\n",weightsum);
      for(int iex = 0; iex < trainingSet->numDigits; iex++) {
        newWeights[i][iex]/=weightsum;
      }
      newAlphas[i] = alphab;
    }
    if(!peaked) {
      for(int i=0; i<10; i++) {
        //if(numClassifiers==14 && i==0) { cerr << "New Weights: " <<endl; }
        for(int iex=0; iex<trainingSet->numDigits; iex++) {
          classifiers[i]->exampleWeights[iex] = newWeights[i][iex];
          //if(numClassifiers==14 && i==0) { cerr << newWeights[i][iex] << " "; }
        }
        //if(numClassifiers==14 && i==0) { cerr << "New Alphas: " <<endl; }
        for(int ic=0; ic< classifiers[i]->numClassifiers; ++ic) {
          (*(classifiers[i]->weakClassifiersAlpha))[ic] = newAlphas[i];
          //if(numClassifiers==14 && i==0) { cerr << newAlphas[i] << " "; }
        }
      }
    }
    if(peaked) {
      float trainAccuracy = BoostedDecisionTreeAccuracy(classifiers, trainingSet);
      float testAccuracy = BoostedDecisionTreeAccuracy(classifiers, testSet);

      trainingOut << numClassifiers << " " 
		   << trainAccuracy << endl;
      testOut << numClassifiers << " "
	      << testAccuracy << endl;
    
      cout << "Number of bags: " << numClassifiers+1 << endl;
      cerr << "Training Accuracy: " << trainAccuracy << endl;
      cerr << "Testing Accuracy: " << testAccuracy << endl;
      cerr << "----" << endl;
      trainingOut.flush();
      testOut.flush();
    }
    
  }

  if(maxEnsembleSize < 10) {
    FILE* file = fopen(classifierOutFileName, "w");
    for(int classifierNum = 0; classifierNum < 10; classifierNum++) {
      WriteBoostedDecisionTree(classifiers[classifierNum], file); 
    }
    fclose(file);
  }

  free(classifiers);
}


void RunBaggedDecisionTree(int argc, char** argv) {
  if(argc < 9) {
    ErrorIntro(argc, argv);
    printf("          <tree-depth> <max-ensemble-size> <classifier-out>\n");
    exit(1);
  }
  
  DigitSet* trainingSet = LoadDigits(argv[2], argv[3]);
  DigitSet* testSet = LoadDigits(argv[4], argv[5]);
	   
  int depth = atoi(argv[6]);

  int maxEnsembleSize = atoi(argv[7]);
  
  char* classifierOutFileName = argv[8];
  CheckClassifierOut(classifierOutFileName);
  
  BaggedDecisionTree** classifiers = 
    (BaggedDecisionTree**) malloc(10 * sizeof(BaggedDecisionTree*));;
  for(int positiveLabel = 0; positiveLabel < 10; positiveLabel++) {
    classifiers[positiveLabel] = 
      NewBaggedDecisionTree(trainingSet, maxEnsembleSize, positiveLabel, depth);
  }
  
  // Tell each classifier to learn, track performance.
  stringstream trainingOutStream;
  trainingOutStream << "results.bagdt.training.d" << depth 
		    << ".txt";
  string trainingOutFileName = trainingOutStream.str();
  
  stringstream testOutStream;
  testOutStream << "results.bagdt.test.d" << depth 
		<< ".txt";
  string testOutFileName = testOutStream.str();
  
  ofstream trainingOut(trainingOutFileName.c_str());
  ofstream testOut(testOutFileName.c_str());

  // Iterate over number of bags
  for(int numClassifiers = 0; numClassifiers < maxEnsembleSize; numClassifiers++
      ) {
    cout << "Number of bags: " << numClassifiers << endl;
    
    // Use bagging to add a decision tree to each of classifiers[0..9], which 
    // corresponds to classifiers treating that digit as the positive class
    for(int i = 0; i < 10; i++) {
      AddDecisionTree(classifiers[i]);
    }

    float trainAccuracy = BaggedDecisionTreeAccuracy(classifiers, trainingSet);
    float testAccuracy = BaggedDecisionTreeAccuracy(classifiers, testSet);

    trainingOut << numClassifiers << " " 
		<< trainAccuracy << endl;
    testOut << numClassifiers << " "
	    << testAccuracy << endl;
    
    cerr << "Training Accuracy: " << trainAccuracy << endl;
    cerr << "Testing Accuracy: " << testAccuracy << endl;
    cerr << "----" << endl;
    trainingOut.flush();
    testOut.flush();
    
  }
  

  if(maxEnsembleSize < 10) {
    FILE* file = fopen(classifierOutFileName, "w");
    for(int classifierNum = 0; classifierNum < 10; classifierNum++) {
      WriteBaggedDecisionTree(classifiers[classifierNum], file); 
    }
    fclose(file);
  }

  free(classifiers);
}




int main(int argc, char** argv) {
  if(argc < 6) {
    ErrorIntro(argc, argv);
    printf("          ...\n");
    exit(1);
  }

  char* opType = argv[1];
  
  if(strcmp(opType, "singletree") == 0) {
    RunSingleTree(argc, argv);
  }
  else if(strcmp(opType, "bagging") == 0) {
    RunBaggedDecisionTree(argc, argv);
  }
  else if(strcmp(opType, "boosting") == 0) {
    RunBoostedDecisionTree(argc, argv);
  }
  else {
    ErrorIntro(argc, argv);
    printf("          ...\n");
    exit(1);
  }
}

