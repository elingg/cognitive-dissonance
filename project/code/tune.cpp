/*****************************************************************************
**  Utility for tuning our classifier/features.
**  Usage: To see usage type in: 
**      % ./tune -h
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
    opts.needBoolOption("onefold","set if want just want on first fold",false);
    opts.needUintOption("examples","number of examples to use",0);
    if(!opts.parseOptions(argc, argv)) {
      opts.usage();
      return -1;
    }

    size_t nFolds=opts.getUintOption("fold");

    size_t nExamples=opts.getUintOption("examples");
    bool bOneFold = opts.getBoolOption("onefold"); 
    // set defaults
    bool bTrainError = opts.getBoolOption("trainerror");
    cerr << "=================================================" << endl;
    CommandOptions copts = opts;
    CClassifier dummyclassifier(copts,false);
    if(!copts.parseOptions(argc, argv)) {
      assert(0);
    }
    dummyclassifier.postCommandParseNotify();
    cerr << "Using " << nFolds << " folds for validation." << endl;
    if(bOneFold) {
      cerr << "Ending after first fold to save time."<<endl;
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
    size_t fold_size = nExamples/nFolds;
    cout << "Running experiments on total of: " << nExamples << " files\n";
    cerr << "=================================================" << endl;
    cout << "Average test errors: \n";
    double avg_train_error=0;
    double avg_test_error=0;
    size_t actualFoldsRun=0;
    Stats stats;
    Timer t("entire experiment",true);
    for(size_t ifold=0; ifold<nFolds; ++ifold) {
      actualFoldsRun++;
      Timer t("fold ",true);
      CommandOptions optsforthisrun = opts;
      CClassifier classifier(optsforthisrun,false);
      if(!optsforthisrun.parseOptions(argc, argv)) {
        assert(0);
      }
      classifier.postCommandParseNotify(true);
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
