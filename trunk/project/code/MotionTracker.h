#pragma once
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "objects.h"


class MotionTracker {
  public:
    MotionTracker();
    ~MotionTracker();
};

/**
 * Kalman Filter for a group of objects
 */
class KalmanFilter {
  public:
    KalmanFilter();
    ~KalmanFilter();
    void update(CObjectList* classifierObjects);
    CObjectList predict();
  private:
    CObjectList kalmanObjects;
    CvBlobTrackPredictor* predictor1;
    CvBlob blob1;
};
