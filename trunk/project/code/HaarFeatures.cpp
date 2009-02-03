#include "HaarFeatures.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

//TODO: Use C++ classes properly

//Represents a single line in the haarfeatures.txt
struct HaarFeature {
  int x;        //x position
  int y;        //y position 
  int w;        //width
  int h;        //height
  string type;  //type
};

//Holds all the features given in haarfeatures.txt
vector<HaarFeature> features;

/**
 * For debugging purposes - prints out feature
 */
void printFeature(HaarFeature f) {
  cerr << "Processing: " << f.x << " " << f.y << " " << f.w << " " << f.h;
  cerr << " " << f.type << endl;
}

/**
 * For debugging purposes - prints out image details
 */
void printImageDetails(const IplImage* img) {
  cout << "width: " << img->width;
  cout << "height: " << img->height;
}

/**
 * Compute feature value based on image and Haar feature
 * Precondition: image should be 64x64
 */
double computeFeatureValue(const IplImage* img, HaarFeature f) {

  //Image should be 64x64 (because the Haar features are based on 64x64 images)
  assert(img->width == 64);
  assert(img->height == 64);
  
  //TODO: Use integral images for faster processing
  /*
  IplImage *iImage = cvCreateImage(cvSize(img->width + 1, img->height + 1), 
                                   IPL_DEPTH_32S, 1);
  cvIntegral(img, iImage);
  value = cvGetReal2D(iImage, feature.y, feature.x);
  value += cvGetReal2D(iImage, feature.y + feature.h, feature.x + feature.w);
  value -= cvGetReal2D(iImage, feature.y, feature.x + feature.w);
  value -= cvGetReal2D(iImage, feature.y + feature.h, feature.x);
  */

  double value = 0.0;

  double z = 0.0;  //normalizing constant
  for(int i = 0; i < f.w; i++) {
    for(int j = 0; j < f.h; j++) {
      CvScalar s = cvGet2D(img, f.x + i, f.y + j);
      z += s.val[0];
    }
  }
  //cerr << "z is " << z << endl;

  //Horizontal - calculation from pg. 9 of problem handout
  if(f.type == "H") {
    for(int i = 0; i < f.w; i++) {
      for(int j = 0; j < (f.h / 2); j++) {
        CvScalar s = cvGet2D(img, f.x + i, f.y + j);  
	value += s.val[0];
      }
    }

    for(int i = 0; i < f.w; i++) {
      for(int j = f.h / 2; j < f.h; j++) {
        CvScalar s = cvGet2D(img, f.x + i, f.y + j);
	value -= s.val[0];
      }
    }

  }

  //Vertical - adaptation of horizontal
  if(f.type == "V") {
    for(int i = 0; i < f.w / 2; i++) {
      for(int j = 0; j < f.h; j++) {
        CvScalar s = cvGet2D(img, f.x + i, f.y + j);  
        value += s.val[0];
      }
    }
    for(int i = f.w / 2; i < f.w; i++) {
      for(int j = 0; j < f.h; j++) {
        CvScalar s = cvGet2D(img, f.x + i, f.y + j);
        value -= s.val[0];
      }
    }
  }

  /**
   * For the next calculations, the strategy is to add up the
   * intensities for *all* the pixels and to subtract out 2 times 
   * the black area. (Note: Variable z already contains the value for all 
   * the pixels.)
   */

  //Top Left
  if(f.type == "TL") {
    double black = 0.0;
    for(int i = 0; i < f.w / 2; i++) {
      for(int j = 0; j < f.h / 2; j++) {
        CvScalar s = cvGet2D(img, f.x + i, f.y + j);
	black += s.val[0];
      }
    }
    value = z - 2 * black;
  }

  //Top Right
  if(f.type == "TR") {
    double black = 0.0;
    for(int i = f.w / 2; i < f.w; i++) {
      for(int j = 0; j < f.h / 2; j++) {
        CvScalar s = cvGet2D(img, f.x + i, f.y + j);
	black += s.val[0];
      }
    }
    value = z - 2 * black;
  }

  //Bottom Left
  if(f.type == "BL") {
    double black = 0.0;
    for(int i = 0; i < f.w / 2; i++) {
      for(int j = f.h / 2; j < f.h; j++) {
        CvScalar s = cvGet2D(img, f.x + i, f.y + j);
	black += s.val[0];
      }
    }
    value = z - 2 * black;
  }

  //Bottom Right
  if(f.type == "BR") {
    double black = 0.0;
    for(int i = f.w / 2; i < f.w; i++) {
      for(int j = f.h / 2; j < f.h; j++) {
        CvScalar s = cvGet2D(img, f.x + i, f.y + j);
	black += s.val[0];
      }
    }
    value = z - 2 * black;
  }

  //Diagonal (which is really a combination of BL and TR)
  if(f.type == "D") {
    double black = 0.0;
    for(int i = 0; i < f.w / 2; i++) {
      for(int j = f.h / 2; j < f.h; j++) {
        CvScalar s = cvGet2D(img, f.x + i, f.y + j);
	black += s.val[0];
      }
    }
    for(int i = f.w / 2; i < f.w; i++) {
      for(int j = 0; j < f.h / 2; j++) {
        CvScalar s = cvGet2D(img, f.x + i, f.y + j);
	black += s.val[0];
      }
    }
    //cerr << "D black is: " << black;
    value = z - 2 * black;
  }

  if(z != 0) {
    return abs(value) / z; 
  } else {
    return 0;
  }
}

/**
 * Reads in the feature list and saves it
 */
HaarFeatures::HaarFeatures() {
  //TODO: This check shouldn't be necessary (I don't understand why I need it)
  if(features.size() == 0) {
    cerr << "Start: building haar features list" << endl;
    ifstream input;
    //TODO: Externalize this filename
    input.open("haarfeatures.txt");
    if(!input) {
      cerr << "Error: unable to open haarfeatures.txt" << endl;
    }
    int x, y, w, h;
    string type;
    while(input >> x >> y >> w >> h >> type) {
      HaarFeature feature;
      feature.x = x;
      feature.y = y;
      feature.h = h;
      feature.w = w;
      feature.type = type;
      features.push_back(feature);
    }
    input.close();
    cerr << "End: finished building haar features list" << endl;
  }
}

HaarFeatures::~HaarFeatures() {}

void HaarFeatures::getFeatureValues(vector<double>& feature_values,
                                    const IplImage* img) const {
  //printImageDetails(img);
  for(vector<HaarFeature>::size_type i = 0; i < features.size(); i++) {
    
    double feature_value = computeFeatureValue(img, features[i]);
    feature_values.push_back(feature_value);
    //printFeature(features[i]);
    //cerr << "Feature value [" << i << "]: " << feature_value << endl;
  }
}
