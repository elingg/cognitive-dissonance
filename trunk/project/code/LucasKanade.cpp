#include "MotionTracker.h"
#include "cv.h"
#include "cxcore.h"
#include "cvaux.h"
#include "objects.h"
#include "highgui.h"
#include <iostream>

#define LK_DEBUG_WINDOW "LK Debug Window"

using namespace std;

/**
 * Lucas Kanade
 */
const int MAX_CORNERS = 500;
const int WIN_SIZE = 10;

LKObject::LKObject() {
  count = 0;
}

LKObject::~LKObject() {
  //TODO: Figure out why this causes a problem
  /*
  cvReleaseImage(&pyramidA);
  cvReleaseImage(&pyramidB);
  */
}

int LKObject::getCount() {
  return count;
}

void LKObject::incrementCount() {
  count++;
}

void LKObject::decrementCount() {
  count--;
}

/**
 * Utility function to create a bounding box when given a list of corners
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
    IplImage* eigImage = cvCreateImage(cvGetSize(grayFrame), IPL_DEPTH_32F, 1);
    IplImage* tmpImage = cvCreateImage(cvGetSize(grayFrame), IPL_DEPTH_32F, 1);

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
    //cout << "Initially found: " << cornerCount << endl;

    cvReleaseImage(&eigImage);
    cvReleaseImage(&tmpImage);

    //TODO: use cvFindCornerSubPix
    
    //Only keep corners found in bounding box
    //TODO: Use image ROI to speed up
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
         //cout << "Did not pass: " << cornersA[i].x << " " << cornersA[i].y << endl;
      }
    }
    cornersA = validCorners;
    cornerCount = validCornersCount;

    initialCount = validCornersCount;
}

double LKObject::getPercentRemaining() {
  return (double) cornerCount / (double) initialCount;
}

CObject LKObject::getObject() {
  return cobject;
}

CObject LKObject::getNewPosition(IplImage* prevGrayFrame, IplImage* grayFrame, 
                                 IplImage* debugFrame) {
  //TODO:Remove frame from parameter list
  cornersB = new CvPoint2D32f[MAX_CORNERS];

  char featuresFound[MAX_CORNERS];
  float featureErrors[MAX_CORNERS];

  //cout << "Calculated optical flow" << endl;
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
  //cout << "Finished optical flow " << endl;

  //We want to filter out any bad points
  CvPoint2D32f* goodCorners = new CvPoint2D32f[MAX_CORNERS];
  int numGoodCorners = 0;

  for(int i = 0; i < cornerCount; i++) {
    int CORNER_ERROR_THRESHOLD = 200;  //controls how far away corners can move
    if(featuresFound[i] != 0 && featureErrors[i] < CORNER_ERROR_THRESHOLD) {
      CvPoint p0 = cvPoint(cvRound(cornersA[i].x), cvRound(cornersA[i].y));
      CvPoint p1 = cvPoint(cvRound(cornersB[i].x), cvRound(cornersB[i].y));
      cvLine(debugFrame, p0, p1, CV_RGB(255,0,0), 2);
      cvShowImage(LK_DEBUG_WINDOW, debugFrame);

      goodCorners[numGoodCorners] = cornersB[i];
      numGoodCorners++;
    } else {
      //cout << "Error is " << featureErrors[i] << endl;
    }
  }
  //cout << "Number of valid points " << numGoodCorners;
  
  //Very important part: update the corners to include only the good corners
  cornersA = goodCorners;
  cornerCount = numGoodCorners;
  
  cobject.rect = createBoundingBox(cornersA, cornerCount);
  return cobject;
}

/****************************************************************************
 * The LucasKanade class holds a set of classifiers
 ****************************************************************************/

/**
 * Lucas Kanade Constructor
 */
LucasKanade::LucasKanade() {
  frameCount = 0;
  cvNamedWindow(LK_DEBUG_WINDOW, CV_WINDOW_AUTOSIZE);
}

/**
 * Lucas Kanade destructor
 */
LucasKanade::~LucasKanade() {
  cvDestroyWindow(LK_DEBUG_WINDOW);
}

/**
 * This function should be called to "seed" the Lucas Kanade filter with a 
 * CObjectList of objects returned by the classifier
 */
void LucasKanade::seed(const IplImage* frame, CObjectList* classifierObjects) {
  //Delete any existing blobs
  blobs.clear();
  
  //Housekeeping
  IplImage* grayFrame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
  cvCvtColor(frame, grayFrame, CV_BGR2GRAY);

  for(size_t i = 0; i < classifierObjects->size(); i++) {
    LKObject lkObject;
    lkObject.initialize(grayFrame, classifierObjects->at(i));
    blobs.push_back(lkObject);
  }

  if(frameCount == 0) {
    prevGrayFrame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
  }
  cvCopyImage(grayFrame, prevGrayFrame);
  cvReleaseImage(&grayFrame);

  frameCount++;
}

/**
 * This function should be used to interpolate between frames.  It will
 * fill in the CObjectList objects
 */
void LucasKanade::interpolate(const IplImage* frame, CObjectList* objects) {
  objects->clear();
  if(frameCount > 0) {
 
    //Housekeeping
    IplImage* grayFrame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
    cvCvtColor(frame, grayFrame, CV_BGR2GRAY);
    
    IplImage* debugFrame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
    cvCopyImage(grayFrame, debugFrame);

    for(size_t i = 0; i < blobs.size(); i++) {
      //TODO: last parameter is the debug frame.  It should be frame instead
      //of grayFrame, but frame is const
      CObject obj = blobs[i].getNewPosition(prevGrayFrame, grayFrame, debugFrame);
      objects->push_back(obj);
    }

    cvCopyImage(grayFrame, prevGrayFrame);
    cvReleaseImage(&grayFrame);
  }
  frameCount++;
}


/**
 * Process a frame.
 */
CObjectList LucasKanade::process(IplImage* frame, CObjectList* classifierObjects) {
  CObjectList objectList;

  size_t numBlobs = blobs.size();
  bool shouldPenalize[numBlobs];
  for(size_t i = 0; i < numBlobs; i++) {
    shouldPenalize[i] = true;
  }

  //Housekeeping
  IplImage* grayFrame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
  cvCvtColor(frame, grayFrame, CV_BGR2GRAY);

  for(size_t i = 0; i < classifierObjects->size(); i++) {
    bool foundMatchingBlob = false;
    for(size_t j = 0; j < blobs.size(); j++) {
      CObject obj = blobs[j].getObject();
      if(obj.label == classifierObjects->at(i).label) {
        //check overlap between filter and classifier object
        int overlap = obj.overlap(classifierObjects->at(i));
        double overlapRatio = (double) (2*overlap) /
                              (obj.area() + classifierObjects->at(i).area()); 
        //if(overlapRatio >= 0.8) {
        if(overlapRatio >= 0.2) {
          foundMatchingBlob = true;
	  blobs[j].incrementCount();
	  shouldPenalize[j] = false;
	  break;
        }
      }
    }
    if(!foundMatchingBlob) {
      LKObject lkObject;
      lkObject.initialize(grayFrame, classifierObjects->at(i));
      blobs.push_back(lkObject);
    }
  }

  for(size_t i = 0; i < numBlobs; i++) {
    if(shouldPenalize[i]) {
      blobs[i].decrementCount();
    }
  }

  if(frameCount > 0) { //Need to have at least one previousFrame
    for(size_t i = 0; i < blobs.size(); i++) {
      //cout << "Looking at blob " << i << endl;
      CObject obj = blobs[i].getNewPosition(prevGrayFrame, grayFrame, frame);
      //cout << "Finished getting position of blob " << i << endl;
      //cout << "Percent corners remaining " << blobs[i].getPercentRemaining() << endl; 
      double CORNER_PERCENT_REMAINING_THRESHOLD = 0.1;
      if(blobs[i].getPercentRemaining() > CORNER_PERCENT_REMAINING_THRESHOLD) {
        objectList.push_back(obj);
      } else {
        blobs.erase(blobs.begin() + i);
      }

      if(blobs[i].getCount() < -3) {
        blobs.erase(blobs.begin() + i);
      }

      //cout << "Finishd with blob " << i << endl;
    }
  }

  if(frameCount == 0) {
    prevGrayFrame = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
  }
  cvCopyImage(grayFrame, prevGrayFrame);
  cvReleaseImage(&grayFrame);

  frameCount++;
  return objectList; 
}
