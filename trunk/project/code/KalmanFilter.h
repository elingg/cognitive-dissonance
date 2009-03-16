#pragma once
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "objects.h"

/**
 * A single Kalman Filter (for each object)
 */
class KFObject {
  public:
    KFObject();
    ~KFObject();
    void update(CObject object);
    CObject predict();
    void incrementCount();
    void decrementCount();
    int getCount();
  private:
    CvBlobTrackPredictor* predictor;
    CObject lastObject;
    int count;
};

/**
 * Kalman Filter for a group of objects.  The built-in OpenCV Kalman Filter
 * is only for one object.  This Kalman Filter handles multiple objects.
 */
class KalmanFilter {
  public:
    KalmanFilter();
    ~KalmanFilter();
    void update(CObjectList *classifierObjects);
    CObjectList predict();
  private:
    std::vector<KFObject> filters;
    CObjectList kalmanObjects;
};


