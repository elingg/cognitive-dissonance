#pragma once
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "objects.h"

/**
 * A single Lucas Kanade (for each object)
 */
class LKObject {
  public:
    LKObject();
    ~LKObject();
    void initialize(IplImage* grayFrame, CObject object);
    CObject getNewPosition(IplImage* prevGrayFrame, IplImage* grayFrame, IplImage* frame);
    CObject getObject();
    double getPercentRemaining();
    int getCount();
    void incrementCount();
    void decrementCount();
  private:
    int cornerCount;
    int initialCount;
    CvPoint2D32f *cornersA;
    CvPoint2D32f *cornersB;
 
    IplImage* pyramidA;
    IplImage* pyramidB;
    CObject cobject;

    int count; //number of occurrences
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

