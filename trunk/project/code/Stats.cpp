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
  m_prediction_results[actual][predicted]++; 
}

void Stats::print_prediction_result() const {
  vector<string> labels;
  getAllLabelStrings(labels);
  cout << "Confusion Matrix:\n\t\t";
  for(size_t i=0; i<labels.size(); i++) {
    labels[i].resize(5); 
    cout << labels[i] << "\t";
  }
  cout << "\n";
  for(size_t i=0; i<labels.size(); i++) {
    cout << labels[i] << "\t";
    for(size_t j=0; j<labels.size(); j++) {
      cout << "\t" <<  m_prediction_results[i][j];
    } 
    cout << endl;
  } 
}

