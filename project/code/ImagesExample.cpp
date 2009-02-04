#include <iostream>
#include <sstream>
#include "ImagesExample.h"

ImagesExample::ImagesExample(vector<double> hfeatures, bool class_label)
   {
    for(int i=0; i<57; i++)
    {
        m_featureValues.push_back(hfeatures.at(i));
    }
  m_classLabel = class_label;
}


