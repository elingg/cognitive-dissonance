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
  cerr << "Processing: " << f.x << " " << f.y << " " << f.w << " " << f.h;
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
double computeFeatureValueIntegral(const IplImage* iImage, EdgeDetectionFeature f) {

    double xEdge = 0.0;
    double yEdge = 0.0;
    double value = 0.0;

  double z = 0.0;  //normalizing constant
  for(int i = 0; i < f.w; i++) {
    for(int j = 0; j < f.h; j++) {
      CvScalar s = cvGet2D(img, f.y + j, f.x + i);
      z += s.val[0];
    }
  }

  for(int j = 0; j < f.h; j++) {
    for(int i = 0; i < (f.w -1); i++) {
       CvScalar s = cvGet2D(img, f.y + j, f.x + i);
       tempval = s.val[0];
       s = cvGet2D(img, f.y + j, f.x + i+1);
       tempval += s.val[0]  * -1;
       if(Math.abs(tempval)>value)
	{
	  value = Math.abs(tempval);
	  xEdge = f.x + j + 1;
	  yEdge = f.y + i;
	}
      }
    }
  

  if(z != 0) {
    return abs(value) / z; 
  } else {
    //cerr << "Image was black." << endl;
    return 0;
  }
}







/**
 * Reads in the feature list and saves it
 */
EdgeDetectionFeatures::EdgeDetectionFeatures() {
  //TODO: This check shouldn't be necessary (I don't understand why I need it)
  if(features.size() == 0) {
    cerr << "Start: building edge detection features list" << endl;
    ifstream input;
    //TODO: Externalize this filename
    input.open("edgedetectionfeatures.txt");
    if(!input) {
      cerr << "Error: unable to open edgedetectionfeatures.txt" << endl;
    }
    int x, y, w, h;
    string type;
    while(input >> x >> y >> w >> h >> type) {
      EdgeDetectionFeature feature;
      feature.x = x;
      feature.y = y;
      feature.h = h;
      feature.w = w;
      feature.type = type;
      features.push_back(feature);
    }
    input.close();
    cerr << "End: finished building edge detection features list" << endl;
  }
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
 
  IplImage *iImage = cvCreateImage(cvSize(img->width + 1, img->height + 1),                                        IPL_DEPTH_32S, 1);
                                   cvIntegral(img, iImage);
  
  for(vector<EdgeDetectionFeature>::size_type i = 0; i < features.size(); i++) {
    double feature_value = computeFeatureValueIntegral(iImage, features[i]);
    feature_values.push_back(feature_value);
    //printFeature(features[i]);
    //cerr << "Feature value [" << i << "]: " << feature_value << endl;
  }

  cvReleaseImage(&iImage);
}


}


