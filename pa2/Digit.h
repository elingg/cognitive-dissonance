#ifndef DIGIT_H
#define DIGIT_H

/* Data Structures */

// Stores a single digit and its label.
typedef struct Digit
{
  float* pixels;  // vector of length numPixels that holds data arry, in row major format
  int label;      // set to -1 if unlabelled

  int numRows;
  int numCols;
  int numPixels;
} Digit;

// Stores an array of digits, and the length of the array.
typedef struct DigitSet
{
  Digit** digits;
  int numDigits;

  int numRows;
  int numCols;
  int numPixels;
} DigitSet;

/* Functions */

// Creates a new digit object.
Digit* NewDigit(int numRows, int numCols);

// Frees the memory associated with a digit object.
void FreeDigit(Digit* digit);

// Creates a new digit set.
DigitSet* NewDigitSet(int numDigits, int numRows, int numCols);

// Frees the memory associated with a digit set.  If freeDigits is true, we
// also call FreeDigit on each of our digits.
void FreeDigitSet(DigitSet* digitSet, bool freeDigits);

// Loads a digit set from disk.
DigitSet* LoadDigits(char* imageFilename, char* labelFilename);

// Saves a digit set to disk.  It saves the digits in the range [start, end].
void SaveDigits(DigitSet* digitSet, char* imageFilename, char* labelFilename, int start, int end);

// Prints an ASCII version of the digit to stdout.
void PrintDigit(Digit* digit);

#endif
