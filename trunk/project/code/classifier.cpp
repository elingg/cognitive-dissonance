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
#include <iostream>
#include <fstream>
#include <algorithm>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "classifier.h"
#include "HaarFeatures.h"
#include "DecisionTree.h"
using namespace std;

// CClassifier class ---------------------------------------------------------

// default constructor
CClassifier::CClassifier()
{
    // initalize the random number generator (for sample code)
    rng = cvRNG(-1);
    parameters = NULL;

    // CS221 TO DO: add initialization for any member variables   
}
    
// destructor
CClassifier::~CClassifier()
{
    if (parameters != NULL) {
	cvReleaseMat(&parameters);
    }

    // CS221 TO DO: free any memory allocated by the object
}

// loadState
// Configure the classifier from the given file.
bool CClassifier::loadState(const char *filename)
{
    assert(filename != NULL);
    decisionTree.loadState(filename);
    // CS221 TO DO: replace this with your own configuration code
    
    return true;
}

// saveState
// Writes classifier configuration to the given file
bool CClassifier::saveState(const char *filename)
{
    assert(filename != NULL);
    
    // CS221 TO DO: replace this with your own configuration code
    decisionTree.saveState(filename); 
    cerr << "Finished saving state";
    return true;
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

    //Use a sliding window,
    int const MIN_LENGTH_SIZE = 64;
    int const MAX_LENGTH_SIZE = 104;
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
          HaarFeatures haar;
          haar.getFeatureValues(values,smallImage);

          //Check for image
          ImagesExample imagesExample(values, true); 
          bool isFound = decisionTree.predictClassLabel(imagesExample);

          //Create bounding box
	  if(isFound) {
            CObject obj;
            obj.rect = cvRect(0, 0, length, length);
	    obj.rect.x = x;
	    obj.rect.y = y;
	    obj.label = string("mug");
	    objects->push_back(obj);
	  }
	  
	  //Release images
	  cvReleaseImage(&clippedImage);
	  cvReleaseImage(&smallImage);
	}
      }
    }

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
        
// train
// Trains the classifier to recognize the objects given in the
// training file list.
bool CClassifier::train(TTrainingFileList& fileList) {
    // CS221 TO DO: replace with your own training code
    vector<string> feature_names;

    string temp;
    for(int i=0; i<57; i++) {
      std::string s;
      std::stringstream out;
      out << i;
      s = out.str();
      temp = "h"+ s;
      feature_names.push_back(temp);
    }
    decisionTree.initialize(feature_names);

    // example code to show you number of samples for each object class
    cout << "Classes:" << endl;
    for (int i = 0; i < (int)fileList.classes.size(); i++) {
	cout << fileList.classes[i] << " (";
	int count = 0;
	for (int j = 0; j < (int)fileList.files.size(); j++) {
	    if (fileList.files[j].label == fileList.classes[i]) {
		count += 1;
	    }
	}
	cout << count << " samples)" << endl;
    }
    cout << endl;

    // example code for loading and resizing image files--
    // you may find this useful for the milestone    
    HaarFeatures haar;
    IplImage *image, *smallImage;
    
    vector<TrainingExample*> trainingSet;

    cout << "Processing images..." << endl;
    smallImage = cvCreateImage(cvSize(64, 64), IPL_DEPTH_8U, 1);
    for (int i = 0; i < (int)fileList.files.size(); i++) {
	// show progress
	if (i % 1000 == 0) {
	    showProgress(i, fileList.files.size());
	}

	// skip non-mug and non-other images (milestone only)
	if ((fileList.files[i].label == "mug") ||
	    (fileList.files[i].label == "other")) {
	    
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

	    if(fileList.files[i].label == "mug") {
  	      trainingSet.push_back(new ImagesExample(values, true));
	    } if(fileList.files[i].label == "other") {
              trainingSet.push_back(new ImagesExample(values, false));
	    } else {
              assert("Encountered bad label while training");
	    }
	    // free memory
	    cvReleaseImage(&image);
	}
    }

    // free memory
    cvReleaseImage(&smallImage);
    cout << endl;

    // CS221 TO DO: train you classifier here
    cerr << "Training tree:" << endl;
    decisionTree.trainTree(trainingSet);
    
    cerr << "Testing tree on trained images: " << endl;
    size_t correct = trainingSet.size();
    for(size_t i=0; i< trainingSet.size(); ++i) {
      bool predicted = decisionTree.predictClassLabel(*trainingSet[i]);
      if(predicted!=trainingSet[i]->getClassLabel()) {
        correct--;
      }     
    }
    cerr << "Correctly predicted " << correct << " out of " << trainingSet.size()<< endl;
    return true;
}
