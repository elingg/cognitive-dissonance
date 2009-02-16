#include "carmodel.h"
#include "transitionmodel.h"
#include "reward.h"
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>

// ------- CONSTANTS -------
const static int EXPLORATION_RUNS = 1000000;
//const static int EXPLORATION_RUNS = 100000;

// ------- PROTOTYPES -----
static void setStateToRandom(double state[STATE_SIZE]);
static double expectedValue(const TransitionModel* model,
                            StateIndex s0, ActionIndex a, 
                            const double valueFunction[DISCRETE_STATE_COUNT]);
static void saveData(const std::string& fileName,
                     const double valueFunction[DISCRETE_STATE_COUNT],
                     const ActionIndex piStar[DISCRETE_STATE_COUNT]);


// ------- MAIN -------
int main(int argc, char* argv[]) {
  TransitionModel model;
  double          valueFunction[DISCRETE_STATE_COUNT];
  ActionIndex     piStar[DISCRETE_STATE_COUNT];

  // Grab the saved file name
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " outputfile.bin" << std::endl;
    return 1;
  }

  // Seed PRNG.  (You can pick a fixed random seed to help debugging).
  srand(time(NULL)); 

  // ----------------------------------------------------
  // | 4.1 EXPLORATION
  // ----------------------------------------------------
  // First, explore to build up a model

  double state0[STATE_SIZE];
  double state1[STATE_SIZE];
  double action[ACTION_SIZE];

  for (int i = 0; i < EXPLORATION_RUNS; i++) {
    //
    // TODODONE:  Put your exploration code here.
    //
    if(i % 1000 == 0) {
      std::cerr << "Exploration Iteration: " << i << "\n";
    }
    setStateToRandom(state0);
    for(int steeri = 0; steeri < STEERING_COUNT; steeri++) {
      for(int wheelveli = 0; wheelveli < WHEEL_VEL_COUNT; wheelveli++) {
        ActionIndex actionIndex = getActionIndex(steeri, wheelveli);
        undiscretizeAction(actionIndex, action);
	simulate(state0, action, state1); 
        StateIndex state0Index = discretizeState(state0);
	StateIndex state1Index = discretizeState(state1);
	model.addCount(state0Index, actionIndex, state1Index);
      }
    }
  }

  //Sanity check that the probabilities for the last state0 are non-zero
  /*
  for(int steeri = 0; steeri < STEERING_COUNT; steeri++) {
    for(int wheelveli = 0; wheelveli < WHEEL_VEL_COUNT; wheelveli++) {
      ActionIndex actionIndex = getActionIndex(steeri, wheelveli);
      StateIndex state0Index = discretizeState(state0);
      StateIndex state1Index = discretizeState(state1);
      double p = model.probability(state0Index, actionIndex, state1Index);
      std::cerr << "Probability: " << p << "\n";
    }
  }
  */

  std::cout << "Finished exploring.            " << std::endl;


  // ----------------------------------------------------
  // | 4.2 SOLVE THE MDP
  // ----------------------------------------------------
  // Run value iteration to find the value of each state.
  const static double GAMMA = 0.8;

  double prevValueFunction[DISCRETE_STATE_COUNT];
  // Clear the value function
  for (StateIndex i = 0; i < DISCRETE_STATE_COUNT; i++) {
    prevValueFunction[i] = valueFunction[i] = 0;
  }

  // Do Value Iteration:
  //
  // TODO:  Put your Value Iteration code here.
  //


  // ----------------------------------------------------
  // | 4.3 COMPUTE OPTIMAL POLICY
  // ----------------------------------------------------

  //
  // TODO: Use your computed value function from above to compute the
  // best action for every state.
  //


  // Save your output policy.
  saveData(argv[1], valueFunction, piStar);

  return 0;
}

/**
 * Sets the state vector to a random state within the limits of the
 * discretized state space.
 */
static void setStateToRandom(double state[STATE_SIZE]) {
  state[STATE_Y] = 2*Y_LIMIT * double(rand()) / double(RAND_MAX) - Y_LIMIT;
  state[STATE_VX] = 2*VX_LIMIT * double(rand()) / double(RAND_MAX) - VX_LIMIT;
  state[STATE_VY] = 2*VY_LIMIT * double(rand()) / double(RAND_MAX) - VY_LIMIT;
  state[STATE_THETA] = 2*THETA_LIMIT * double(rand()) / double(RAND_MAX) - THETA_LIMIT;
}

/**
 * Returns the expected value of taking action 'a' from state 's0',
 * according to the value function 'valueFunction'.  This code
 * computes sum(T(s0, a, s1) * valueFunction[s1]) for you efficiently
 * by skipping over states where T(s0,a,s1) == 0.
 */
static double expectedValue(const TransitionModel* model,
                            StateIndex s0, ActionIndex a, 
                            const double valueFunction[DISCRETE_STATE_COUNT]) {
  const Distribution* dist = model->getDistribution(s0,a);
  if (!dist) return 0;

  double sum = 0;
  Distribution::CountMapType::const_iterator itr;
  for (itr = dist->mapBegin(); itr != dist->mapEnd(); ++itr) {
    double p = double(itr->second)/double(dist->getSampleCount());
    StateIndex s1 = itr->first;
    sum += p*valueFunction[s1];
  }
  return sum;
}


/**
 * Saves the optimal policy and value function from the global
 * variables to a file.
 */
static void saveData(const std::string& fileName,
                     const double valueFunction[DISCRETE_STATE_COUNT],
                     const ActionIndex piStar[DISCRETE_STATE_COUNT]) {
  std::fstream output(fileName.c_str(), std::ios::out|std::ios::binary);
  output.write((const char*)piStar, sizeof(ActionIndex)*DISCRETE_STATE_COUNT);
  output.write((const char*)valueFunction, sizeof(double)*DISCRETE_STATE_COUNT);
  if (output.fail()) {
    std::cerr << "Saving data to file " << fileName << " failed!" << std::endl;
  }
}
