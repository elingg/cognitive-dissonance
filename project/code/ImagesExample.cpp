#include <iostream>
#include <sstream>
#include "ImagesExample.h"

ImagesExample::ImagesExample(const vector<double>& hfeatures)
:m_featureValues(hfeatures) {}
ImagesExample::ImagesExample(const vector<double>& hfeatures, 
                            const string& class_label)
:m_featureValues(hfeatures),m_classLabel(class_label) {} 


