#include "KalmanFilter.h"
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "objects.h"
#include "highgui.h"
#include <iostream>

using namespace std;

/**
 * Utility function to convert a CvBlob to a CObject
 */
CObject cvBlobToCObject(CvBlob* blob, string label) {
  CObject obj;
  obj.rect = cvRect(0, 0, blob->w, blob->h); 
  obj.rect.x = blob->x;
  obj.rect.y = blob->y;
  obj.label = label;
  return obj; 
}

/**
 * Utility function to convert a CObject to CvBlob
 */
CvBlob cObjectToCVBlob(CObject cObject) {
  CvBlob blob; 
  blob.x = cObject.rect.x;
  blob.y = cObject.rect.y;
  blob.w = cObject.rect.width;
  blob.h = cObject.rect.height;
  return blob;
}

/**
 * Kalman Filter
 */
KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

/**
 * Runs the update step for all the filters
 */
void KalmanFilter::update(CObjectList* classifierObjects) {
  size_t numFilters = filters.size(); //need this because the filters.size changes
  bool shouldPenalize[numFilters];
  for(size_t i = 0; i < numFilters; i++) {
    shouldPenalize[i] = true;
  }

  for(size_t i = 0; i < classifierObjects->size(); i++) {
    bool foundMatchingBlob = false;
    for(size_t j = 0; j < filters.size(); j++) {
      CObject predicted = filters[j].predict();
      if(predicted.label == classifierObjects->at(i).label) {
        //check overlap between filter and classifier object
        int overlap = predicted.overlap(classifierObjects->at(i));
        double overlapRatio = (double) (2*overlap) /
                            (predicted.area() + classifierObjects->at(i).area()); 
        if(overlapRatio >= 0.8) {
          filters[j].update(classifierObjects->at(i));
	  filters[j].incrementCount();
	  shouldPenalize[j] = false;
          foundMatchingBlob = true;
	  break;
        }
      }
    }
    if(!foundMatchingBlob) {
      //Create new blob because none was found
      KFObject kfObject;
      kfObject.update(classifierObjects->at(i));
      filters.push_back(kfObject);
    }
  }

  for(size_t i = 0; i < numFilters; i++) {
    if(shouldPenalize[i]) {
      filters[i].decrementCount();
    }
  }
}

/**
 * Runs predict step for all the kalman filters
 */
CObjectList KalmanFilter::predict() {
  kalmanObjects.clear();
  for(size_t i = 0; i < filters.size(); i++) {
    if(filters[i].getCount() > 0) {
      CObject obj = filters[i].predict();
      kalmanObjects.push_back(obj);
    }
  }

  for(size_t i = 0; i < filters.size(); i++) {
    if(filters[i].getCount() < -3) {
      filters.erase(filters.begin() + i);
    }
  }

  return kalmanObjects;
}

/**
 * Individual Kalman Filters for each blob tracked
 */
KFObject::KFObject() {
  count = 0;
  predictor = cvCreateModuleBlobTrackPredictKalman();
};

KFObject::~KFObject() {};

CObject KFObject::predict() {
  CvBlob* predicted = predictor->Predict();
  return cvBlobToCObject(predicted, label);
}

void KFObject::update(CObject object) {
  label = object.label;
  CvBlob blob = cObjectToCVBlob(object);
  predictor->Update(&blob); 
}

int KFObject::getCount() {
  return count;
}

void KFObject::incrementCount() {
  count++;
}

void KFObject::decrementCount() {
  count--;
}
