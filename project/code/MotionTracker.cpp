/**
 * MotionTracker - entry point for motion tracking algorithms:
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

using namespace std;

bool USE_KALMAN = false;
bool USE_LK = false;
bool USE_MOTION_TRACKING_RESULTS = false;

MotionTracker::MotionTracker() {}
MotionTracker::~MotionTracker() {}

CObjectList MotionTracker::process(IplImage* frame, CObjectList classifierObjects) {
  CObjectList debugObjects;

  if(USE_KALMAN) {
    kalmanFilter.update(&classifierObjects); 
    debugObjects = kalmanFilter.predict();
    cout << "Kalman objects: " << debugObjects.size();
  }

  if(USE_LK) {
    debugObjects = lucasKanade.process(frame, &classifierObjects); 
  }

  CvFont font;
  cvInitFont(&font, CV_FONT_VECTOR0, 0.75, 0.75, 0.0f, 1, CV_AA);
  CObjectList::iterator iObj;
  for (iObj = debugObjects.begin(); iObj != debugObjects.end(); iObj++) {
    iObj->draw(frame, CV_RGB(0, 0, 255), &font);
  }

  if(USE_MOTION_TRACKING_RESULTS) {
    return debugObjects;
  } else {
    return classifierObjects;
  }
}
