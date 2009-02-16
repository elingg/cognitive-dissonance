#include "transitionmodel.h"
#include <iostream>

static double wrap_to_pi(double angle) {
  int num2pi = (int)floor(angle/2.0/M_PI  + 0.5);
  angle -= num2pi * 2 * M_PI;
  return angle;
}

ActionIndex getActionIndex(int steeri, int wheelveli) {
  steeri = std::min(std::max(steeri,0), STEERING_COUNT-1);
  wheelveli = std::min(std::max(wheelveli,0), WHEEL_VEL_COUNT-1);

  return wheelveli*STEERING_COUNT + steeri;
}

ActionIndex discretizeAction(const double a[ACTION_SIZE]) {
  int steeri, veli;

  for (steeri = 0; steeri < STEERING_COUNT; steeri++) {
    if (a[ACTION_STEERING] < STEERING_ANGLES[steeri]) break ;
  }
  if (steeri == STEERING_COUNT) steeri = STEERING_COUNT-1;
  else if (steeri > 0) {
    double lo = STEERING_ANGLES[steeri-1];
    double hi = STEERING_ANGLES[steeri];
    if (a[ACTION_STEERING] < 0.5*(lo+hi)) steeri--;
  }


  for (veli = 0; veli < WHEEL_VEL_COUNT; veli++) {
    if (a[ACTION_WHEEL_VEL] < WHEEL_VELOCITIES[veli]) break ;
  }
  if (veli == WHEEL_VEL_COUNT) veli = WHEEL_VEL_COUNT-1;
  else if (veli > 0) {
    double lo = WHEEL_VELOCITIES[veli-1];
    double hi = WHEEL_VELOCITIES[veli];
    if (a[ACTION_WHEEL_VEL] < 0.5*(lo+hi)) veli--;
  }

  return getActionIndex(steeri,veli);
}

void undiscretizeAction(ActionIndex action, double a[ACTION_SIZE]) {
  int wheelveli = (action/STEERING_COUNT)%WHEEL_VEL_COUNT;
  int steeri = action%STEERING_COUNT;

  a[ACTION_STEERING] = STEERING_ANGLES[steeri];
  a[ACTION_WHEEL_VEL] = WHEEL_VELOCITIES[wheelveli];
}


StateIndex getStateIndex(int yi, int vxi, int vyi, int thetai) {
  yi = std::min(std::max(yi, 0), Y_COUNT-1);
  vxi = std::min(std::max(vxi, 0), VX_COUNT-1);
  vyi = std::min(std::max(vyi, 0), VY_COUNT-1);
  thetai = std::min(std::max(thetai, 0), THETA_COUNT-1);

  StateIndex s = (thetai*Y_COUNT*VX_COUNT*VY_COUNT) +
    (vyi*Y_COUNT*VX_COUNT) +
    (vxi*Y_COUNT) +
    yi;

  if (s >= DISCRETE_STATE_COUNT) {
    std::cerr << "Internal: discrete state index out of bounds." << std::endl;
  }
  return s;
}

StateIndex discretizeState(const double state[STATE_SIZE]) {
  double y = (state[STATE_Y] + Y_LIMIT)/(2.0*Y_LIMIT)*Y_COUNT;
  double vx = (state[STATE_VX] + VX_LIMIT)/(2.0*VX_LIMIT)*VX_COUNT;
  double vy = (state[STATE_VY] + VY_LIMIT)/(2.0*VY_LIMIT)*VY_COUNT;
  double theta = (wrap_to_pi(state[STATE_THETA]) + THETA_LIMIT)/(2.0*THETA_LIMIT)*THETA_COUNT;
  
  return getStateIndex(int(y),int(vx),int(vy),int(theta));
}


void undiscretizeState(StateIndex discreteState, double state[STATE_SIZE]) {
  int yi = discreteState % Y_COUNT;
  int vxi = (discreteState / Y_COUNT) % VX_COUNT;
  int vyi = (discreteState / (Y_COUNT*VX_COUNT)) % VY_COUNT;
  int thetai = (discreteState / (Y_COUNT*VX_COUNT*VY_COUNT)) % THETA_COUNT;

  state[STATE_THETA] = (double(thetai)+0.5)/THETA_COUNT*2.0*THETA_LIMIT - THETA_LIMIT;
  state[STATE_VY] = (double(vyi)+0.5)/VY_COUNT*2.0*VY_LIMIT - VY_LIMIT;
  state[STATE_VX] = (double(vxi)+0.5)/VX_COUNT*2.0*VX_LIMIT - VX_LIMIT;
  state[STATE_Y] = (double(yi)+0.5)/Y_COUNT*2.0*Y_LIMIT - Y_LIMIT;
}
