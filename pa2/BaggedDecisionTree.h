#ifndef BAGGEDDECISIONTREE_H
#define BAGGEDDECISIONTREE_H

#include "DecisionTree.h"

// Struct: BaggedDecisionTree
//
// Encapsulates everything needed for a bagged decision tree classifier.  
// exampleWeights are used during the iteration of the algorithm to represent 
// the number of times that example is picked for the training set
typedef struct BaggedDecisionTree {
  DigitSet* trainingSet;            // Our training set.
  int numTrainingExamples;
  
  DecisionTreeSet* weakClassifiers; // The set of weak classifiers
  int numClassifiers;               // The current number of weak classifiers.

  float* exampleWeights;            // The weights for each training instance.

  int positiveLabel;                // Which digit label we consider the positive class.
  int treeDepth;                    // The depth we use when we grow trees.
} BaggedDecisionTree;

// Creates a new BaggedDecisionTree.
BaggedDecisionTree* NewBaggedDecisionTree(DigitSet* trainingSet,
					  int maxNumClassifiers,
					  int positiveLabel,
					  int treeDepth);

// Frees the memory associated with this classifier.  
// Note: We do not free the trainingSet that is passed into 
//  NewBaggedDecisionTree
void FreeBaggedDecisionTree(BaggedDecisionTree* classifier);

// Reads the classifier from a file.
BaggedDecisionTree* ReadBaggedDecisionTree(FILE* file);

// Writes the classifier to a file.  The cached training set is not stored.
void WriteBaggedDecisionTree(BaggedDecisionTree* classifier, FILE* file);

// This function runs the bagging algorithm for one iteration,
// creating a new tree.  This allows you to easily graph the accuracy as a
// function of the number of trees.

// Grows a new decision tree.
void AddDecisionTree(BaggedDecisionTree* classifier);

// Returns our confidence that the given digit is from the positiveLabel
// class.  If the classifier was created with 
//   NewBaggedDecisionTree(trainingSet, 200, 2, maxDepth)
// then it would return the confidence that the digit is a 2.
float PositiveConfidence(BaggedDecisionTree* classifier,
			 Digit* digit);

#endif
