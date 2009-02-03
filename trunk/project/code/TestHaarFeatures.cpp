#include "TestHaarFeatures.h"
#include <iostream>
#include <vector>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

using namespace std;

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
bool testWhite() {
  IplImage *image = cvLoadImage("./testimages/white.jpg", 0);

  IplImage *resizedImage = cvCreateImage(
    cvSize(64, 64),
    image->depth,
    image->nChannels);
  cvResize(image, resizedImage);

  vector<double> values;
  HaarFeatures haar;
  haar.getFeatureValues(values, resizedImage);
  for(vector<double>::size_type i = 0; i < values.size(); i++) {
    cerr << i << ": " << values[i] << endl;
    if((values[i] != 0) && (values[i] != 0.5)) {
      cerr << "Problem with HaarFeatures calculation" << endl;
      return false;
    }
  }
  return true;

}

bool TestHaarFeatures::test() {
  testWhite();
  return true;
}


