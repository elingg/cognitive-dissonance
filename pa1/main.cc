#include "robotsim.hh"
#include "robotconfig.hh"
#include "agent.hh"
#include "cmdline.hh"
#include <stdlib.h>
#include <time.h>

////////
//Main//
////////

RobotConfig config;
Agent       agent;


void errorMessage() {
  fprintf(stderr, "usage:  rpplan [-points x] [-seed x] [-text] -file filename\n");
  exit(-1);
}

void commandLineParse(int argc, char **argv, 
                      int *seed, int *numPoints, 
                      bool *textMode, string *filename) {
  CCmdLine cmdLine;
  cmdLine.SplitLine(argc, argv);
  
  string numPointsS = cmdLine.GetSafeArgument( "-points", 0, "20" );
  string seedS      = cmdLine.GetSafeArgument( "-seed", 0, "-1" );
  *filename         = cmdLine.GetSafeArgument( "-file", 0, "");
  
  *numPoints = atoi(numPointsS.c_str());
  *seed      = atoi(seedS.c_str());
  *textMode  = cmdLine.HasSwitch("-text");

  if (*filename == "") {
    fprintf(stderr, "Please specify filename\n");
    errorMessage();
  }
    

  if (*numPoints<=0)
    *numPoints=20;
}

int main(int argc, char **argv) {

  int numPoints;
  int seed;
  bool textMode;
  string filename;

  commandLineParse(argc, argv, &seed, &numPoints, &textMode, &filename);
  
  if (seed != -1){
    printf("Seed: %d\n", seed);
    srand(seed);
  }
  else {
    srand(time(NULL));
    printf("Seed: none\n");
  }
  
  config.load(filename.c_str());
  printf("WorldFile: %s\n", filename.c_str());

  agent.init(config, numPoints);  
  
  if (textMode) {
    agent.runTests();
  }
  else {
    RobotSim sim;
    sim.run(argc, argv);
  }
}
