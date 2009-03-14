#include "MotionTracker.h"
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "objects.h"
#include <iostream>

using namespace std;
/**
 * Contains all the motion tracking code
 */
MotionTracker::MotionTracker() {}
MotionTracker::~MotionTracker() {}

KalmanFilter::KalmanFilter() {
  predictor1 = cvCreateModuleBlobTrackPredictKalman();

  //Hacky way to know if struct is initialized
  //TODO: Ask Anand how to avoid this hack
  blob1.x = -1;
}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::update(CObjectList* classifierObjects) {
  cout << "Objects found: " << classifierObjects->size() << endl;
  for(size_t i = 0; i < classifierObjects->size(); i++) {
    CvBlob blob; 
    blob.x = classifierObjects->at(i).rect.x;
    blob.y = classifierObjects->at(i).rect.y;
    blob.w = classifierObjects->at(i).rect.width;
    blob.h = classifierObjects->at(i).rect.height;
    if(blob1.x == -1) {
      //Assign first blob
      blob1 = blob;
      predictor1->Update(&blob);
    }
    if(sqrt(pow(blob.x - blob1.x, 2) + pow(blob.y - blob1.y, 2)) < 25) {
      //Assign first blob
      blob1 = blob;
      predictor1->Update(&blob);
    }
  }
  CvBlob* predicted = predictor1->Predict();

  //Add prediction to list of Kalman objects
  CObject obj;
  obj.rect = cvRect(0, 0, predicted->w, predicted->h); 
  obj.rect.x = predicted->x;
  obj.rect.y = predicted->y;
  obj.label = string("kalman");
  kalmanObjects.clear();
  kalmanObjects.push_back(obj);
}

CObjectList KalmanFilter::predict() {
  return kalmanObjects;
}
