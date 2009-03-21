#pragma once
#include "CommandOptions.h"

// Add final overriding settings here!!!!
// Keep writing it in here, we will check with these uncommented....
void overwriteWithFinalSettingsForSubmission(CommandOptions& args) {
  args.addBoolOption("motiontracker",true);
  args.addDoubleOption("overlap_ratio",0.2);
  args.addUintOption("trees",1000);
  args.addUintOption("depth",2);
  args.addBoolOption("homegrownboost",false);
  args.addBoolOption("circle_feature", true);
  args.addBoolOption("corner_feature", true);
  args.addUintOption("framestoskip", 5);
}

