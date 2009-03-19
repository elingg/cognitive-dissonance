/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    classifier.h
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  The CClassifier class runs all the object classifiers. The loadState()
**  member function is called during initialization with the filename of the
**  configuration file. The run() member function is then called on each video
**  frame returning a list of objects found.
**  During training, the train() method is called with a list of files on
**  which to train the classifier. The saveState() function is called once
**  training has finished to save the new configuration.
**
** CS221 TO DO:
**  Implement the loadState(), saveState(), run(), and train() member
**  functions.
*****************************************************************************/

#pragma once

#include "cv.h"
#include "cxcore.h"

#include "utils.h"
#include "objects.h"
#include "MotionTracker.h"

#include "ImagesExample.h"
#include "Stats.h"
#include "CommandOptions.h"

class AbstractMulticlassClassifier;

/* CClassifier class ---------------------------------------------------------
 */
class CClassifier {
protected:
    CvRNG rng;
    CvMat *parameters;

    // CS221 TO DO: ADD YOUR MEMBER VARIABLES HERE
    
public:
    // constructors
    CClassifier(bool verbose_flag/*=true*/, 
                size_t numtrees/*=1000*/, 
                size_t depth/*=2*/, 
                bool ourclassifier/*=false*/,
                CommandOptions* opts=0);
    
    // destructor
    virtual ~CClassifier();

    // load and save classifier configuration
    virtual bool loadState(const char *);
    virtual bool saveState(const char *);

    // run the classifier over a single frame
    virtual bool run(const IplImage *, CObjectList *);
        
    // train the classifier using given set of files
    virtual bool train(TTrainingFileList&);

    // test the classifier using given set of files, returns test error...
    virtual double test(TTrainingFileList& fls, Stats& stats); 
    
    // this function can spit out the values of the input parameters it got
    // from command line that is relevant to this class...
    void postCommandParseNotify();
private:
    // CS221 TO DO: ADD YOUR MEMBER FUNCTIONS HERE
    AbstractMulticlassClassifier* classifier;
    bool verbose;

    //Motion Tracking items
    int frameCount;
    LucasKanade lucasKanade;
    CommandOptions* ourOptions;
};

