#include "Digit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Memory routines for the digit structs.

Digit* NewDigit(int numRows, int numCols) {
  Digit* digit = (Digit*) malloc(sizeof(Digit));

  digit->numRows = numRows;
  digit->numCols = numCols;
  digit->numPixels = numRows * numCols;

  digit->label = -1;

  digit->pixels = (float*) malloc(digit->numPixels * sizeof(float));

  return digit;
}

void FreeDigit(Digit* digit) {
  free(digit->pixels);
  free(digit);
}

DigitSet* NewDigitSet(int numDigits, int numRows, int numCols) {
  DigitSet* digitSet = (DigitSet*) malloc(sizeof(DigitSet));

  digitSet->numRows = numRows;
  digitSet->numCols = numCols;
  digitSet->numPixels = numRows * numCols;

  digitSet->numDigits = numDigits;

  digitSet->digits = 
    (Digit**) malloc(numDigits * sizeof(Digit*));

  return digitSet;
}

void FreeDigitSet(DigitSet* digitSet, bool freeDigits) {
  if(freeDigits) {
    for(int digitNum = 0; digitNum < digitSet->numDigits; digitNum++) {
      FreeDigit(digitSet->digits[digitNum]);
    }
  }

  free(digitSet->digits);
  free(digitSet);
}


/*
 * Stuff written by ADP, Sun Nov  1 19:12:42 1998
 * to access .idx_ files at 
 *
 * http://www.research.att.com/~yann/ocr/mnist/index.html 
 *
 * Updated by Dave Mulford, Tuesday September 30 2003
 */
   
/* ReadBinaryInt
 *
 * Reads the next 4 bytes from the file and interprets them as an int.
 * The first byte read is the most significant byte.
 *
 */
int ReadBinaryInt(FILE* fp) 
{
  unsigned char byteArray[4];

  byteArray[0] = getc(fp);
  byteArray[1] = getc(fp);
  byteArray[2] = getc(fp);
  byteArray[3] = getc(fp);

  return 
    256 * 256 * 256 * byteArray[0] +
    256 * 256 * byteArray[1] +
    256 * byteArray[2] +
    byteArray[3];
}

/* WriteBinaryInt
 *
 * Writes an int to a file as 4 bytes.
 * The most significant byte is written first.
 * 
 */
void WriteBinaryInt(FILE* fp, int i) 
{
  putc((unsigned char) (i >> 24), fp); 
  putc((unsigned char) (i >> 16), fp); 
  putc((unsigned char) (i >> 8), fp); 
  putc((unsigned char) (i >> 0), fp); 
}

/* FOpenOrDie
 *
 * Opens a file, or dies trying 
 *
 */
FILE* FOpenOrDie(char* filename, char* mode) 
{
  FILE* fp;
  
  fp = fopen(filename, mode);
  if (!fp) { 
    fprintf(stderr, "FOpenOrDie: File %s not found.\n", filename); 
    exit(-1); 
  }
  
  return fp;
}

/*  OpenMagicNumberFile
 *
 *  This is a special fopen which opens a file and returns a
 *  pointer.  If the file does not begin with the given magic number, this procedure
 *  exits with value -1.  The pointer will be pointing after the magic number,
 *  i.e. 4 bytes into the file.  The pointer will be read-only.
 */
FILE* OpenMagicNumberFile(char* filename, int magicNumber) {
  FILE* fp;
  int readMagicNumber;

  fp = FOpenOrDie(filename, "r");
  readMagicNumber = ReadBinaryInt(fp);

  if (magicNumber != readMagicNumber) {
    fprintf(stderr, "OpenMagicNumberFile: %s is not a legal file (magic number is %d)\n",
	    filename, readMagicNumber); 
    exit(-1); 
  }
  
  return fp; 
}

 
/*  OpenLabelFile
 *
 *  This is a special fopen which opens a label file and returns a
 *  pointer.  If the file is not a legal label file, this procedure
 *  exits with value -1.  The pointer will be read-only.
 *
 */
FILE* OpenLabelFile(char* filename) 
{
  return OpenMagicNumberFile(filename, 2049);
}
  

/* OpenImageFile
 *
 * This is a special fopen which opens an image file and returns a
 * pointer.  If the file is not a legal image file, this procedure
 * exits with value -1.  The pointer will be read-only.
 *
 */
FILE* OpenImageFile(char* filename) 
{
  return OpenMagicNumberFile(filename, 2051);
}
  

/* LoadDigitImages
 *
 * Loads in the digit pixel data from the files into an array of
 * Digits for faster lookup. labels are all set to -1.
 * The file pointer is expected to be opened for reading.
 */
DigitSet* LoadDigitImages(FILE* imageFp)
{
  fseek(imageFp, 4, SEEK_SET);

  int numDigits = ReadBinaryInt(imageFp);
  int numRows = ReadBinaryInt(imageFp);
  int numCols = ReadBinaryInt(imageFp);

  DigitSet* digitSet = NewDigitSet(numDigits, numRows, numCols);

  /* allocate memory for each digit's data */
  for(int digitNum = 0; digitNum < digitSet->numDigits; digitNum++) {     
    digitSet->digits[digitNum] = NewDigit(numRows, numCols);
  }

  /* read digit data */
  for(int digitNum = 0; digitNum < digitSet->numDigits; digitNum++) {
    if (digitNum % 100 == 0) {       
      fprintf(stderr, "...%d", digitSet->numDigits - digitNum); 
    }
    for(int pixelNum = 0; pixelNum < digitSet->numPixels; pixelNum++) {
      unsigned char c = getc(imageFp);
      digitSet->digits[digitNum]->pixels[pixelNum] = 
	((float) c) / 255.0;
    }
  }
  fprintf(stderr, "\n");

  return digitSet;
}
  
/* LoadDigitLabels
 *
 * This is like load_digit_data_only but also loads in label
 * information.  Again, size (number of digits read in) is returned in 
 * num.
 *
 */
void LoadDigitLabels(FILE* labelFp, DigitSet* digitSet)
{
  fseek(labelFp, 4, SEEK_SET);

  int numLabels = ReadBinaryInt(labelFp);
  
  if (numLabels != digitSet->numDigits) {
    fprintf(stderr, "LoadDigitLabels: The image file and the label file have different lengths.\n");
    exit(-1); 
  }

  /* Read the labels */
  for(int digitNum = 0; digitNum < numLabels; digitNum++) {
    digitSet->digits[digitNum]->label = getc(labelFp);

    if (digitSet->digits[digitNum]->label == EOF) {
      fprintf(stderr, "LoadDigitLabels: Error!  label file \n");
      fprintf(stderr, "should have more entries than this.  File must be corrupt.\n");
      exit(-1); 
    }
  }
}

/* LoadDigits
 * 
 * Loads the images and labels from the two given files and returns
 * a digit set representing the digits.
 */
DigitSet* LoadDigits(char* imageFilename, char* labelFilename) {
  FILE* imageFp = OpenImageFile(imageFilename);
  FILE* labelFp = OpenLabelFile(labelFilename);

  DigitSet* digitSet = LoadDigitImages(imageFp);
  LoadDigitLabels(labelFp, digitSet);

  fclose(imageFp);
  fclose(labelFp);

  return digitSet;
}


/* SaveDigitImages
 *
 * Given a DigitSet, saves digits s1
 * to e2, inclusive, to img_filename.  Labels are not saved, just
 * image data.
 *
 */
void SaveDigitImages(DigitSet* digitSet, char* imageFilename, 
		     int start, int end)
{
  int i, j;
  FILE *fp;
  int num;

  num = end - start + 1;
  fp = FOpenOrDie(imageFilename, "w");

  /* 1. magic number for image file */
  putc(0, fp); putc(0, fp); putc(8, fp); putc(3, fp); 

  
  /* 2. number of items */
  WriteBinaryInt(fp, num);

  /* 3. n rows */
  WriteBinaryInt(fp, digitSet->digits[start]->numRows);

  /* 4. n cols */
  WriteBinaryInt(fp, digitSet->digits[start]->numCols);

  /* 5. data! */
  for (i= start; i <= end; i++) {
    for(j=0; j < digitSet->digits[i]->numRows * digitSet->digits[i]->numCols; j++) {
      putc((unsigned char)(digitSet->digits[i]->pixels[j] * 255), fp);
    }
  }
  
  fclose(fp);
}

/* SaveDigitLabels
 *
 * Given a DigitSet, saves the labels
 * from s1 to e2, inclusive, to the label_filename.  Labels are
 * saved, not the image data.
 *
 */
void SaveDigitLabels(DigitSet* digitSet, char* labelFilename, 
		       int start, int end)
{
  int i;
  FILE *fp;
  int num;

  num = end - start + 1;
  fp = FOpenOrDie(labelFilename, "w");

  /* 1. magic number for image file */
  putc(0, fp); putc(0, fp); putc(8, fp); putc(1, fp); 
  
  /* 2. number of items */
  WriteBinaryInt(fp, num);

  /* 3. labels! */
  for (i=start; i <= end; i++) {
    putc(digitSet->digits[i]->label, fp); 
  }

  fclose(fp);
}

/*
 * SaveDigits
 * 
 * Saves the images and labels to the two given files, in the
 * range start to end, inclusive.
 */
void SaveDigits(DigitSet* digitSet, char* imageFilename, 
		char* labelFilename, int start, int end) {
  SaveDigitImages(digitSet, imageFilename, start, end);
  SaveDigitLabels(digitSet, labelFilename, start, end);
}



/* PrintDigit
 *
 * This procedure accesses and prints the data of a Digit.
 * Output goes to stdout.
 */
void PrintDigit(Digit* digit)
{  
  int i, j;
  int pixelNum;
  int pixelValue;

  printf("PrintDigit: Number is %d\n", digit->label); 
  
  /* get image of digit */
  for (i = 0; i < digit->numRows; i++) {
    for(j = 0; j < digit->numCols; j++) {      
      pixelNum = i * digit->numRows + j;
      pixelValue = (int) (255 * digit->pixels[pixelNum]);
      printf("%3d ", pixelValue);
    }
    
    printf("\n"); 
  }
}

