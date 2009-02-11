#include "OneAgainstMany.h"
#include <iostream>
#include <assert.h>

using namespace std;

// Each one of these functions returns the accuracy of the classifiers on the
// given test set.  Implement the One-Against-Many functionality here.
// classifiers[j] should be a classifier which treats j as the positive digit.
//
// These functions should make use of the PositiveConfidence functions. so you
// should make sure that those work before using these functions.

float DecisionTreeAccuracy(DecisionTreeSet* classifiers,
			   DigitSet* testSet) {
  // exactly similar implementation as bagged classifiers...

  const int NUM_DIGITS = 10;
  int numCorrect = 0;
  int totalTested = 0;

  //Iterate through all the examples in th test set
  for(int digitIndex = 0; digitIndex < testSet->numDigits; digitIndex++) {
    int bestDigit = 0;
    float maxConfidence = 0.0;
    
    //Out of all the digits (from 1 to 10), we find the best one
    for(int i = 0; i < NUM_DIGITS; i++) {
      float confidence = PositiveConfidence(classifiers->trees[i], 
                                            testSet->digits[digitIndex]);
      if(confidence > maxConfidence) {
        maxConfidence = confidence;
        bestDigit = i;
      }
    }
    //cerr << "Max confidence was: " << maxConfidence << endl;
    //Check if our choice was the correct one
    if((testSet->digits[digitIndex]->label) == bestDigit) {
      numCorrect++;
    }
    totalTested++;
  }

  return (float) numCorrect / (float) totalTested;
  // DONETODO
  // assert(false); // remove
}

float BaggedDecisionTreeAccuracy(BaggedDecisionTree** classifiers,
				 DigitSet* testSet) {
  const int NUM_DIGITS = 10;
  int numCorrect = 0;
  int totalTested = 0;

  //Iterate through all the examples in th test set
  for(int digitIndex = 0; digitIndex < testSet->numDigits; digitIndex++) {
    
    int bestDigit = 0;
    float maxConfidence = 0.0;

    //Out of all the digits (from 1 to 10), we find the best one
    for(int i = 0; i < NUM_DIGITS; i++) {
      float confidence = PositiveConfidence(classifiers[i], 
                                            testSet->digits[digitIndex]);

      //cerr << "Confidence for digit " << i << " is: " << confidence << endl;
      if(confidence > maxConfidence) {
        maxConfidence = confidence;
        bestDigit = i;
      }
    }

    //Check if our choice was the correct one
    if((testSet->digits[digitIndex]->label) == bestDigit) {
      numCorrect++;
    }
    totalTested++;
  }

  return (float) numCorrect / (float) totalTested;
}

float BoostedDecisionTreeAccuracy(BoostedDecisionTree** classifiers,
				 DigitSet* testSet) {
  const int NUM_DIGITS = 10;
  int numCorrect = 0;
  int totalTested = 0;

  //Iterate through all the examples in th test set
  for(int digitIndex = 0; digitIndex < testSet->numDigits; digitIndex++) {
    
    int bestDigit = 0;
    float maxConfidence = 0.0;

    //Out of all the digits (from 1 to 10), we find the best one
    for(int i = 0; i < NUM_DIGITS; i++) {
      float confidence = PositiveConfidence(classifiers[i], 
                                            testSet->digits[digitIndex]);

      //cerr << "Confidence for digit " << i << " is: " << confidence << endl;
      if(confidence > maxConfidence) {
        maxConfidence = confidence;
        bestDigit = i;
      }
    }

    //Check if our choice was the correct one
    if((testSet->digits[digitIndex]->label) == bestDigit) {
      numCorrect++;
    }
    totalTested++;
  }

  return (float) numCorrect / (float) totalTested;
}

int GetBoostedTreePrediction(BoostedDecisionTree** classifiers,
         Digit* digit) {
  int bestDigit = 0;
  float maxConfidence = 0.0;
  //Out of all the digits (from 1 to 10), we find the best one
  for(int i = 0; i < 10; i++) {
    float confidence = PositiveConfidence(classifiers[i], 
                                          digit);
      // cerr << "Confidence for digit " << i << " is: " << confidence << endl;
      if(confidence > maxConfidence) {
        maxConfidence = confidence;
        bestDigit = i;
      }
  }
  return bestDigit;
}

