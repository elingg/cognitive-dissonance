#pragma once

// Add final overriding settings here!!!!
// Keep writing it in here, we will check with these uncommented....
void overwriteWithFinalSettingsForSubmission(CommandOptions& args) {
  args.addUintOption("motiontracker",true);
  args.addUintOption("overlap_ratio",0.2);
  args.addUintOption("trees",1000);
  args.addUintOption("depth",2);
  args.addBoolOption("homegrownboost",false);
  args.addBoolOption("circle_feature", true);
  args.addBoolOption("corner_feature", true);
  args.addBoolOption("framestoskip", 5);
}

