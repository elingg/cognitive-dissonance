#include "Stats.h"
#include <iostream>

Stats::~Stats() {
}

Stats::Stats() {
  vector<string> labels;
  getAllLabelStrings(labels);
  m_prediction_results.resize(labels.size()); // one for each actual label
  for(size_t il=0; il<labels.size(); il++) {
    m_prediction_results[il].resize(labels.size(),0); // one for each predicted
  }
}

void Stats::log_prediction_result(const Label& actual, const Label& predicted) {
  // cerr << "Actual: " << getLabelString(actual) << ", Predicted: " << getLabelString(predicted) << endl;
  m_prediction_results[actual][predicted]++; 
}

void Stats::print_prediction_result() const {
  vector<string> labels;
  getAllLabelStrings(labels);
  cout << "-------------------------------------------------------------" << endl;
  cout << "Confusion Matrix: Predicted labels ->" << endl;
  cout << "-------------------------------------------------------------" << endl;
  cout << "\t\t";
  for(size_t i=0; i<labels.size(); i++) {
    labels[i].resize(5); 
    cout << labels[i] << "\t";
  }
  cout << "\n";
  cout << "-------------------------------------------------------------" << endl;
  for(size_t i=0; i<labels.size(); i++) {
    cout << labels[i] << "\t";
    for(size_t j=0; j<labels.size(); j++) {
      cout << "\t" <<  m_prediction_results[i][j];
    } 
    cout << endl;
  }
  vector<size_t> tp(labels.size(),0);
  vector<size_t> fp(labels.size(),0);
  vector<size_t> fn(labels.size(),0);
  for(size_t i=0; i<labels.size(); i++) {
    // for this actual label i
    for(size_t j=0; j<labels.size(); j++) {
      if(i==j) tp[i]+=m_prediction_results[i][j];
      else fn[i]+=m_prediction_results[i][j];
      if(i!=j) fp[j]+=m_prediction_results[i][j];
    }
  }
  cout << "\n";
  cout << "-------------------------------------------------------------" << endl;
  cout << "\tPrec\tRecall\tF-1\n";
  cout << "-------------------------------------------------------------" << endl;
  for(size_t i=0; i<labels.size(); i++) {
    cout << labels[i];
    double precision = (double)tp[i]/(double)(tp[i]+fp[i]);
    double recall = (double)tp[i]/(double)(tp[i]+fn[i]);
    double f1 = 2*precision*recall/(precision+recall);
    printf("\t%1.3lf\t%1.3lf\t%1.3lf\n",precision,recall, f1);
  }
  // Precision: TP/(TP+FP) (how many actual mugs in the total mugs predicted?)
  // Recall: TP/(TP+FN) (also TP rate) (how many mugs recognized from total mugs)
  // Accuracy: (TP+TN)/(TP+TN+FP+FN)
  // F1-measure: 2*P*R/(P+R)
}

