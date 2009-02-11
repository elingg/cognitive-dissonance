#ifndef ONEAGAINSTMANY_H
#define ONEAGAINSTMANY_H

#include "Digit.h"

#include "DecisionTree.h"
#include "BaggedDecisionTree.h"
#include "BoostedDecisionTree.h"

// Each one of these functions returns the accuracy of the classifiers on the
// given test set.  Implement the One-Against-Many functionality here.
// classifiers[j] should be a classifier which treats j as the positive digit.
//
// These functions should make use of the PositiveConfidence functions, so you
// should make sure that those work before using these functions.

float DecisionTreeAccuracy(DecisionTreeSet* classifiers,
			   DigitSet* testSet);

float BaggedDecisionTreeAccuracy(BaggedDecisionTree** classifiers,
				 DigitSet* testSet);

float BoostedDecisionTreeAccuracy(BoostedDecisionTree** classifiers,
				 DigitSet* testSet);

int GetBoostedTreePrediction(BoostedDecisionTree** classifiers, 
         Digit* digit);
#endif
