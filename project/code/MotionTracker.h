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
 * Lucas Kanade optical flow for multiple objects.  The built-in OpenCV
 * doesn't handle the specific case of different objects.
 */
class LucasKanade {
  public:
    LucasKanade();
    ~LucasKanade();
    CObjectList getObjectList(IplImage* frame, CObjectList* classifierObjects);
  private:
    void initialize(IplImage* frame);

    int frameCount;
    IplImage* prevGrayFrame;
    CvPoint2D32f *cornersA;
    CvPoint2D32f *cornersB;

    IplImage* eigImage;
    IplImage* tmpImage;
    IplImage* pyramidA;
    IplImage* pyramidB;
};

/**
 * Tracker
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
    CvBlob blob1;
    CObject cvBlobToCObject(CvBlob *blob);
    CvBlob cObjectToCVBlob(CObject cObject);
};


