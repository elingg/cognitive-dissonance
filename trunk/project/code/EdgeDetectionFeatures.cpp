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

//Represents a single line in the edgedetectionfeatures.txt
struct EdgeDetectionFeature {
  int x;        //x position
  int y;        //y position 
  int w;        //width
  int h;        //height
};

//Holds all the features given in edgedetectionfeatures.txt
vector<EdgeDetectionFeature> features;

/**
 * For debugging purposes - prints out feature
 */
void printFeature(EdgeDetectionFeature f) {
//  cerr << "Processing: " << f.x << " " << f.y << " " << f.w << " " << f.h;
}

/**
 * For debugging purposes - prints out image details
 */
void printImageDetails(const IplImage* img) {
  cout << "width: " << img->width;
  cout << "height: " << img->height;
}

/**
 * Compute feature values based on Integral Image
 */
cvHistogram * computeFeatureValueIntegral(const IplImage* iImage, EdgeDetectionFeature f) {

  cvSobel(iImage,iImage);
  CvHistogram * hist;
  int bins = 32;
  int hist_size[] = {bins};
  hist = cvCreateHist(1,hist_size, CV_HIST_ARRAY, NULL, 1);
  cvCalcHist(&iImage,hist);

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
void EdgeDetectionFeatures::getFeatureValues(vector<cvHistogram>& feature_values,
                                    const IplImage* img) const {
  //Image should be 64x64 (because the features are based on 64x64 images)
  assert(img->width == 64);
  assert(img->height == 64);
 
  IplImage *iImage = cvCreateImage(cvSize(img->width + 1, img->height + 1),                                        IPL_DEPTH_32S, 1);
                                   cvIntegral(img, iImage);
  
    double feature_value = computeFeatureValueIntegral(iImage);
    feature_values.push_back(feature_value);
    //printFeature(features[i]);
    //cerr << "Feature value [" << i << "]: " << feature_value << endl;
  }

  cvReleaseImage(&iImage);
}


}


