/*****************************************************************************
** CS221 Computer Vision Project
** Copyright (c) 2006, Stanford University
**
** FILENAME:    test.cpp
** AUTHOR(S):   Stephen Gould <sgould@stanford.edu>
** DESCRIPTION:
**  This file contains the main executable for running classifiers on
**  test video streams. You should not modify this file.
** EXAMPLE:
**  Executing with command-line
**      % ./test -v -c parameters.txt -g data/easy.xml data/easy.avi
**
**  will configure the classifier with parameters from file "parameters.txt"
**  and then run on the video "easy.avi" showing ground truth labels from
**  file "easy.xml".
*****************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "cvaux.h"

#include "objects.h"
#include "classifier.h"
#include "replay.h"

#include "MotionTracker.h"

#define WINDOW_NAME "CS221 Computer Vision Project"

using namespace std;

/* Main *********************************************************************/

void usage() {
    cerr << "./test [<options>] <AVI filename>" << endl << endl;
    cerr << "OPTIONS:" << endl;
    cerr << "    -c <filename>  :: configuration file for classifier" << endl;
    cerr << "    -f <fps>       :: frames per second (0 to pause each frame)" << endl;
    cerr << "    -g <filename>  :: ground truth file" << endl;
    cerr << "    -h             :: show this message" << endl;
    cerr << "    -o <filename>  :: output to file" << endl;
    cerr << "    -v             :: verbose" << endl;
    cerr << "    -x             :: disable display" << endl;
    cerr << endl;
}

int main(int argc, char *argv[])
{
    char *configurationFile;
    char *groundTruthFile;
    char *outputFile;
    char *aviFile;
    bool bVerbose;
    bool bShowVideo;
    int fpsDelay;
    char **args;
    
    // set defaults
    configurationFile = NULL;   // set using -c <filename>
    groundTruthFile = NULL;     // set using -g <filename>
    outputFile = NULL;          // set using -o <filename>
    bVerbose = false;           // turn on with -v
    bShowVideo = true;          // turn off with -x
    fpsDelay = 65;              // 65ms delay (~15fps)

    // check arguments
    args = argv + 1;
    while (argc-- > 2) {
        if (!strcmp(*args, "-c")) {
            argc--; args++;
            if (configurationFile != NULL) {
                usage();
                return -1;
            }
            configurationFile = *args;
        } else if (!strcmp(*args, "-f")) {
            argc--; args++;
            int fps = atoi(*args);
            if (fps <= 0) {
                fpsDelay = 0;
            } else {
                fpsDelay = 1000 / fps;
            }
        } else if (!strcmp(*args, "-g")) {
            argc--; args++;
            if (groundTruthFile != NULL) {
                usage();
                return -1;
            }
            groundTruthFile = *args;
        } else if (!strcmp(*args, "-h") || !strcmp(*args, "--help")) {
            usage();
            return 0;
        } else if (!strcmp(*args, "-o")) {
            argc--; args++;
            if (outputFile != NULL) {
                usage();
                return -1;
            }
            outputFile = *args;
        } else if (!strcmp(*args, "-v")) {
            bVerbose = !bVerbose;
        } else if (!strcmp(*args, "-x")) {
            bShowVideo = !bShowVideo;
        } else {
            cerr << "ERROR: unrecognized option " << *args << endl;
            return -1;
        }
        args++;
    }

    if (argc != 1) {
        usage();
        return -1;
    }

    aviFile = args[0];

    // show settings
    if (bVerbose) {
        cout << "Configuration file: "
             << (configurationFile == NULL ? "(none)" : configurationFile) << endl;
        cout << "Ground truth file:  " 
             << (groundTruthFile == NULL ? "(none)" : groundTruthFile) << endl;
        cout << "Output file:        "
             << (outputFile == NULL ? "(none)" : outputFile) << endl;
        cout << "Test AVI file:      " << aviFile << endl;       
    }

    CClassifier classifier;
    CObjectReplay replayer;
    CvFont font;
    ofstream *outputStream = NULL;

    // configure the classifier
    if (configurationFile != NULL) {
        if (!classifier.loadState(configurationFile)) {
            cerr << "ERROR: could not configure the classifier" << endl;
            exit(-1);
        }
    }
    
    // configure the replayer
    if (groundTruthFile != NULL) {
        if (!replayer.configure(groundTruthFile)) {
            cerr << "ERROR: could not configure ground truth replay" << endl;
            exit(1);
        }
    }

    // open the video file
    CvCapture *capture = cvCaptureFromAVI(aviFile);
    if (capture == NULL) {
        cerr << "ERROR: could not open AVI file" << endl;
        exit(-1);
    }

    // open the output file
    if (outputFile != NULL) {
        outputStream = new ofstream(outputFile);
        if ((outputStream == NULL) || (!outputStream->good())) {
            cerr << "ERROR: could not open output file" << endl;
            exit(-1);
        }
        *outputStream << "<ObjectList>" << endl;
    }

    // play the video and run classifier
    if (bShowVideo) {
        cvNamedWindow(WINDOW_NAME, CV_WINDOW_AUTOSIZE);
        cvInitFont(&font, CV_FONT_VECTOR0, 0.75, 0.75, 0.0f, 1, CV_AA);
    }

    IplImage *frame = NULL;
    CObjectList classifierObjects;
    CObjectList groundTruthObjects;

    KalmanFilter kalmanFilter; 
    CObjectList kalmanObjects;

    LucasKanade lucasKanade;
    CObjectList lkObjects;

    CObjectList::iterator iObj;
    int frameCount = 0;
    while ((frame = cvQueryFrame(capture)) != NULL) {
        frameCount += 1;
        if (bVerbose) {
            cout << "Processing frame " << frameCount << "..." << endl;
        }
        classifierObjects.clear();
        classifier.run(frame, &classifierObjects);
        groundTruthObjects.clear();
        replayer.run(frame, &groundTruthObjects);

        //Kalman Filter
	//TODO: Make this command-line argument
        bool USE_KALMAN = false;
	if(USE_KALMAN) {
          kalmanFilter.update(&classifierObjects); 
	  kalmanObjects = kalmanFilter.predict();
	  cout << "Kalman objects: " << kalmanObjects.size();
	  classifierObjects = kalmanObjects;
        }

	bool USE_LK = false;
	if(USE_LK) {
	  lkObjects = lucasKanade.getObjectList(frame, &classifierObjects); 
	}

        if (outputStream) {
            *outputStream << "  <frame id=\"" << (frameCount - 1) << "\">" << endl;
            for (iObj = classifierObjects.begin(); iObj != classifierObjects.end(); iObj++) {
                *outputStream << "    ";
                iObj->writeAsXML(*outputStream);
            }
            *outputStream << "  </frame>" << endl;
        }
        
        if (bShowVideo) {
            IplImage *frameCopy = cvCloneImage(frame);
            for (iObj = groundTruthObjects.begin(); iObj != groundTruthObjects.end(); iObj++) {
                iObj->draw(frameCopy, CV_RGB(255, 0, 0), &font);
            }
            for (iObj = classifierObjects.begin(); iObj != classifierObjects.end(); iObj++) {
                iObj->draw(frameCopy, CV_RGB(0, 255, 0), &font);
            }

	    //Show Kalman object
	    if(USE_KALMAN) {
              for (iObj = kalmanObjects.begin(); iObj != kalmanObjects.end(); iObj++) {
                iObj->draw(frameCopy, CV_RGB(0, 0, 255), &font);
	      }
	    }

            cvShowImage(WINDOW_NAME, frameCopy);
            cvReleaseImage(&frameCopy);

            // check for user pressing <esc> and delay for fpsDelay milliseconds
            if (cvWaitKey(fpsDelay) == 27) break;
        }
    }

    // clean up
    if (outputStream) {
        *outputStream << "</ObjectList>" << endl;
        outputStream->close();
        delete outputStream;
    }

    if (bShowVideo) {
        cvDestroyWindow(WINDOW_NAME);
    }

    cvReleaseCapture(&capture);

    return 0;
}
