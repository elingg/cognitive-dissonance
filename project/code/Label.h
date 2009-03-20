#pragma once

#include <string>
#include <vector>
using namespace std;

/* Label:  Single place for all things labels */

typedef size_t Label; // using unsigned int (size_t) for efficiency of compares
//typedef string Label;

Label getLabel(const string& str);
string getLabelString(const Label& label);
string getClassifierFileName(const Label& label, const string& basename);
void getAllLabelStrings(vector<string>& labels);
bool isLabelEqual(const Label& l1, const Label& l2);
