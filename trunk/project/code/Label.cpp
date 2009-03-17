#include "Label.h"

#if 0
Label getLabel(const string& str) {
  return str;
}
#endif
string getClassifierFileName(const Label& label, const string& basename) {
  string fname("trained_classifier/");
  if(!basename.empty()) {
    fname=basename+"/";
  }
  fname = fname+getLabelString(label)+".dat";
  return fname;
}

Label getLabel(const string& str) {
  switch (str.size()) {
    // exploit known string sizes...
    case(3): /*("mug"):*/ return 0; // 3
    case(7): /*("stapler"):*/ return 1; // 7
    case(8): /*("keyboard"):*/ if(str[0]=='k') return 2; else return 4; // 8
    case(5): /*("clock"):*/ if(str[0]=='o') return 5; else return 3; // 5
    default: return 5;
  }
}

#if 0
string getLabelString(const Label& l) {
  return l;
}
#endif 

void getAllLabelStrings(vector<string>& labels) {
  labels.push_back("mug");
  labels.push_back("stapler");
  labels.push_back("keyboard");
  labels.push_back("clock");
  labels.push_back("scissors");
  labels.push_back("other");
}

string getLabelString(const Label& l) {
  // make more efficient...
  if(l==0) { return "mug"; }
  if(l==1) { return "stapler"; }
  if(l==2) { return "keyboard"; }
  if(l==3) { return "clock"; }
  if(l==4) { return "scissors"; }
  // else...
  return "other"; 
}

bool isLabelEqual(const Label& l1, const Label& l2) { 
  if(l1==l2) return true;
  return false;
}
