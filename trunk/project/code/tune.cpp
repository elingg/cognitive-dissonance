/*****************************************************************************
**  Usage 
**      % ./tune -c parameters.txt -v
**                     -folds nfolds -trees num_forest_trees
**                     -classifier <cv | ours>
**                     -tol leaf_tolerance 
**                      data 
**
*****************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <sys/types.h>
#include <dirent.h>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "utils.h"
#include "objects.h"
#include "classifier.h"
#include "Timer.h"

using namespace std;

/* Main *********************************************************************/

void usage() {
    cerr << "./tune [<options>] <directory>" << endl << endl;
    cerr << "OPTIONS:" << endl;
    cerr << "    -h             :: show this message" << endl;
    cerr << "    -fold          :: number of folds " << endl;
    cerr << "    -depth         :: max tree depth " << endl;
    cerr << "    -trees         :: number of trees in boosted" << endl;
    cerr << "    -classifier    :: cv (OpenCV boost), ours (home grown adaboost)\n";
    cerr << "    -examples      :: number of examples to use\n";
    cerr << "    -trainerror    :: output training error as well (will slow it down)\n";
    cerr << "    -onefold       :: just perform one fold of the 'n' folds" <<endl;
    cerr << "    -v             :: verbose" << endl;
    cerr << endl;
}

int main(int argc, char *argv[])
{
    char *configurationFile = 0;
    bool bVerbose;
    char **args;
    size_t nFolds=3;
    string classifier_name("cv");
    size_t nTrees=100;
    size_t nDepth=2;
    size_t nExamples=0;
    bool bOneFold = false; 
    // set defaults
    configurationFile = NULL;   // set using -c <filename>
    bVerbose = false;           // turn on with -v
    bool bTrainError = false;
    // check arguments
    args = argv + 1;
    while (argc-- > 2) {
        if (!strcmp(*args, "-fold")) {
            argc--; args++;
            nFolds = atoi(*args);
            if(nFolds<=1) { 
              cerr << "Specify atleast two folds" << endl;
              usage(); 
              return -1;
            }
        } else if (!strcmp(*args, "-examples")) {
            argc--; args++;
            nExamples = atoi(*args);
        } else if (!strcmp(*args, "-depth")) {
            argc--; args++;
            nDepth = atoi(*args);
        } else if (!strcmp(*args, "-trees")) {
            argc--; args++;
            nTrees= atoi(*args);
        } else if (!strcmp(*args, "-classifier")) {
            argc--; args++;
            classifier_name = *args;
            if(strcmp(classifier_name.c_str(),"cv")&&
               strcmp(classifier_name.c_str(),"ours")) {
              usage();
              return -1;
            } 
        } else if (!strcmp(*args, "-c")) {
            argc--; args++;
            if (configurationFile != NULL) {
                usage();
                return -1;
            }
            configurationFile = *args;
        } else if (!strcmp(*args, "-h")) {
            usage();
            return 0;
        } else if (!strcmp(*args, "-onefold")) {
            bOneFold = true;
        } else if (!strcmp(*args, "-trainerror")) {
            bTrainError = true;
        } else if (!strcmp(*args, "-v")) {
            bVerbose = !bVerbose;
        } else {
            cerr << "ERROR: unrecognized option " << *args << endl;
            return -1;
        }
        args++;
    }
    if (argc != 1) {
      usage();
      exit(-1);
    }
    cerr << "=================================================" << endl;
    cerr << "Using " << nFolds << " folds for validation." << endl;
    cerr << "Using " << classifier_name << " for classification." << endl;
    cerr << "Using " << nTrees << " trees in the boosted tree." << endl;
    cerr << "Using max depth " << nDepth << " in the tree." << endl;
    if(bOneFold) {
      cerr << "Ending after first fold to save time."<<endl;
    }
    // load the training file list
    TTrainingFileList fileList;
    fileList = getTrainingFiles(*args, ".jpg");
    // mix it up
    random_shuffle(fileList.files.begin(),fileList.files.end());
    if(nExamples==0)
      nExamples = fileList.files.size();
    cerr << "Using " << nExamples << " examples" << endl;
    cerr << "=================================================" << endl;
    size_t fold_size = nExamples/nFolds;
    Timer t("entire experiment",true);
    cout << "Running experiments on total of: " << nExamples << " files\n";
    Stats stats;
    cout << "Average test errors: \n";
    double avg_train_error=0;
    double avg_test_error=0;
    size_t actualFoldsRun=0;
    for(size_t ifold=0; ifold<nFolds; ++ifold) {
      actualFoldsRun++;
      Timer t("fold ",true);
      CClassifier classifier(bVerbose,nTrees,nDepth,false);
      TTrainingFileList trainFileList, testFileList;
      trainFileList.classes = fileList.classes;
      testFileList.classes = fileList.classes;
      for(size_t iofold=0; iofold<nFolds; ++iofold) {
        size_t fold_start = fold_size*iofold;
        size_t fold_stop = fold_start+fold_size;
        if(iofold==(nFolds-1)) { // last fold
          fold_stop = nExamples;
        }
        for(size_t ifile=fold_start; ifile<fold_stop; ifile++) {
          if(ifold==iofold) {
            testFileList.files.push_back(fileList.files[ifile]);
          } else {
            trainFileList.files.push_back(fileList.files[ifile]);
          }
        }
      } 
      // now train the classifier
      if (!classifier.train(trainFileList)) {
        cerr << "ERROR: could not train classifier" << endl;
        exit(-1);
      }
      double test_error = classifier.test(testFileList,stats);
      cout << "Test error: " << test_error << endl;
      avg_test_error+= test_error;
      if(bTrainError) {
        Stats trainstats;
        double train_error = classifier.test(trainFileList,trainstats);
        avg_train_error += train_error;
        cout << "Training error: " << train_error << endl;
      }
      cout << "Fold " << ifold <<": " << test_error << endl;
      if(bOneFold) { 
        cout << "Terminating after 1 fold" << endl;
        break;
      }
    }
    if(bTrainError)  {
      avg_train_error /= (double)actualFoldsRun;
    }
    avg_test_error /= (double)actualFoldsRun;
    cerr << "------------------------------------------------" << endl;
    cout << "Avg Training error\t Avg Test error\n";
    if(bTrainError) {
      cout << avg_train_error << "\t\t" << avg_test_error <<endl;
    }
    else { 
      cout << "-n/a-" << "\t\t" << avg_test_error <<endl;
    }
 
    cerr << "------------------------------------------------" << endl;
    stats.print_prediction_result();
    return 0;
}
