#include "BoostedDecisionTree.h"
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <iostream>
using namespace std;
// Creates a new BoostedDecisionTree.
BoostedDecisionTree* NewBoostedDecisionTree(DigitSet* trainingSet,
					  int maxNumClassifiers,
					  int positiveLabel,
					  int treeDepth) {
  BoostedDecisionTree* classifier = 
    (BoostedDecisionTree*) malloc(sizeof(BoostedDecisionTree));

  classifier->trainingSet = trainingSet;
  classifier->numTrainingExamples = trainingSet->numDigits;

  classifier->weakClassifiers = NewDecisionTreeSet(maxNumClassifiers);
  classifier->numClassifiers = 0;

  classifier->exampleWeights =
    (float *) malloc(trainingSet->numDigits * sizeof(float));

  for(int digitNum = 0; digitNum < trainingSet->numDigits; digitNum++) {
    classifier->exampleWeights[digitNum] = 1.0f / float(trainingSet->numDigits);
  }

  classifier->positiveLabel = positiveLabel;
  classifier->treeDepth = treeDepth;
  classifier->weakClassifiersAlpha = new vector<float>();
  return classifier;
}

// Frees the memory associated with this classifier.  
// Note: We do not free the trainingSet that is passed into 
//  NewBoostedDecisionTree
void FreeBoostedDecisionTree(BoostedDecisionTree* classifier) {
  FreeDecisionTreeSet(classifier->weakClassifiers, true);

  free(classifier->exampleWeights);

  free(classifier);
}

// Reads the classifier from a file.
BoostedDecisionTree* ReadBoostedDecisionTree(FILE* file) {
  BoostedDecisionTree* classifier = 
    (BoostedDecisionTree*) malloc(sizeof(BoostedDecisionTree));

  classifier->trainingSet = NULL;

  classifier->weakClassifiers = ReadDecisionTreeSet(file);

  fscanf(file, "%d\n", &classifier->numClassifiers);

  fscanf(file, "%d\n", &classifier->numTrainingExamples);
  classifier->exampleWeights =    (float *) malloc(classifier->trainingSet->numDigits * sizeof(float));
  for(int exampleNum = 0;
      exampleNum < classifier->numTrainingExamples;
      exampleNum++) {
    fscanf(file, "%f ", &classifier->exampleWeights[exampleNum]);
  }
  fscanf(file, "\n");

  fscanf(file, "%d\n", &classifier->positiveLabel);
  fscanf(file, "%d\n", &classifier->treeDepth);
  classifier->weakClassifiersAlpha->resize(classifier->numClassifiers,1);
  for(int ic=0; ic<classifier->numClassifiers; ic++) {
    fscanf(file, "%f\n", &((*(classifier->weakClassifiersAlpha))[ic]));
  }

  return classifier;
}

// Writes the classifier to a file.
void WriteBoostedDecisionTree(BoostedDecisionTree* classifier, FILE* file) {
  WriteDecisionTreeSet(classifier->weakClassifiers, file);

  fprintf(file, "%d\n", classifier->numClassifiers);

  fprintf(file, "%d\n", classifier->numTrainingExamples);
  for(int exampleNum = 0;
      exampleNum < classifier->numTrainingExamples;
      exampleNum++) {
    fprintf(file, "%f ", classifier->exampleWeights[exampleNum]);
  }
  fprintf(file, "\n");

  fprintf(file, "%d\n", classifier->positiveLabel);
  fprintf(file, "%d\n", classifier->treeDepth);
  for(int ic=0; ic<classifier->numClassifiers; ic++) {
    fprintf(file, "%f\n", (*(classifier->weakClassifiersAlpha))[ic]);
  }
}


// Performs one step of bagging: samples a training set to use, and
// adds a decision tree trained on that training set. All of this
// should be treating classifier->positiveLabel as the positive class.
// This allows you to easily graph the accuracy as a function of the
// number of trees.
// This function should update the example weights to represent the
// randomly sampled examples.
void AddDecisionTree(BoostedDecisionTree* classifier) {
  assert(classifier->numClassifiers < classifier->weakClassifiers->numTrees);

  //Use bagging to add a decision tree.
  // we rely on weights already set outside of this...

  //Add another bag at the end of the trees array
  classifier->weakClassifiers->trees[classifier->numClassifiers] = 
    GrowDecisionTree(classifier->trainingSet,
                     classifier->exampleWeights,
                     classifier->positiveLabel,
                     classifier->treeDepth);

  // Increment the count of classifiers
  (classifier->numClassifiers)++;
  classifier->weakClassifiersAlpha->push_back(1);
}

// Returns our confidence that the given digit is from the positiveLabel
// class.  If the classifier was created with 
//   NewBoostedDecisionTree(trainingSet, 200, 2, maxDepth)
// then it would return the confidence that the digit is a 2.
// The confidence can be measured as the number of decision trees that
// classify the digit into the positive class.
float PositiveConfidence(BoostedDecisionTree* classifier,
			 Digit* digit) {
  //Compute BoostedDecisionTree's output.
  float count = 0;
  float sumalphas=0;
  for(int i=0; i<classifier->numClassifiers; i++) {
    sumalphas += (*(classifier->weakClassifiersAlpha))[i];
  }
  for(int i = 0; i < classifier->numClassifiers; i++) {
    if(IsPositiveClass(classifier->weakClassifiers->trees[i], digit)) {
      // cerr << "Found a positive class for number " << classifier->positiveLabel << endl;
      count+=(*(classifier->weakClassifiersAlpha))[i]/sumalphas;
    }
  }
  // printf("count: %f\n", count);
  return count;
}
