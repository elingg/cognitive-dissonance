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
#include "CommandOptions.h"

using namespace std;

/* Main *********************************************************************/

int main(int argc, char *argv[])
{
    CommandOptions opts;
    opts.needStringOption("files","directory where training files are located", "/afs/ir/class/cs221/vision/data/vision_all");
    opts.needUintOption("fold","number of folds",3);
    opts.needBoolOption("trainerror","output training error as well (will slow it down)",false);
    opts.needUintOption("depth","max tree depth",2);
    opts.needStringOption("classifier","cv (CvBoost), outs (home grown AdaBoost)","cv");
    opts.needBoolOption("onefold","set if want just want on first fold",false);
    opts.needUintOption("examples","number of examples to use",0);
    opts.needUintOption("trees","number of trees in boosting",100);
    if(!opts.parseOptions(argc, argv)) {
      opts.usage();
      return -1;
    }

    size_t nFolds=opts.getUintOption("fold");
    string classifier_name=opts.getStringOption("classifier");
    bool bHomegrown = false; 
    if(classifier_name.compare("ours")==0) bHomegrown = true;
    size_t nTrees=opts.getUintOption("trees");
    size_t nDepth=opts.getUintOption("depth");

    size_t nExamples=opts.getUintOption("examples");
    bool bOneFold = opts.getBoolOption("onefold"); 
    // set defaults
    bool bVerbose = opts.getVerboseFlag();
    bool bTrainError = opts.getBoolOption("trainerror");
    cerr << "=================================================" << endl;
    cerr << "Using " << nFolds << " folds for validation." << endl;
    cerr << "Using " << classifier_name << " for classification." << endl;
    cerr << "Using " << nTrees << " trees in the boosted tree." << endl;
    cerr << "Using max depth " << nDepth << " in the tree." << endl;
    if(bOneFold) {
      cerr << "Ending after first fold to save time."<<endl;
    }
    if(bHomegrown) {
      cerr << "Using **homegrown** classifier: fasten your seatbelts" << endl;
    } else {
      cerr << "Using CvBoost classifier" << endl;
    }
    // load the training file list
    TTrainingFileList fileList;
    string training_dir = opts.getStringOption("files");
    cerr << "Using files in " << training_dir << endl;
    fileList = getTrainingFiles(training_dir.c_str(), ".jpg");
    // fileList = getTrainingFiles(*args, ".jpg");
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
      CommandOptions optsforthisrun = opts;
      CClassifier classifier(bVerbose,nTrees,nDepth,bHomegrown,&optsforthisrun);
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
