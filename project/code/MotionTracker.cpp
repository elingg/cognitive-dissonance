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

LKObject::LKObject() {

}

LKObject::~LKObject() {

}

/**
 * Utility function to create a boudning box
 */
CvRect createBoundingBox(CvPoint2D32f *corners, int numCorners) {
  int minX = corners[0].x;
  int minY = corners[0].y;
  int maxX = corners[0].x;
  int maxY = corners[0].y;

  for(int i = 0; i < numCorners; i++) {
    if(corners[i].x < minX) {
      minX = corners[i].x;
    }
    if(corners[i].y < minY) {
      minY = corners[i].y;
    }
    if(corners[i].x > maxX) {
      maxX = corners[i].x;
    }
    if(corners[i].y > maxY) {
      maxY = corners[i].y;
    }
  }
  return cvRect(minX, minY, maxX - minX, maxY - minY);
}

/**
 * Initialize the Lucas Kanade object.  We need the first frame of the video
 * to know the size for the temporary images we build.
 * Based on sample code of OpenCV book on page 333.
 */
void LKObject::initialize(IplImage* grayFrame, CObject object) {
    cobject = object; //save a local copy
    
    cornerCount = MAX_CORNERS;
    //Workspace objects
    eigImage = cvCreateImage(cvGetSize(grayFrame), IPL_DEPTH_32F, 1);
    tmpImage = cvCreateImage(cvGetSize(grayFrame), IPL_DEPTH_32F, 1);

    CvSize pyramidSize = cvSize(grayFrame->width+8, grayFrame->height/3);
    pyramidA = cvCreateImage(pyramidSize, IPL_DEPTH_32F, 1);
    pyramidB = cvCreateImage(pyramidSize, IPL_DEPTH_32F, 1);

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

    //Only keep corners found in bounding box
    //TODO: Use image ROI to speed up
    cout << "Object: " << object.rect.x << " " << object.rect.y << " " 
         << object.rect.width << " " << object.rect.height << endl;
    CvPoint2D32f* validCorners = new CvPoint2D32f[MAX_CORNERS];
    int validCornersCount = 0;
    for(int i = 0; i < cornerCount; i++) {
      if((cornersA[i].x >= object.rect.x) &&
         (cornersA[i].x <= object.rect.x + object.rect.width) &&
	 (cornersA[i].y >= object.rect.y) &&
	 (cornersA[i].y <= object.rect.y + object.rect.height)) {
         validCorners[validCornersCount] = cornersA[i];
	 validCornersCount++;
      } else {
         cout << "Did not pass: " << cornersA[i].x << " " << cornersA[i].y << endl;
      }
    }
    cornersA = validCorners;
    cornerCount = validCornersCount;
}

//Remove frame from parameter list
CObject LKObject::getNewPosition(IplImage* prevGrayFrame, IplImage* grayFrame, IplImage* frame) {

  //TODO: use cvFindCornerSubPix
  cornersB = new CvPoint2D32f[MAX_CORNERS];

  char featuresFound[MAX_CORNERS];
  float featureErrors[MAX_CORNERS];

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

  //Debug code
  //TODO: Move this debug code
  float  meanX = 0;
  float  meanY = 0;
  int numGoodCorners = 0;

  //We want to filter out any bad points
  CvPoint2D32f* goodCorners = new CvPoint2D32f[MAX_CORNERS];

  for(int i = 0; i < cornerCount; i++) {
    if(featuresFound[i] != 0 && featureErrors[i] < 550) {
    //if(!featuresFound[i] || featureErrors[i] < 550) {
      CvPoint p0 = cvPoint(cvRound(cornersA[i].x), cvRound(cornersA[i].y));
      CvPoint p1 = cvPoint(cvRound(cornersB[i].x), cvRound(cornersB[i].y));
      cvLine(frame, p0, p1, CV_RGB(255,0,0), 2);

      meanX += cornersB[i].x;
      meanY += cornersB[i].y;

      goodCorners[numGoodCorners] = cornersB[i];

      numGoodCorners++;
    } else {
      //cout << "Found a bad point" << endl;
      //cout << "Error is " << featureErrors[i] << endl;
    }
  }
  meanX = meanX / numGoodCorners;
  meanY = meanY / numGoodCorners;
  //cout << "Number of valid points " << numGoodCorners;
  CvPoint meanPoint = cvPoint((int) meanX, (int) meanY);
  cvCircle(frame, meanPoint, 5, CV_RGB(0, 0, 255));

  //cvShowImage(LK_WINDOW_NAME, frame);
  
  //Important, and this will probably lead to bugs if you forget about it:
  //swap the corners
  cornersA = goodCorners;
  cornerCount = numGoodCorners;
  
  cobject.rect = createBoundingBox(cornersA, cornerCount);
  return cobject;
}

/**
 * Lucas Kanade - holds a set of blobs
 */
LucasKanade::LucasKanade() {
  frameCount = 0;
  //cvNamedWindow(LK_WINDOW_NAME, CV_WINDOW_AUTOSIZE);

}

LucasKanade::~LucasKanade() {
  //cvDestroyWindow(LK_WINDOW_NAME);
}

CObjectList LucasKanade::process(IplImage* frame, CObjectList* classifierObjects) {
  CObjectList objectList;

  //Housekeeping
  IplImage* grayFrame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
  cvCvtColor(frame, grayFrame, CV_BGR2GRAY);

  if((frameCount == 0) && (classifierObjects->size() > 0)) {
  //if(frameCount == 0) {
    cout << "Classifier object size: " << classifierObjects->size() << endl;
    LKObject lkObject;
    CObject object = classifierObjects->at(0);
    lkObject.initialize(grayFrame, object);
    blobs.push_back(lkObject);
  }

  if(frameCount > 0) { //Need to have at least one previousFrame
    for(size_t i = 0; i < blobs.size(); i++) {
      CObject obj = blobs[i].getNewPosition(prevGrayFrame, grayFrame, frame);
      objectList.push_back(obj);
    }
  }
  prevGrayFrame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
  cvCopyImage(grayFrame, prevGrayFrame);

  //Release previous frame
  //TODO: Determine if we need to release these images to avoid memory leak
  /*
  if(frameCount > 0) {
    cvReleaseImage(&prevGrayFrame);
  }
  cout << "Released old frame" << endl;
  */

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

//TODO: Merge update and predict functions after experimenting with different ordering
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

