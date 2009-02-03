#include "TestHaarFeatures.h"
#include <iostream>
#include <vector>
#include <string>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

using namespace std;

string whiteImageFile("./testimages/white.jpg");
string blackImageFile("./testimages/black.jpg");
string haarVImageFile("./testimages/haar-v.jpg");
string haarHImageFile("./testimages/haar-h.jpg");

int main(int argc, char** argv) {
  TestHaarFeatures::test();
}

/**
 * For debugging purposes - displays an image
 */
void displayImage() {
  IplImage *image = cvLoadImage("/afs/ir/class/cs221/vision/data/mug/mug0001.jpg", 0);
  cvNamedWindow("Image1", CV_WINDOW_AUTOSIZE);
  cvShowImage("Image1", image);
  cvWaitKey(0);
  cvReleaseImage(&image);
  cvDestroyWindow("Image1");
}

/**
 * Tests a simple white image
 */
vector<double> getFeatureValues(string filename) {
  IplImage *image = cvLoadImage(filename.c_str(), 0);

  IplImage *resizedImage = cvCreateImage(
    cvSize(64, 64),
    image->depth,
    image->nChannels);
  cvResize(image, resizedImage);

  vector<double> values;
  HaarFeatures haar;
  haar.getFeatureValues(values, resizedImage);
  return values;
}

bool testBlackImage() {
  vector<double> values = getFeatureValues(blackImageFile);
  for(vector<double>::size_type i = 0; i < values.size(); i++) {
    //cerr << i << ": " << values[i] << endl;
    if(values[i] != 0) {
      cerr << "Problem with black image" << endl;
      return false;
    }
  }
  return true;
}

bool testWhiteImage() {
  int zeroCount = 0; //Number of 0s
  int halfCount = 0; //Number of 0.5s
  vector<double> values = getFeatureValues(whiteImageFile);
  for(vector<double>::size_type i = 0; i < values.size(); i++) {
    //cerr << i << ": " << values[i] << endl;
   
    if(values[i] == 0) {
      zeroCount++;
    }
    if(values[i] == 0.5) {
      halfCount++;
    }
  }
 
  if((zeroCount != 48) || (halfCount != 9)) {
    cerr << "Problem with white image" << endl;
    cerr << "Zero count: " << zeroCount << endl;
    cerr << "White count: " << halfCount << endl;
    return false;
  }
  return true;
}

void printFeatures(vector<double> values) {
  for(vector<double>::size_type i = 0; i < values.size(); i++) {
    cerr << i << ": " << values[i] << endl;
  } 

}

bool testHaarVImage() {
  vector<double> values = getFeatureValues(haarVImageFile);
  if(values[50] != 1) {
    cerr << "Problem with haar-v image" << endl;
    return false;
  }
  //printFeatures(values);
  return true;
}

bool testVectorSize() {
  vector<double> values = getFeatureValues(haarVImageFile);
  if(values.size() != 57) {
    cerr << "Problem with vector size" << endl;
    return false;
  }
  return true;
}


bool TestHaarFeatures::test() {
  testWhiteImage();
  testBlackImage();
  testHaarVImage();
  testVectorSize();
  return true;
}
