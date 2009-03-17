#include "CvBoostedClassifier.h"
#include "ImagesExample.h"
#include <iostream>
#include "cxcore.h"
#include "ml.h"
#include "Timer.h"

CvBoostedClassifier::CvBoostedClassifier(const Label& label, 
 size_t num_max_trees, size_t depth)
:m_label(label), m_num_max_trees(num_max_trees), m_depth(depth) {
}

CvBoostedClassifier::~CvBoostedClassifier() {
}

void CvBoostedClassifier::train(const vector<TrainingExample*>& examples) {
  if(examples.size()==0) return;
  const TrainingExample* sample_example = examples[0];
  size_t num_features = sample_example->getNumberOfFeatures()+
                    sample_example->getNumberOfFeatures()%2;  // 64-bit dummy
  size_t num_examples = examples.size();
  
  
  CvMat* var_type = cvCreateMat(num_features+1, 1, CV_8UC1); // 1 for label
  cvSet(var_type, cvScalarAll(CV_VAR_NUMERICAL));
  CV_MAT_ELEM(*var_type, char, num_features, 0) = CV_VAR_CATEGORICAL;

  CvMat* train_data = cvCreateMat(num_examples, num_features, CV_32F);
  CvMat* labels = cvCreateMat(num_examples, 1, CV_32SC1);
  { // Timer t("data conversion");
  for(size_t ie=0; ie<examples.size(); ++ie) {
    const TrainingExample* example = examples[ie];
    size_t ifeat=0;
    for(ifeat=0;ifeat< example->getNumberOfFeatures(); ifeat++) {
      // cerr << "setting train_data matrix: ie: " << ie << 
      // ", ifeat: " << ifeat << ", value: " << 
      // example->getFeatureDoubleValue(ifeat) << endl;
      cvmSet(train_data, ie, ifeat, example->getFeatureDoubleValue(ifeat));
      // REVISIT and make more efficient.
    }
    for(size_t iremain=ifeat; iremain<num_features; iremain++) {
      cvmSet(train_data, ie, iremain, 0.0); // dummy for 64-bit
    }
    bool binary_label = isLabelEqual(example->getLabel(),m_label);
    // cerr << "setting labels matrix: ie: " << ie 
    //  << ", value: " << binary_label << endl;
    CV_MAT_ELEM(*labels, int, ie, 0) = binary_label;
  }
  }
  // cerr << "Training cvboost for class: " << getLabelString(m_label) << endl;
  // cerr << " Label rows: " << labels->rows << ", cols: " << labels->cols << endl;
  // cerr << " Train data rows: " << train_data->rows << ", cols: " << train_data->cols << endl;

  CvBoostParams params(CvBoost::GENTLE, m_num_max_trees, 0.95, m_depth, false, 0);
  params.split_criteria = CvBoost::DEFAULT;
  m_cvimpl.train(train_data, CV_ROW_SAMPLE, labels, 0, 0, var_type, 0, 
                 params);
  // cerr << "Done training cvboost for class: " << getLabelString(m_label) << endl;
  cvReleaseMat(&train_data); 
  cvReleaseMat(&labels); 
  cvReleaseMat(&var_type); 
}

double CvBoostedClassifier::predict(const Example& example) const {
  CvMat* weak_responses = cvCreateMat(1, 
                             (const_cast<CvBoostedClassifier*>(this))->
                             m_cvimpl.get_weak_predictors()->total, 
                             CV_32F);
  size_t num_features = example.getNumberOfFeatures() + 
                        example.getNumberOfFeatures()%2;
  CvMat* exmat = cvCreateMat(1, num_features, CV_32F);
  size_t ifeat = 0;
  for(ifeat=0; ifeat<example.getNumberOfFeatures(); ifeat++) {
    cvmSet(exmat, 0, ifeat, example.getFeatureDoubleValue(ifeat));
  }
  for(size_t iremain=ifeat; iremain<num_features; iremain++) {
    cvmSet(exmat, 0, iremain, 0.0); // dummy for 64-bit
  }
  double return_val = m_cvimpl.predict(exmat, 0, 0); // -1.0 to 1.0 i think
  // cerr << "My label: " << m_label << " or " << getLabelString(m_label) << 
  //     ", Return value: " << return_val << endl;
  cvReleaseMat(&weak_responses);
  cvReleaseMat(&exmat);
  return return_val;
}

bool CvBoostedClassifier::loadState(const char* file) {
  m_cvimpl.load(file, "cogntree");
  return true;
}

bool CvBoostedClassifier::saveState(const char* file) const {
  (const_cast<CvBoostedClassifier*>(this))->m_cvimpl.save(file,"cogntree");
  return true;
}
