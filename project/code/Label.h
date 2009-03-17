#pragma once

#include <string>
using namespace std;

typedef uint Label;
//typedef string Label;

Label getLabel(const string& str);
string getLabelString(const Label& label);
string getClassifierFileName(const Label& label, const string& basename);

bool isLabelEqual(const Label& l1, const Label& l2);
