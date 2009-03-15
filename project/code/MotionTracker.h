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
 * A single Lucas Kanade (for each object)
 */
class LKObject {
  public:
    LKObject();
    void initialize(IplImage* grayFrame, CObject object);
    ~LKObject();
    CObject getNewPosition(IplImage* prevGrayFrame, IplImage* grayFrame, IplImage* frame);
  private:
    int cornerCount;

    CvPoint2D32f *cornersA;
    CvPoint2D32f *cornersB;

    IplImage* eigImage;
    IplImage* tmpImage;
    IplImage* pyramidA;
    IplImage* pyramidB;
    
    CObject cobject;
};

/**
 * Lucas Kanade optical flow for multiple objects.  The built-in OpenCV
 * doesn't handle the specific case of different objects.
 */
class LucasKanade {
  public:
    LucasKanade();
    ~LucasKanade();
    CObjectList process(IplImage* frame, CObjectList* classifierObjects);
  private:
    int frameCount;
    IplImage* prevGrayFrame;
    std::vector<LKObject> blobs;
 
};

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


