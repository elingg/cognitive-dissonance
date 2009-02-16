#ifndef CARMODEL_H
#define CARMODEL_H

// The size of each continuous state
const static int STATE_SIZE = 4;
// The size of each continuous action
const static int ACTION_SIZE = 2;

/*******************************************
                  ^ Y  (theta=pi/2)
                  |
         o  o     |
 - - -  [HHHHH> - + - - - - - - > X (theta=0)
         o  o
 
 
 *******************************************/

// A state is an array of doubles with the following elements:
const static int STATE_Y = 0; // position relative to road centerline.
const static int STATE_VX = 1;  // velocity along the X axis (along the road).
const static int STATE_VY = 2; // velocity along Y axis (across the road).
const static int STATE_THETA = 3; // heading of car in radians (0 is down the road)

// An action is an array of doubles with the following elements:
const static int ACTION_STEERING = 0; // steering angle in radians
const static int ACTION_WHEEL_VEL = 1; // wheel "velocity" in meters/sec

// Maximum allowed steering angle.
const static double MAX_STEERING = 1.0;

// Time elapsed, in seconds, per simulator step.
const static double DT = 0.25;

/**
 * Sets a car state to the initial state.  This has the car facing
 * down the road, on the centerline, (positive X-axis), travelling at
 * 30m/s straight ahead.
 */
void setStateToInitial(double state[STATE_SIZE]);

/**
 * Computes the velocity of the car in the car's coordinate frame.
 */
void getBodyVelocity(const double state[STATE_SIZE], double v[2]);

/**
 * Simulates one step in the model.  The car begins in state 's0',
 * and the continuous action 'action' is simulated.  The resulting
 * state is then stored in 's1'.
 */
void simulate(const double s0[STATE_SIZE], 
              const double action[ACTION_SIZE],
              double s1[STATE_SIZE]);


#endif /* CARMODEL_H */
