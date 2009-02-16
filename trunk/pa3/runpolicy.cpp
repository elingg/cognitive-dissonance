#include "simulation.h"
#include "graphics.h"
#include "carmodel.h"
#include "transitionmodel.h"
#include <iostream>
#include <fstream>
#include <time.h>

int main(int argc, char* argv[]) {
  // check parameters
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " results_file" << std::endl;
    return -1;
  }

  // policy and value function
  ActionIndex piStar[DISCRETE_STATE_COUNT];
  double valueFunction[DISCRETE_STATE_COUNT];

  // read the data file
  std::fstream datafile(argv[1], std::ios::in|std::ios::binary);
  datafile.read((char*)piStar, sizeof(piStar)); 
  if (datafile.fail()) {
    std::cerr << "Couldn't read the policy from file: " << argv[1] << std::endl;
    return 1;
  }
  datafile.read((char*)valueFunction, sizeof(valueFunction));
  if (datafile.fail()) {
    std::cerr << "Couldn't read the value function from file: " << argv[1] << std::endl;
    return 1;
  }

  double initState[STATE_SIZE];
  setStateToInitial(initState);
  StateIndex initStateIndex = discretizeState(initState);
  std::cerr << "V(s_0) = " << valueFunction[initStateIndex] << std::endl;

  InitState(piStar, valueFunction);
  InitGraphics(argc, argv);
  LaunchGUI();

  return 0;
}
