#include "DecisionTree.h"

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>

// Function Prototypes

DecisionTree* GrowChild(DecisionTree* tree, 
			DigitSet* digitSet, float* exampleWeights, 
			int positiveLabel, int depth, int childNum);
void ChoosePixelAndThreshold(DecisionTree* tree, int positiveLabel, 
			     DigitSet* digitSet, float* exampleWeights);
void InitializeProbabilities(DecisionTree* tree, DigitSet* digitSet, 
			     float* exampleWeights, int positiveLabel);
static float Entropy(float p);
static float LogBaseTwo(float x);

// Allocates a new, uninitialized DecisionTree.
DecisionTree* AllocateDecisionTree() {
  DecisionTree* tree = (DecisionTree*) malloc(sizeof(DecisionTree));

  tree->isLeaf = false;

  tree->positiveCount = 0;
  tree->totalCount = 0;

  tree->pixelNum = -1;
  tree->threshold = -1;
  for(int childNum = 0; childNum < NUM_CHILDREN; childNum++) {
    tree->children[childNum] = NULL;
  }

  return tree;
}

// Frees the memory associated with a DecisionTree.
void FreeDecisionTree(DecisionTree* tree) {
  if(!tree->isLeaf) {
    for(int childNum = 0; childNum < NUM_CHILDREN; childNum++) {
      FreeDecisionTree(tree->children[childNum]);
    }
  }

  free(tree);
}

// Recursive Helper function.
DecisionTree* ReadDecisionTree(FILE* file, int numSpaces) {
  DecisionTree* tree = AllocateDecisionTree();

  for(int i = 0; i < numSpaces; i++) {
    fscanf(file, " ");
  }

  char ch;
  fscanf(file, "%c", &ch);

  tree->isLeaf = (ch == 'L');
  if(tree->isLeaf) {
    fscanf(file, "%f / %f\n", &(tree->positiveCount), &(tree->totalCount));
  }
  else {
    fscanf(file, "%d %f\n", &(tree->pixelNum), &(tree->threshold));

    for(int childNum = 0; childNum < NUM_CHILDREN; childNum++) {
      tree->children[childNum] = ReadDecisionTree(file, numSpaces + 1);
    }
  }

  return tree;
}

// Reads a decision tree from a file.
DecisionTree* ReadDecisionTree(FILE* file) {
  char ch;
  fscanf(file, "%c\n", &ch);

  if(ch == 'N') {
    return NULL;
  }
  else {
    return ReadDecisionTree(file, 0);
  }
}

// Recursive Helper function.
void WriteDecisionTree(DecisionTree* tree, FILE* file, int numSpaces) {
  for(int i = 0; i < numSpaces; i++) {
    fprintf(file, " ");
  }

  if(tree->isLeaf) {
    fprintf(file, "L ");

    if(tree->positiveCount == int(tree->positiveCount) &&
       tree->totalCount == int(tree->totalCount) ) {
      // Save space for random trees.
      fprintf(file, "%d / %d\n", int(tree->positiveCount), int(tree->totalCount));
    }
    else {
      fprintf(file, "%f / %f\n", tree->positiveCount, tree->totalCount);
    }
  }
  else {
    fprintf(file, "I ");

    fprintf(file, "%d %.1f\n", tree->pixelNum, tree->threshold);

    for(int childNum = 0; childNum < NUM_CHILDREN; childNum++) {
      WriteDecisionTree(tree->children[childNum], file, numSpaces + 1);
    }
  }
}

// Writes the decision tree to a FILE*.
void WriteDecisionTree(DecisionTree* tree, FILE* file) {
  if(tree == NULL) {
    fprintf(file, "N\n");
  }
  else {
    fprintf(file, "T\n");
    WriteDecisionTree(tree, file, 0);
  }
}

// Helper function to compute which branch to send an example down.
int ComputeChildNum(float threshold, float pixelValue) {
  if(pixelValue > threshold) {
    return 0;
  }
  else {
    return 1;
  }
}

// Returns true if we classify the given digit into the positive class.
//
// This function is equivalent to PositiveConfidence(tree, digit) > 0.5
bool IsPositiveClass(DecisionTree* tree, Digit* digit) {
  return PositiveConfidence(tree, digit) > 0.5;
}

// Returns our confidence that the digit is in the positive class.  
// Return positiveCount / totalCount if we're at a leaf node, otherwise
// recurse down the proper branch.  You can use the ComputeChildNum helper.
float PositiveConfidence(DecisionTree* tree, Digit* digit) {
  // DONETODO: Finish out the implementation of this recursive function.
  // assert(false); // remove
  if(tree->isLeaf) {
    return tree->positiveCount/tree->totalCount;
  }
  int child = ComputeChildNum(tree->threshold, digit->pixels[tree->pixelNum]);
  return PositiveConfidence(tree->children[child], digit);
}

// DONETODO: This function is fully written out, but you might want to
// understand how it works as this implements the core algorithm.
// Learns a decision tree based on the given weights.
// This function splits the training set and recursively calls itself.
// It should initialize the probabilities when it gets to a leaf node.
DecisionTree* GrowDecisionTree(DigitSet* digitSet, float* exampleWeights,
			       int positiveLabel, int depth) {
  if(depth == 0 || digitSet->numDigits <= 1) {
    DecisionTree* tree = 
      (DecisionTree*) malloc(sizeof(DecisionTree));

    tree->isLeaf = true;

    tree->positiveCount = 0;
    tree->totalCount = 0;

    // Pointers to the child nodes of the tree.
    tree->pixelNum = -1;
    tree->threshold = -1;
    for(int childNum = 0; childNum < NUM_CHILDREN; childNum++) {
      tree->children[childNum] = NULL;
    }

    InitializeProbabilities(tree, digitSet, exampleWeights, positiveLabel);

    return tree;
  }
  else {
    DecisionTree* tree = (DecisionTree*) malloc(sizeof(DecisionTree));
    
    tree->isLeaf = false;

    tree->positiveCount = 0;
    tree->totalCount = 0;

    ChoosePixelAndThreshold(tree, positiveLabel, digitSet, exampleWeights);

    for(int childNum = 0; childNum < NUM_CHILDREN; childNum++) {
      tree->children[childNum] = 
      GrowChild(tree,
		  digitSet, exampleWeights,
		  positiveLabel, depth, childNum);
    }

    return tree;
  }
}

// Helper function to GrowDecisionTree which divides the digitSet into
// two parts for the children.  It recursively calls GrowDecisionTree for the
// children.
DecisionTree* GrowChild(DecisionTree* tree, 
			DigitSet* digitSet, float* exampleWeights, 
			int positiveLabel, int depth, int childNum) {
  // Count how many digits are going to the child.
  int count = 0;
  for(int digitNum = 0; digitNum < digitSet->numDigits; digitNum++) {
    
    if(ComputeChildNum(tree->threshold, 
		       digitSet->digits[digitNum]->pixels[tree->pixelNum]) ==
       childNum) {
      count++;
    }
  }

  // Create the sets for the child.
  DigitSet* childDigitSet = 
    NewDigitSet(count, digitSet->numRows, digitSet->numCols);
  float* childExampleWeights = (float*) malloc(count * sizeof(float));

  int childIndex = 0;
  for(int digitNum = 0; digitNum < digitSet->numDigits; digitNum++) {
    if(ComputeChildNum(tree->threshold, 
		       digitSet->digits[digitNum]->pixels[tree->pixelNum]) ==
       childNum) {
      childDigitSet->digits[childIndex] = digitSet->digits[digitNum];
      childExampleWeights[childIndex] = exampleWeights[digitNum];
      childIndex++;
    }
  }

  DecisionTree* returnValue = 
    GrowDecisionTree(childDigitSet, childExampleWeights,
		     positiveLabel, depth - 1);

  // Clean up our memory.
  FreeDigitSet(childDigitSet, false);
  free(childExampleWeights);

  return returnValue;
}

// Uses the entropy calculation to decide which pixel to split on, and
// a threshold value.  This function should set the fields in tree.
void ChoosePixelAndThreshold(DecisionTree* tree, int positiveLabel, 
			     DigitSet* digitSet, float* exampleWeights) {
  // TODO: NEED SOMEONE TO REVIEW THIS
  int chosenPixel = -1;
  float maxGain = -FLT_MAX;
  float chosenThreshold = -1;
  for(int ipixel=0; ipixel<digitSet->numPixels; ipixel++) {
    // go through each pixel (feature) in example
    for(float threshold = 0.1; threshold<1; threshold+=0.1) {
      // for each threshold we try..
      float positiveCount=0.0, totalCount=0.0;
      for(int iexample=0; iexample<digitSet->numDigits; iexample++) {
        // go through each given example
        Digit* example = digitSet->digits[iexample]; 
        float exampleWeight = exampleWeights[iexample];
        float pixelValue = example->pixels[ipixel];
        // add to positive count, total count
        if(example->label==positiveLabel) {
          positiveCount+=pixelValue*exampleWeight;
        }
        totalCount+=pixelValue*exampleWeight;
      }
      // measure entropy using positive and negative counts
      float positiveExampleEntropy = Entropy(positiveCount/totalCount);
      float negativeExampleEntropy = Entropy((totalCount-positiveCount)/totalCount);
      float gain = 0.0; // TODO
      if(gain>maxGain) {
        chosenThreshold = threshold;
        chosenPixel = ipixel;
      }
    }
  }
  tree->pixelNum = chosenPixel;
  tree->threshold = chosenThreshold;
}

// Computes the entropy function, handling the case where p = 0.
static float Entropy(float p) {
  if(p == 0 || p >= 1) {
    return 0;
  }
  else {
    return -(p * LogBaseTwo(p) + (1-p) * LogBaseTwo(1-p));
  }
}

// Computes log_2(x).
static float LogBaseTwo(float x) {
  return log(x) / log(2.0);
}

// Initializes the positiveCount and totalCount variables for a leaf node.
// The digitSet will contain all the digits that end up at this leaf.
// exampleWeights contains the weight of each example.
// positiveCount / totalCount should give the probability that a digit has the
// label positiveLabel.
void InitializeProbabilities(DecisionTree* tree, DigitSet* digitSet, 
			     float* exampleWeights, int positiveLabel) {
  // DONETODO
  for(int iexample=0; iexample<digitSet->numDigits; ++iexample) {
    Digit* example = digitSet->digits[iexample];
    float weight = exampleWeights[iexample];
    if(example->label==positiveLabel) {
      tree->positiveCount+=weight; 
    }
    tree->totalCount+=weight;
  }
}

// Creates a new decision tree set.  The trees will be initialized to NULL.
DecisionTreeSet* NewDecisionTreeSet(int numTrees) {
  DecisionTreeSet* treeSet = (DecisionTreeSet*)
    malloc(sizeof(DecisionTreeSet));

  treeSet->numTrees = numTrees;
  treeSet->trees = (DecisionTree**) malloc(numTrees * sizeof(DecisionTree*));

  for(int treeNum = 0; treeNum < numTrees; treeNum++) {
    treeSet->trees[treeNum] = NULL;
  }

  return treeSet;
}

// Frees the memory associated with a DecisionTreeSet.
// If freeTrees is true, we will also free the trees contained in our set.
void FreeDecisionTreeSet(DecisionTreeSet* treeSet, bool freeTrees) {
  if(freeTrees) {
    for(int treeNum = 0; treeNum < treeSet->numTrees; treeNum++) {
      FreeDecisionTree(treeSet->trees[treeNum]);
    }
  }

  free(treeSet->trees);
  free(treeSet);
}

// Reads a set of decision trees from the file.
DecisionTreeSet* ReadDecisionTreeSet(FILE* file) {
  int numTrees;
  fscanf(file, "%d\n", &numTrees);

  DecisionTreeSet* treeSet = NewDecisionTreeSet(numTrees);

  for(int treeNum = 0; treeNum < numTrees; treeNum++) {
    treeSet->trees[treeNum] = ReadDecisionTree(file);
  }

  return treeSet;
}

// Writes a set of decision trees to file.
void WriteDecisionTreeSet(DecisionTreeSet* treeSet, FILE* file) {
  fprintf(file, "%d\n", treeSet->numTrees);

  for(int treeNum = 0; treeNum < treeSet->numTrees; treeNum++) {
    WriteDecisionTree(treeSet->trees[treeNum], file);
  }
}

