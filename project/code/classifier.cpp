/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    classifier.cpp
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  See classifier.h
**
*****************************************************************************/

#include <cassert>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "classifier.h"
#include "HaarFeatures.h"
#include "EdgeDetectionFeatures.h"
#include "MulticlassClassifier.h"
#include "Label.h"
#include "Timer.h"

#include "Hough.h"
using namespace std;

// CClassifier class ---------------------------------------------------------

// default constructor
CClassifier::CClassifier(bool verbose_flag, 
                         size_t numtrees, size_t depth,
                         bool homegrown):verbose(verbose_flag)
{
    // initalize the random number generator (for sample code)
    rng = cvRNG(-1);
    parameters = NULL;
    // CS221 TO DO: add initialization for any member variables   
    vector<Label> classes;
    vector<string> labels;
    getAllLabelStrings(labels);
    for(size_t il=0;il<labels.size(); il++) {
      classes.push_back(getLabel(labels[il]));
    }
 
    classifier = new MulticlassClassifier(classes, numtrees, 
                                          depth, homegrown, verbose);
}
    
// destructor
CClassifier::~CClassifier()
{
    if (parameters != NULL) {
	cvReleaseMat(&parameters);
    }

    // CS221 TO DO: free any memory allocated by the object
    delete classifier;
}

// loadState
// Configure the classifier from the given file.
bool CClassifier::loadState(const char *filename)
{
    assert(filename != NULL);
    classifier->loadState(filename);
    // CS221 TO DO: replace this with your own configuration code
    
    return true;
}

// saveState
// Writes classifier configuration to the given file
bool CClassifier::saveState(const char *filename)
{
    assert(filename != NULL);
    
    // CS221 TO DO: replace this with your own configuration code
    classifier->saveState(filename); 
    if(verbose) cerr << "Finished saving state" << endl;
    return true;
}

void printCObject(const CObject& obj) {
   cout << "[label:"<<obj.label << 
             ", x:" << obj.rect.x <<
             ", y:"<<obj.rect.y <<
             ", h:" <<obj.rect.height <<
             ", w:" <<obj.rect.width << "]";
}
 
// coalesces objects with the same labels into single rectangles if they
// overlap
void coalesceOverlappingRectangles(CObjectList* firstobjs, CObjectList* resobjs) {
  multimap<string, CObject*> tmp;
  set<string> labels;
  for(size_t iobj=0; iobj<firstobjs->size(); iobj++) {
    CObject& obj = (*firstobjs)[iobj]; 
    tmp.insert(make_pair(obj.label,&obj));
    labels.insert(obj.label);
  }
  //cout << "Merging overlapping rectangles: " << endl;
  // for each label...
  for(set<string>::iterator itl=labels.begin(); itl!=labels.end(); itl++) {
    const string& label = *itl;
    //cout << "Processing label: " << label << endl;
    vector<CObject> currbuff;
    multimap<string, CObject*>::iterator uit = tmp.upper_bound(label);
    multimap<string, CObject*>::iterator lit = tmp.lower_bound(label);
    multimap<string, CObject*>::iterator it;
    for(it=lit; it!=uit; it++) {
      currbuff.push_back(*(it->second));
    }
    bool merge_exists;
    do {
      // cout << "Attempting merge of " << currbuff.size() << " objects..." <<endl;
      //for(size_t iobj=0; iobj<currbuff.size(); iobj++) {
        //cout << "Object: " << iobj << endl;
        //printCObject(currbuff[iobj]); cout << endl;
      //}
      merge_exists = false;
      vector<CObject> buff = currbuff;
      currbuff.clear();
      set<size_t> merged;
      for(size_t iobj1=0;iobj1<buff.size(); iobj1++) {
        for(size_t iobj2=iobj1; iobj2<buff.size(); iobj2++) {
          if(iobj1==iobj2) continue;
          int overlap = buff[iobj1].overlap(buff[iobj2]);
          double overlap_ratio = (double)(2*overlap)/
                                   (buff[iobj1].area()+buff[iobj2].area());
          if(overlap_ratio < 0.05) continue;
          //cout << "Overlap ratio: " << overlap_ratio << endl;
          merged.insert(iobj1); merged.insert(iobj2);
          merge_exists = true; 
          CvRect sup = cvMaxRect(&(buff[iobj1].rect),&(buff[iobj2].rect));
          CObject newobj(sup,label);
          //cout << "Merged: " << endl; 
          //printCObject(buff[iobj1]); cout << " and " << endl;
          //printCObject(buff[iobj2]); cout << " into " << endl;
          //printCObject(newobj); cout << endl;
          bool alreadyadded = false;
          for(size_t ic=0; ic<currbuff.size(); ic++) {
            if((currbuff[ic].rect.x==sup.x)&&
               (currbuff[ic].rect.y==sup.y)&&
               (currbuff[ic].rect.width=sup.width)&&
               (currbuff[ic].rect.height==sup.height)) {
              alreadyadded = true; 
              break;
            }
          } 
          if(!alreadyadded) {
            currbuff.push_back(newobj);
          }
        } 
        if(merged.find(iobj1)==merged.end()) { 
          currbuff.push_back(buff[iobj1]);
        }
      }      
      // keep coalescing if you find merges...
    } while(merge_exists);
    for(size_t iobj=0; iobj<currbuff.size(); iobj++) {
      resobjs->push_back(currbuff[iobj]);
    }
  }
}

// run
// Runs the classifier over the given frame and returns a list of
// objects found (and their location).
bool CClassifier::run(const IplImage *frame, CObjectList *objects) {
    
    assert((frame != NULL) && (objects != NULL));

    //Convert to gray scale (from OpenCV lecture notes)
    IplImage *gray = cvCreateImage(
      cvGetSize(frame),
      IPL_DEPTH_8U,
      1);
    cvCvtColor(frame, gray, CV_BGR2GRAY);
    CObjectList firstpassobjects;
    //Use a sliding window,
    int const MIN_LENGTH_SIZE = 64;
    int const MAX_LENGTH_SIZE = 208; // 104
    for(int length = MIN_LENGTH_SIZE; length <= MAX_LENGTH_SIZE; length += 8) {
      for(int x = 0; x < gray->width - length; x = x + 8) {
        for(int y = 0; y < gray->height - length; y = y + 8) {
//          cerr << "New window: " << x << " " << y << " " << length << endl;
          //Clip the frame to a square
          CvRect region = cvRect(x, y, length, length);
          IplImage *clippedImage = cvCreateImage(
          cvSize(region.width, region.height),
          gray->depth, gray->nChannels);
          cvSetImageROI(gray, region);
          cvCopyImage(gray, clippedImage);
          cvResetImageROI(gray);

          //Resize clipped image to 64 x 64
          IplImage *smallImage = cvCreateImage(cvSize(64, 64), IPL_DEPTH_8U, 1);
          cvResize(clippedImage, smallImage);

          //Get Haar feature values
          vector<double> values;
          HaarFeatures haar(verbose);
          haar.getFeatureValues(values,smallImage);
	  Hough hough;
	  //hough.getCircles(values,smallImage);
	  hough.getEdges(values,smallImage);
          //EdgeDetectionFeatures sobel;
	  //sobel.getFeatureValues(values,smallImage);

          //Check for image
          ImagesExample imagesExample(values); 
          Label label = classifier->predict(imagesExample);
          // cerr << "label: " << getLabelString(label) << endl;
          //Create bounding box
          CObject obj;
          obj.rect = cvRect(0, 0, length, length);
          obj.rect.x = x;
          obj.rect.y = y;
          obj.label = getLabelString(label);
          if(!isLabelEqual(label, getLabel("other"))) 
            firstpassobjects.push_back(obj);
	        
          //Release images
          cvReleaseImage(&clippedImage);
          cvReleaseImage(&smallImage);
        }
      }
    }
    if(verbose) cerr << "coalescing " << firstpassobjects.size() << endl;
    coalesceOverlappingRectangles(&firstpassobjects,objects);

    cvReleaseImage(&gray);
    // cout << "Number of results found: " << objects->size() << endl;
    // cout << objects->size() << "\t";
    // Example code which returns up to 10 random objects, each object
    // having a width and height equal to half the frame size.
    /*
    const char *labels[5] = {
	"mug", "stapler", "keyboard", "clock", "scissors"
    }; 
    int n = cvRandInt(&rng) % 10;
    while (n-- > 0) {
        CObject obj;
        obj.rect = cvRect(0, 0, frame->width / 2, frame->height / 2);
        obj.rect.x = cvRandInt(&rng) % (frame->width - obj.rect.width);
        obj.rect.y = cvRandInt(&rng) % (frame->height - obj.rect.height);
	obj.label = string(labels[cvRandInt(&rng) % 5]);
	objects->push_back(obj);        
    }
    */

    return true;
}
      
bool extractFeatures(TTrainingFileList& fileList, 
                     vector<TrainingExample*>& trainingSet, bool verbose) {
  Timer t("extracting features from images",verbose);
  HaarFeatures haar(verbose);
  //EdgeDetectionFeatures sobel;
  Hough hough;
  IplImage *image, *smallImage;
  smallImage = cvCreateImage(cvSize(64, 64), IPL_DEPTH_8U, 1);
  vector<string> labels; 
  getAllLabelStrings(labels);
  for (int i = 0; i < (int)fileList.files.size(); i++) {
    // show progress
    if (i % 1000 == 0) {
      showProgress(i, fileList.files.size());
    }
    if (find(labels.begin(),labels.end(),fileList.files[i].label)!=labels.end()) {
      // load the image
      image = cvLoadImage(fileList.files[i].filename.c_str(), 0);
      if (image == NULL) {
         cerr << "ERROR: could not load image "
              << fileList.files[i].filename.c_str() << endl;
         continue;
      }

      // resize to 64 x 64
      cvResize(image, smallImage);

      // CS221 TO DO: extract features from image here
      vector<double> values;
      haar.getFeatureValues(values,smallImage);
      //hough.getCircles(values,smallImage);
      hough.getEdges(values,smallImage);
      //sobel.getFeatureValues(values,smallImage);

      trainingSet.push_back(new ImagesExample(values, 
                                    getLabel(fileList.files[i].label)));
      // free memory
      cvReleaseImage(&image);
    }
  }
  // free memory
  cvReleaseImage(&smallImage);
  return fileList.files.size()==trainingSet.size();
}
  
// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool CClassifier::train(TTrainingFileList& fileList) {
    // CS221 TO DO: replace with your own training code
    // example code to show you number of samples for each object class
    if(verbose) cout << "Classes:" << endl;
    for (int i = 0; i < (int)fileList.classes.size(); i++) {
      if(verbose) cout << fileList.classes[i] << " (";
      int count = 0;
      for (int j = 0; j < (int)fileList.files.size(); j++) {
        if (fileList.files[j].label == fileList.classes[i]) {
          count += 1;
	      }
	    }
      if(verbose) cout << count << " samples)" << endl;
    }
    if(verbose) cout << endl;

    // example code for loading and resizing image files--
    // you may find this useful for the milestone    
    
    if(verbose) cout << "Processing images..." << endl;
    vector<TrainingExample*> trainingSet;
    if(!extractFeatures(fileList, trainingSet, verbose)) {
      for(size_t it=0;it<trainingSet.size(); it++) {
        delete trainingSet[it];
      }
      exit(-1);
    }
    if(verbose) cout << endl;
    // CS221 TO DO: train you classifier here
    if(verbose) cerr << "Training tree:" << endl;
    classifier->train(trainingSet);
    return true;
}

double CClassifier::test(TTrainingFileList& fileList, Stats& stat) {
    if(verbose) cerr << "Testing tree on trained images: " << endl;
    Timer t("testing on images",verbose);
    if(verbose) cout << "Processing images..." << endl;
    vector<TrainingExample*> testSet;
    if(!extractFeatures(fileList, testSet, verbose)) {
      for(size_t it=0;it<testSet.size(); it++) {
        delete testSet[it];
      }
      exit(-1);
    }
    size_t incorrect = 0;
    for(size_t i=0; i< testSet.size(); ++i) {
      Label predictedLabel = classifier->predict(*testSet[i]);
      stat.log_prediction_result(testSet[i]->getLabel(),predictedLabel);
      if(!isLabelEqual(testSet[i]->getLabel(),predictedLabel)) {
        incorrect++;
      }     
    }
    // cerr << "Incorrectly predicted " << incorrect << " out of " << testSet.size()<< endl;
    return (double)(incorrect)/(double)(testSet.size());
}
