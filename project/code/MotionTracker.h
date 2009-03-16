#pragma once
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "objects.h"
#include "LucasKanade.h"
#include "KalmanFilter.h"

class MotionTracker {
  public:
    MotionTracker();
    ~MotionTracker();
    CObjectList process(IplImage* frame, CObjectList classifierObjects);
  private:
    KalmanFilter kalmanFilter;
    LucasKanade lucasKanade;
};
