#include "reward.h"
#include "transitionmodel.h"
#include <iostream>

double reward(StateIndex dstate) {
  double state[STATE_SIZE];
  undiscretizeState(dstate, state);

  // Rewarded for:
  double yReward = -state[STATE_Y]*state[STATE_Y]; // staying near center line

  double vxReward = -0.1*state[STATE_VX]; // going forward is kinda bad...
  if (state[STATE_VX] < 0) {
    vxReward = state[STATE_VX]*state[STATE_VX]*2; // go back other way as fast as possible.
  }
  double vyReward = -state[STATE_VY]*state[STATE_VY]; // keeping horizontal speed low
  double headingReward = 0;//state[STATE_THETA]*state[STATE_THETA]; // facing opposite way

  return 2*yReward + vxReward + 0.1*vyReward + 5*headingReward;
}

double rewardRubber(StateIndex dstate) {
  double state[STATE_SIZE];
  undiscretizeState(dstate, state);

  double v[2];
  getBodyVelocity(state, v);

  return reward(dstate) + v[1]*v[1]*4;
}

