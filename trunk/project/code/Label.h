#pragma once

#include <string>
#include <vector>
using namespace std;

// Single place for all things labels...

typedef uint Label;
//typedef string Label;

Label getLabel(const string& str);
string getLabelString(const Label& label);
string getClassifierFileName(const Label& label, const string& basename);
void getAllLabelStrings(vector<string>& labels);
bool isLabelEqual(const Label& l1, const Label& l2);
