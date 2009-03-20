#include "EdgeDetectionFeatures.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

//TODO: Use C++ classes properly


/**
 * Compute feature values based on Integral Image
 */
CvHistogram* computeFeatureValueIntegral(const IplImage* iImage) {

  IplImage * destination;

 destination = (IplImage*) iImage;
//  cvSobel(iImage,destination,1,0,3);

  CvHistogram * hist;
  int bins = 80;
  int hist_size[] = {bins};
  hist = cvCreateHist(1,hist_size, CV_HIST_ARRAY, NULL, 1);
  cvCalcHist(&destination,hist);

  return hist;

}

/**
 * Reads in the feature list and saves it
 */
EdgeDetectionFeatures::EdgeDetectionFeatures() {
}

EdgeDetectionFeatures::~EdgeDetectionFeatures() {}

/**
 * Returns edge detection feature values
 * Precondition: image should be 64x64
 */
void EdgeDetectionFeatures::getFeatureValues(vector<double>& feature_values,
                                    const IplImage* img) const {
  //Image should be 64x64 (because the features are based on 64x64 images)
  assert(img->width == 64);
  assert(img->height == 64);
 
  
   int bins = 80; 
   CvHistogram* hist = computeFeatureValueIntegral(img);

  for(int i =0; i<bins; i++)
  {
        feature_values.push_back(cvQueryHistValue_1D(hist,i));
  }

  cvReleaseHist(&hist);
  

}




