#include "carmodel.h"
#include <math.h>

void setStateToInitial(double state[STATE_SIZE]) {
  state[STATE_Y] = state[STATE_VY] = state[STATE_THETA] = 0;
  state[STATE_VX] = 10;
}

static void rotate2d(double t, const double v0[2], double v[2]) {
  double ct = cos(t);
  double st = sin(t);
  v[0] = ct*v0[0] - st*v0[1];
  v[1] = st*v0[0] + ct*v0[1];
}

void getBodyVelocity(const double state[STATE_SIZE], double v[2]) {
  rotate2d(-state[STATE_THETA], state+STATE_VX, v);
}

const static double MU_S_BACK = 0.9;
const static double MU_D_BACK = 0.3;
const static double MU_S_FRONT = 1.2;
const static double MU_D_FRONT = 0.8;
const static double G = 9.81;
const static double L = 1.5;

// w00t for ridonculous hacked models like this one...
void simulate(const double s0[STATE_SIZE], 
              const double action[ACTION_SIZE],
              double s1[STATE_SIZE]) {
  double steeringAngle = action[ACTION_STEERING];
  double driveWheelVel = action[ACTION_WHEEL_VEL];
  double bodyVel[2];
  double frontWheelVel[2];

  if (steeringAngle > MAX_STEERING) { steeringAngle = MAX_STEERING; }
  if (steeringAngle < -MAX_STEERING) { steeringAngle = -MAX_STEERING; }
  if (driveWheelVel < 0) driveWheelVel = 0; // brakes.
  getBodyVelocity(s0, bodyVel);

  rotate2d(-steeringAngle, bodyVel, frontWheelVel);

  double af_prll = 0;
  double af_perp = -frontWheelVel[1] / DT;
  double af_norm = fabs(af_perp);
  if (af_norm > G*MU_S_FRONT) af_perp = G*MU_D_FRONT * (af_perp / af_norm);

  double ar_prll = (driveWheelVel - bodyVel[0]) / DT;
  double ar_perp = -bodyVel[1] / DT;
  double ar_norm = sqrt(ar_perp*ar_perp + ar_prll*ar_prll);
  if (ar_norm > G*MU_S_BACK) {
    ar_perp = ar_perp * G*MU_D_BACK / ar_norm;
    ar_prll = ar_prll * G*MU_D_BACK / ar_norm;
  }
  
  // integrate velocities to tires...
  frontWheelVel[0] += af_prll*DT;
  frontWheelVel[1] += af_perp*DT;
  bodyVel[0] += ar_prll * DT;
  bodyVel[1] += ar_perp * DT;

  double frontWheelBodyVel[2];
  rotate2d(steeringAngle, frontWheelVel, frontWheelBodyVel);
  double frontPos[2] = { L + frontWheelBodyVel[0]*DT,
                         frontWheelBodyVel[1]*DT };
  double backPos[2] = { bodyVel[0]*DT,
                        bodyVel[1]*DT };
  double newTheta = atan2(frontPos[1]-backPos[1], frontPos[0]-backPos[0]);

  // update next state.....
  rotate2d(s0[STATE_THETA], bodyVel, s1+STATE_VX);
  s1[STATE_Y] = s0[STATE_Y] + s1[STATE_VY]*DT;
  s1[STATE_THETA] = s0[STATE_THETA] + newTheta;
}
