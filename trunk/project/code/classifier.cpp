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
CClassifier::CClassifier(CommandOptions& opts, bool video_mode)
:ourOptions(opts),videoMode(video_mode)
{
    // initalize the random number generator (for sample code)
    rng = cvRNG(-1);
    parameters = NULL;
    // CS221 TO DO: add initialization for any member variables   
    if(videoMode) {
      ourOptions.needUintOption("framestoskip", "number of frames to skip", 0); 
      ourOptions.needDoubleOption("overlap_ratio", "overlap ratio for coalescing", 0.05); 
    }
    ourOptions.needBoolOption("corner_feature", "use corner detection as feature", false); 
    ourOptions.needBoolOption("circle_feature", "use circles as feature", false); 
    ourOptions.needBoolOption("edge_feature", "use edges as feature", false); 
    ourOptions.needBoolOption("sobel_feature", "use sobel as feature", true); 
    ourOptions.needUintOption("depth", "depth of tree to use", 2); 
    ourOptions.needUintOption("trees", "number of trees in boosting", 1000); 
    ourOptions.needBoolOption("homegrownboost", "use homegrown boosting classifier",false); 

    classifier = 0; 
    frameCount = 0;
}
    
// destructor
CClassifier::~CClassifier()
{
    if (parameters != NULL) {
      cvReleaseMat(&parameters);
    }
    // CS221 TO DO: free any memory allocated by the object
    assert(classifier); // did not call postCommandParseNotify
    delete classifier;
}

void CClassifier::postCommandParseNotify(bool dontprint) {
  assert(classifier==0);
  vector<Label> classes;
  vector<string> labels;
  getAllLabelStrings(labels);
  for(size_t il=0;il<labels.size(); il++) {
    classes.push_back(getLabel(labels[il]));
  }
  classifier = new MulticlassClassifier(classes, 
                                        ourOptions.getUintOption("trees"),
                                        ourOptions.getUintOption("depth"),
                                        ourOptions.getBoolOption("homegrownboost"),
                                        ourOptions.getVerboseFlag());
  if(dontprint) return;
  if(videoMode) {
    cout << "Classifier used only for every " << ourOptions.getUintOption("framestoskip") <<  " frames" << endl;
    cout << "Coalescing if rectangle overlap ratio exceeds " << ourOptions.getDoubleOption("overlap_ratio") << endl;
  }
  if(ourOptions.getBoolOption("circle_feature")) {
    cout << "Using circle detection features" << endl;
  }
  if(ourOptions.getBoolOption("edge_feature")) {
    cout << "Using edge detection features" << endl;
  }
  if(ourOptions.getBoolOption("sobel_feature")) {
    cout << "Using sobel detection features" << endl;
  }
  if(ourOptions.getBoolOption("corner_feature")) {
    cout << "Using corner detection features" << endl;
  }
  if(ourOptions.getBoolOption("homegrownboost")) {
    cout << "Using *HOMEGROWN* boosting classifier" << endl;
  } else {
    cout << "Using CvBoost classifier" << endl;
  }
  cout << "Using " << ourOptions.getUintOption("trees") << " trees in boosting" << endl;
  cout << "Using " << ourOptions.getUintOption("depth") << " depth in boosting" << endl;
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

void coreExtractFeatures(IplImage* image,
                         vector<double>& values, 
                         const CommandOptions& opts) {
  HaarFeatures haar(opts.getVerboseFlag());
  haar.getFeatureValues(values,image);
  if(opts.getBoolOption("circle_feature")) {
    Hough hough;
    hough.getCircles(values,image);
  }
  if(opts.getBoolOption("edge_feature")) {
    Hough hough;
    hough.getEdges(values,image);
  }
  if(opts.getBoolOption("corner_feature")) {
    Hough hough;
    hough.getCorners(values,image); 
  }
  if(opts.getBoolOption("sobel_feature")) {
    EdgeDetectionFeatures sobel;
    sobel.getFeatureValues(values,image);
  }
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
void coalesceOverlappingRectangles(CObjectList* firstobjs, CObjectList* resobjs,
                                   double overlap_ratio_cutoff) {
  // cerr << "Overlap ratio cutoff: " << overlap_ratio_cutoff << endl;
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
          if(overlap_ratio < overlap_ratio_cutoff) continue;
          // cout << "Overlap ratio: " << overlap_ratio << endl;
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
  assert(videoMode);
  size_t frames_to_skip = ourOptions.getUintOption("framestoskip");   
  assert((frame != NULL) && (objects != NULL));
  
  //In order to speed up running time, skip a few frames at a time
  //and let Lucas kanade handle the interpolate the other points
  if(frameCount % frames_to_skip == 0) {
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
          coreExtractFeatures(smallImage, values, ourOptions);

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
    double overlap_ratio = ourOptions.getDoubleOption("overlap_ratio"); 
    if(verbose) cerr << "Coalescing " << firstpassobjects.size() << " rectangles"; 
    coalesceOverlappingRectangles(&firstpassobjects,objects,overlap_ratio);
    if(verbose) cerr << " into " << objects->size() << endl;
    cvReleaseImage(&gray);
    lucasKanade.seed(frame, objects);
  } else {
    lucasKanade.interpolate(frame, objects); 
  }

  frameCount++;
  return true;
}

     
bool extractFeatures(TTrainingFileList& fileList, 
                     vector<TrainingExample*>& trainingSet,  
                     const CommandOptions& opts) {
  Timer t("extracting features from images",opts.getVerboseFlag());
  HaarFeatures haar(opts.getVerboseFlag());
  EdgeDetectionFeatures sobel;
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
      assert(smallImage->nChannels==1); // assert its grayscale
      // CS221 TO DO: extract features from image here
      vector<double> values;
      coreExtractFeatures(smallImage, values, opts);

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
    if(ourOptions.getVerboseFlag()) cout << "Classes:" << endl;
    for (int i = 0; i < (int)fileList.classes.size(); i++) {
      if(ourOptions.getVerboseFlag()) cout << fileList.classes[i] << " (";
      int count = 0;
      for (int j = 0; j < (int)fileList.files.size(); j++) {
        if (fileList.files[j].label == fileList.classes[i]) {
          count += 1;
	      }
	    }
      if(ourOptions.getVerboseFlag()) cout << count << " samples)" << endl;
    }
    if(ourOptions.getVerboseFlag()) cout << endl;

    // example code for loading and resizing image files--
    // you may find this useful for the milestone    
    
    if(ourOptions.getVerboseFlag()) cout << "Processing images..." << endl;
    vector<TrainingExample*> trainingSet;
    if(!extractFeatures(fileList, trainingSet, ourOptions)) {
      for(size_t it=0;it<trainingSet.size(); it++) {
        delete trainingSet[it];
      }
      exit(-1);
    }
    if(ourOptions.getVerboseFlag()) cout << endl;
    // CS221 TO DO: train you classifier here
    if(ourOptions.getVerboseFlag()) cerr << "Training tree:" << endl;
    classifier->train(trainingSet);
    return true;
}

double CClassifier::test(TTrainingFileList& fileList, Stats& stat) {
    if(ourOptions.getVerboseFlag()) cerr << "Testing tree on trained images: " << endl;
    Timer t("testing on images",ourOptions.getVerboseFlag());
    if(ourOptions.getVerboseFlag()) cout << "Processing images..." << endl;
    vector<TrainingExample*> testSet;
    if(!extractFeatures(fileList, testSet, ourOptions)) {
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
