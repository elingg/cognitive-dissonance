#include "simulation.h"
#include "carmodel.h"
#include "graphics.h"
#include "transitionmodel.h"

static double carState[STATE_SIZE];
static double carStateX;
static ActionIndex * piStar = NULL;
static double * valueFunction = NULL;

void InitState(ActionIndex * policy, double * vf)
{
  piStar = policy;
  valueFunction = vf;

 // Initialize state...
  carStateX = 0;
  setStateToInitial(carState);
}


void UpdateState()
{
  
  StateIndex dstate = discretizeState(carState);
  ActionIndex a = piStar[dstate];
  

  double continuousAction[2];
  undiscretizeAction(a, continuousAction);
  
  simulate(carState, continuousAction, carState);
  carStateX += carState[STATE_VX]*DT;
  
  // display the car at position =  carStateX, carState[STATE_Y]
  // with orientation = carState[STATE_THETA]
  UpdateCar(carStateX, carState[STATE_Y], carState[STATE_THETA]);
 
}
