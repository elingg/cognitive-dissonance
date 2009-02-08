#include "BaggedDecisionTree.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

// Creates a new BaggedDecisionTree.
BaggedDecisionTree* NewBaggedDecisionTree(DigitSet* trainingSet,
					  int maxNumClassifiers,
					  int positiveLabel,
					  int treeDepth) {
  BaggedDecisionTree* classifier = 
    (BaggedDecisionTree*) malloc(sizeof(BaggedDecisionTree));

  classifier->trainingSet = trainingSet;
  classifier->numTrainingExamples = trainingSet->numDigits;

  classifier->weakClassifiers = NewDecisionTreeSet(maxNumClassifiers);
  classifier->numClassifiers = 0;

  classifier->exampleWeights =
    (float *) malloc(trainingSet->numDigits * sizeof(float));

  for(int digitNum = 0; digitNum < trainingSet->numDigits; digitNum++) {
    classifier->exampleWeights[digitNum] = 0; // Changed from: 1.0f / float(trainingSet->numDigits);
  }

  classifier->positiveLabel = positiveLabel;
  classifier->treeDepth = treeDepth;

  return classifier;
}

// Frees the memory associated with this classifier.  
// Note: We do not free the trainingSet that is passed into 
//  NewBaggedDecisionTree
void FreeBaggedDecisionTree(BaggedDecisionTree* classifier) {
  FreeDecisionTreeSet(classifier->weakClassifiers, true);

  free(classifier->exampleWeights);

  free(classifier);
}

// Reads the classifier from a file.
BaggedDecisionTree* ReadBaggedDecisionTree(FILE* file) {
  BaggedDecisionTree* classifier = 
    (BaggedDecisionTree*) malloc(sizeof(BaggedDecisionTree));

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

  return classifier;
}

// Writes the classifier to a file.
void WriteBaggedDecisionTree(BaggedDecisionTree* classifier, FILE* file) {
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
}


// Performs one step of bagging: samples a training set to use, and
// adds a decision tree trained on that training set. All of this
// should be treating classifier->positiveLabel as the positive class.
// This allows you to easily graph the accuracy as a function of the
// number of trees.
// This function should update the example weights to represent the
// randomly sampled examples.
void AddDecisionTree(BaggedDecisionTree* classifier) {
  assert(classifier->numClassifiers < classifier->weakClassifiers->numTrees);

  // TODO: Use bagging to add a decision tree.




  // Increment the count of classifiers
  (classifier->numClassifiers)++;
}

// Returns our confidence that the given digit is from the positiveLabel
// class.  If the classifier was created with 
//   NewBaggedDecisionTree(trainingSet, 200, 2, maxDepth)
// then it would return the confidence that the digit is a 2.
// The confidence can be measured as the number of decision trees that
// classify the digit into the positive class.
float PositiveConfidence(BaggedDecisionTree* classifier,
			 Digit* digit) {
  // TODO: Compute BaggedDecisionTree's output.
  assert(false); // remove





  return 0;
}
