#include "OneAgainstMany.h"

#include <assert.h>

// Each one of these functions returns the accuracy of the classifiers on the
// given test set.  Implement the One-Against-Many functionality here.
// classifiers[j] should be a classifier which treats j as the positive digit.
//
// These functions should make use of the PositiveConfidence functions. so you
// should make sure that those work before using these functions.

float DecisionTreeAccuracy(DecisionTreeSet* classifiers,
			   DigitSet* testSet) {
  // TODO
  assert(false); // remove




  return 0;
}

float BaggedDecisionTreeAccuracy(BaggedDecisionTree** classifiers,
				 DigitSet* testSet) {
  // TODO
  assert(false); // remove




  return 0;
}
