#pragma once
#include <vector>
#include "Label.h"
using namespace std;

// Class for collecting statistics on various runs....
class Stats {
public: 
  Stats();
  ~Stats();
  void log_prediction_result(const Label& actual, const Label& predicted);
  void print_prediction_result() const;
private:
  vector<vector<size_t> > m_prediction_results;
    
};
