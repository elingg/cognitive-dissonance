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
}

CObjectList LucasKanade::getObjectList(IplImage* frame, CObjectList* classifierObjects) {
  CObjectList objectList;

  if(frameCount == 0) {
    initialize(frame);
    return objectList;
  }

  IplImage* grayFrame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
  cvCvtColor(frame, grayFrame, CV_BGR2GRAY);
  //cout << "Converted gray frame" << endl;

  int cornerCount = MAX_CORNERS;
  CvPoint2D32f* cornersA = new CvPoint2D32f[MAX_CORNERS];
  CvPoint2D32f* cornersB = new CvPoint2D32f[MAX_CORNERS];
  cvGoodFeaturesToTrack(
    grayFrame,
    eigImage,
    tmpImage,
    cornersA,
    &cornerCount,
    0.01,
    0.01,
    NULL);
  //TODO: use cvFindCornerSubPix
  //cout << "Found features" << endl;

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
    CvPoint p0 = cvPoint(cvRound(cornersA[i].x), cvRound(cornersA[i].y));
    CvPoint p1 = cvPoint(cvRound(cornersB[i].x), cvRound(cornersB[i].y));
    cvLine(grayFrame, p0, p1, CV_RGB(255,0,0),2);
  }

  //cout << "Created optical flow image" << endl;

  cvShowImage(LK_WINDOW_NAME, grayFrame);
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
  //cout << "Copied old image" << endl;
  frameCount++;
  return objectList; 
}

/**
 * Kalman Filter
 */
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
