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
    case(3): /*("mug"):*/ return 1; // 3
    case(7): /*("stapler"):*/ return 2; // 7
    case(8): /*("keyboard"):*/ if(str[0]=='k') return 3; else return 5; // 8
    case(5): /*("clock"):*/ return 4; // 5
    // case("scissors"): return 5; // 8
    default: return 0;
  }
}

#if 0
string getLabelString(const Label& l) {
  return l;
}
#endif 

string getLabelString(const Label& l) {
  // make more efficient...
  if(l==1) { return "mug"; }
  if(l==2) { return "stapler"; }
  if(l==3) { return "keyboard"; }
  if(l==4) { return "clock"; }
  if(l==5) { return "scissors"; }
  // else...
  return "other"; 
}

bool isLabelEqual(const Label& l1, const Label& l2) { 
  if(l1==l2) return true;
  return false;
}
