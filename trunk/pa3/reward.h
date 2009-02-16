#ifndef REWARD_H
#define REWARD_H

#include "transitionmodel.h"

// Returns the reward for being in the specified discrete state.  The
// basic reward penalizes for moving farther down the road (i.e.,
// positive X velocity), or for moving away from the center-line.  It
// gives a reward for facing and driving in the opposite direction
// (i.e., facing down the negative X axis, and large negative X
// velocity).
double reward(StateIndex state);

// Return a different reward that encourages the car to drive
// eratically.  Specifically, this returns reward(state), plus a term
// that rewards sideways motion.
double rewardRubber(StateIndex state);

#endif /* REWARD_H */
