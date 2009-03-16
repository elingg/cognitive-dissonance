#include "KalmanFilter.h"
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "objects.h"
#include "highgui.h"
#include <iostream>

using namespace std;

/**
 * Kalman Filter
 */
KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

//TODO: Merge update and predict functions after experimenting with different ordering
void KalmanFilter::update(CObjectList* classifierObjects) {
  cout << "Objects found: " << classifierObjects->size() << endl;

  size_t numFilters = filters.size(); //need this because the filters.size changes
  bool shouldPenalize[numFilters];
  for(size_t i = 0; i < numFilters; i++) {
    shouldPenalize[i] = true;
  }

  for(size_t i = 0; i < classifierObjects->size(); i++) {
    bool foundMatchingBlob = false;
    for(size_t j = 0; j < filters.size(); j++) {
      CObject predicted = filters[j].predict();
      int overlap = predicted.overlap(classifierObjects->at(i));
      //cout << "Overlap is" << overlap << endl;
      //cout << "Size is" << predicted.area() << endl;
      //cout << "Size is" << classifierObjects->at(i).area() << endl;
      double overlapRatio = (double) (2*overlap) /
                            (predicted.area() + classifierObjects->at(i).area()); 
      //check overlap between filter and classifier object
      if(overlapRatio >= 0.8) {
        filters[j].update(classifierObjects->at(i));
	filters[j].incrementCount();
	shouldPenalize[j] = false;
        foundMatchingBlob = true;
	break;
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
 * Utility function to convert a CvBlob to a CObject
 * TODO: Externalize label name
 */
CObject cvBlobToCObject(CvBlob* blob) {
  CObject obj;
  obj.rect = cvRect(0, 0, blob->w, blob->h); 
  obj.rect.x = blob->x;
  obj.rect.y = blob->y;
  obj.label = string("mug");
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
 * Generic Tracker for each Object 
 */
KFObject::KFObject() {
  count = 0;
  predictor = cvCreateModuleBlobTrackPredictKalman();
};

KFObject::~KFObject() {};

CObject KFObject::predict() {
  CvBlob* predicted = predictor->Predict();
  return cvBlobToCObject(predicted);
}

void KFObject::update(CObject object) {
  lastObject = object;
  CvBlob blob = cObjectToCVBlob(object);
  predictor->Update(&blob); 
}

void KFObject::incrementCount() {
  count++;
}

void KFObject::decrementCount() {
  count--;
}

int KFObject::getCount() {
  return count;
}

