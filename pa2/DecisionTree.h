#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include "Digit.h"

#include <stdio.h>

static const int NUM_CHILDREN = 2;

// Struct: DecisionTree
//
// Represents a node in a decision tree.  This data structure is recursive.
typedef struct DecisionTree {
  bool isLeaf;

  // Pointers to the child nodes of the tree.
  // Leaf nodes will have NULL pointers.
  int pixelNum;
  float threshold;
  DecisionTree* children[NUM_CHILDREN];

  // Statistics on the number of examples that make it to this leaf.
  // These counts are real numbers, just in case weights are real numbers.
  // These fields are only used for leaf nodes.
  float positiveCount;
  float totalCount;
} DecisionTree;

// Learns a decision tree based on the given weights.
// This function splits the training set and recursively calls itself.
// It should initialize the probabilities when it gets to a leaf node.
DecisionTree* GrowDecisionTree(DigitSet* digitSet, float* exampleWeights,
			       int positiveLabel, int depth);

// Frees the memory associated with a DecisionTree.
void FreeDecisionTree(DecisionTree* tree);

// Reads a decision tree from a file.
DecisionTree* ReadDecisionTree(FILE* file);

// Prints the tree to a FILE (can be stdout).  
void WriteDecisionTree(DecisionTree* tree, FILE* file);

// Returns true if we classify the given digit into the positive class.
//
// This function is equivalent to PositiveConfidence(tree, digit) > 0.5
bool IsPositiveClass(DecisionTree* tree, Digit* digit);

// Returns our confidence that the digit is in the positive class.  
float PositiveConfidence(DecisionTree* tree, Digit* digit);


// Struct: DecisionTreeSet
//
// Simple struct for a list of DecisionTrees, so you don't have to keep
// passing sizes around with arrays.
typedef struct DecisionTreeSet {
  DecisionTree** trees;
  int numTrees;
};

// Creates a new decision tree set.  The trees will be initialized to NULL.
DecisionTreeSet* NewDecisionTreeSet(int numTrees);

// Frees the memory associated with a DecisionTreeSet.
// If freeTrees is true, we will also free the trees contained in our set.
void FreeDecisionTreeSet(DecisionTreeSet* treeSet, bool freeTrees);

// Reads a set of decision trees from the file.
DecisionTreeSet* ReadDecisionTreeSet(FILE* file);

// Writes a set of decision trees to file.
void WriteDecisionTreeSet(DecisionTreeSet* treeSet, FILE* file);

#endif
