/**
 * MotionTracker
 *
 * Contains all the motion tracking code
 * - Lucas Kanade
 * - Kalman Filter
 */
#include "MotionTracker.h"
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "objects.h"
#include "highgui.h"
#include <iostream>

#define LK_WINDOW_NAME "Lucas Kanade Debug"
#define KF_WINDOW_NAME "Kalman Filter Debug Window"

using namespace std;

MotionTracker::MotionTracker() {}
MotionTracker::~MotionTracker() {}

/**
 * Lucas Kanade
 */
const int MAX_CORNERS = 500;
const int WIN_SIZE = 10;

LucasKanade::LucasKanade() {
  frameCount = 0;
}

LucasKanade::~LucasKanade() {
  cvDestroyWindow(LK_WINDOW_NAME);
}

/**
 * Initialize the Lucas Kanade object.  We need the first frame of the video
 * to know the size for the temporary images we build.
 * Based on sample code of OpenCV book on page 333.
 */
void LucasKanade::initialize(IplImage* frame) {
 
}

CObjectList LucasKanade::getObjectList(IplImage* frame, CObjectList* classifierObjects) {
  CObjectList objectList;

  int cornerCount = MAX_CORNERS;
  if(frameCount == 0) {

    initialize(frame);
    //TODO: Copy this back into initialize
    cvNamedWindow(LK_WINDOW_NAME, CV_WINDOW_AUTOSIZE);

    //Workspace objects
    eigImage = cvCreateImage(cvGetSize(frame), IPL_DEPTH_32F, 1);
    tmpImage = cvCreateImage(cvGetSize(frame), IPL_DEPTH_32F, 1);

    CvSize pyramidSize = cvSize(frame->width+8, frame->height/3);
    pyramidA = cvCreateImage(pyramidSize, IPL_DEPTH_32F, 1);
    pyramidB = cvCreateImage(pyramidSize, IPL_DEPTH_32F, 1);

    //Housekeeping
    IplImage* grayFrame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
    prevGrayFrame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
    cvCvtColor(frame, grayFrame, CV_BGR2GRAY);
    cvCopyImage(grayFrame, prevGrayFrame);
    frameCount++;
     
    cornersA = new CvPoint2D32f[MAX_CORNERS];
    cvGoodFeaturesToTrack(
      grayFrame,
      eigImage,
      tmpImage,
      cornersA,
      &cornerCount,
      0.01,
      0.01,
      NULL);
    cout << "Initially found: " << cornerCount << endl;
    return objectList;
  }

  IplImage* grayFrame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
  cvCvtColor(frame, grayFrame, CV_BGR2GRAY);
  //cout << "Converted gray frame" << endl;

  //TODO: use cvFindCornerSubPix
  //cout << "Found features" << endl;
  cornersB = new CvPoint2D32f[MAX_CORNERS];

  char featuresFound[MAX_CORNERS];
  float featureErrors[MAX_CORNERS];

  //cvShowImage(LK_WINDOW_NAME, prevGrayFrame);
  cvCalcOpticalFlowPyrLK(
    prevGrayFrame,
    grayFrame,
    pyramidA,
    pyramidB,
    cornersA,
    cornersB,
    cornerCount,
    cvSize(WIN_SIZE, WIN_SIZE),
    5,
    featuresFound,
    featureErrors,
    cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.3),
    0
  );
  //cout << "Calculated optical flow" << endl;

  for(int i = 0; i < cornerCount; i++) {
    if(featuresFound[i] == 0 || featureErrors[i] > 550) {
      continue;
    }
    //cout << "Corner " << i << ": " << featuresFound[i] << ";" << featureErrors[i] << endl;
    CvPoint p0 = cvPoint(cvRound(cornersA[i].x), cvRound(cornersA[i].y));
    CvPoint p1 = cvPoint(cvRound(cornersB[i].x), cvRound(cornersB[i].y));
    cvLine(frame, p0, p1, CV_RGB(255,0,0),2);
  }

  //cout << "Created optical flow image" << endl;

  cvShowImage(LK_WINDOW_NAME, frame);
  //cout << "Showed image" << endl;

  //Release previous frame
  //TODO: Determine if we need to release these images to avoid memory leak
  /*
  if(frameCount > 0) {
    cvReleaseImage(&prevGrayFrame);
  }
  cout << "Released old frame" << endl;
  */

  cvCopyImage(grayFrame, prevGrayFrame);
	cornersA = cornersB;
  //cout << "Copied old image" << endl;
  frameCount++;
  return objectList; 
}

/**
 * Kalman Filter
 */
KalmanFilter::KalmanFilter() {
  //cvNamedWindow(KF_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
}

KalmanFilter::~KalmanFilter() {
  //cvDestroyWindow(KF_WINDOW_NAME);
}

void KalmanFilter::update(CObjectList* classifierObjects) {
  cout << "Objects found: " << classifierObjects->size() << endl;

  size_t numFilters = filters.size();
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

