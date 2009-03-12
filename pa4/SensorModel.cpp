#include <iostream>
#include <stdlib.h>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "Maps.h"
#include "Robot.h"
#include "SensorModel.h"
#include "ProbUtils.h"
#define C_BUF   100 // size of buffer

/* =================================================================== */
/*                         BumpSensorModel                             */
/* =================================================================== */

BumpSensorModel::BumpSensorModel()
{
  bmap = NULL;
}

/* Initialize */
/* ---------- */
/* Note: We DO NOT bother deleting this in a destructor because we
 * don't know if somebody else is still using the bmap.
 */
 void BumpSensorModel::Initialize(BeliefMap *bm) { bmap = bm; }

/* SetReadings */
/* ----------- */
/* Nothing doing here.  We don't even bother reading the robot's state
 * because we just assume the robot never ran into anything.  So... we
 * don't even bother reading the robot state.  The only reason we have
 * this function is because we inherited it from the parent class, and 
 * it was a pure virtual method that needed implementation.
 */
void BumpSensorModel::SetReadings(RobotState *robotState){}

/* GetPFromMapCoord */
/* ---------------- */
/* Basically all we do here is just see if the map coordinate given
 * corresponds to a free cell.  If it does not, then we return 0.0
 * because there's a zero probability that the robot would have missed
 * bumping.  If it does, then we return 1.0.  
 *
 * Note: we are pretty much assuming that the bump sensor works
 * perfectly.
 */
double BumpSensorModel::GetPFromMapCoord(MapCoord mc)
{
  if(bmap->GetCell(mc)->type != FREE)
    return 0.0;
  else
    return 1.0;
}
/* =================================================================== */
/*                        LaserArraySensorModel                        */
/* =================================================================== */

LaserArraySensorModel::LaserArraySensorModel() 
{
  bmap = NULL; 
}

/* Initialize */
/* ---------- */
/* Takes in the belief map and the the standard deviation to use with
 * the laser sensor model.
 */

void LaserArraySensorModel::Initialize(BeliefMap *bm, double stddev)
{
  bmap = bm; 
  this->stddev = stddev;
}

void LaserArraySensorModel::SetReadings(RobotState *robotState)
{
  this->lsr = robotState->lsr;
}

/* GetProbReadingGivenDistance */
/* --------------------------- */
/* This basically returns the probability of one laser giving us a
 * reading of "sensor" distance to the closest obstacle given that the
 * map tells us that really the distance should be "distance".
 */

double LaserArraySensorModel::GetProbReadingGivenDistance(double sensor, double distance)
{
  double alpha;

  // alpha is 1.0 divided by the area of the normal curve that is to
  // the right of 0.0.  basically it is our renormalization constant.
  alpha = 1.0 / (1.0 - GetCND(0.0, distance, stddev));

  return alpha * GetProbND(sensor, distance, stddev);
}

/* GetPFromMapCoord */
/* ---------------- */
/* We assume each reading from the lasers are independent from one
 * another, so we just return the product of the probabilities
 * associated with each one.
 */
double LaserArraySensorModel::GetPFromMapCoord(MapCoord mc)
{
  double result = 1.0;
  int i;

  // iterate over each of the "lasers" in the directions, and multiply the
  // probabilities with each other.
  for(i = 0; i < NUM_DIRECTIONS; i++)
    result *= GetProbReadingGivenDistance(lsr.dist[i], bmap->GetDistanceToWall(mc, i));

  return result;
}

/* =================================================================== */
/*                        CustomLASensorModel                          */
/* =================================================================== */

/* CS221 Students: The following class is yours to change.  Really
 * though, all you need to edit is the GetProbReadingGivenDistance
 * method.  But... for the sake of creativity, change anything (as
 * long as everything still works...).
 */

/** CS221 TASK 5: BEGIN EDIT CODE **/

CustomLASensorModel::CustomLASensorModel() 
{
  bmap = NULL; 
}

/* Initialize */
/* ---------- */
/* Takes in the belief map and the the standard deviation to use with
 * the laser sensor model.
 */

void CustomLASensorModel::Initialize(BeliefMap *bm, double stddev,
   double sensorWt, double unexpectedWt, double failureWt, double inexplicableWt)
{
  bmap = bm; 
  this->stddev = stddev;
  sensorModelWeight = sensorWt;
  unexpectedModelWeight = unexpectedWt;
  failureModelWeight = failureWt;
  inexplicableModelWeight = inexplicableWt;

  //Outputs numbers for graphing
  /*
  for(int j = 0; j < 30; j++) {
    double prob = GetProbReadingGivenDistance(j, 5);
    cout << j << " " << prob << endl;
  }
  */
}

void CustomLASensorModel::SetReadings(RobotState *robotState)
{
  this->lsr = robotState->lsr;
}

/* GetProbReadingGivenDistance */
/* --------------------------- */
/* This basically returns the probability of one laser giving us a
 * reading of "sensor" distance to the closest obstacle given that the
 * map tells us that really the distance should be "distance".
 *
 * CS221 Students: You'll want to change this method at the very
 * least.  Check out ProbUtils.h for some useful functions for the
 * Normal distribution.
 */

double CustomLASensorModel::GetProbReadingGivenDistance(double sensor, double distance)
{
  //Take into account the maximum effective range dmax (pg. 6)
  const double dMax = 30.0;
  // cerr << " sensor: " << sensor << ", distance: " << distance << endl;
  if(sensor > dMax) {
    sensor = dMax;
  }
  
  // alpha is 1.0 divided by the area of the normal curve that is to
  // the right of 0.0.  basically it is our renormalization constant.

  //Model the noise with the sensor (same as initial sensor model)
  double sensorAlpha = 1.0 / (1.0 - GetCND(0.0, distance, stddev));
  double sensorModel = sensorAlpha * GetProbND(sensor, distance, stddev);

  //The rest of these distributions are based on pg. 7 of the handout

  //Model unexpected obstacles (exponential distribution)
  double lambda = 0.5;
  double unexpectedModel = GetProbExponential(sensor, lambda);

  //Model laser failures (spikey distribution?)
  //TODO: Verify that this is the correct approach
  double failureModel = 0;
  if(sensor == dMax) {
    failureModel = 1.0;
  }

  //Model inexplicable measurements (uniform distribution)
  double inexplicableModel = GetProbUniform(dMax); 
  
  //We're returning a mixed model.  These are the relative weights.  These
  //must add up to 1.0
  //TODO: Allow these numbers to be passed in as parameters
  //double sensorModelWeight = 0.968;
  //double unexpectedModelWeight = 0.001;
  //double failureModelWeight = 0.001;
  //double inexplicableModelWeight = 0.03;

  //Make sure probabilities add up to 1.  We have to assert a range because
  //of crazy floating point math
  assert(sensorModelWeight + unexpectedModelWeight + 
         failureModelWeight + inexplicableModelWeight > 0.999);
  assert(sensorModelWeight + unexpectedModelWeight + 
         failureModelWeight + inexplicableModelWeight < 1.001);

  return sensorModelWeight * sensorModel +
	 unexpectedModelWeight * unexpectedModel + 
	 failureModelWeight * failureModel +
	 inexplicableModelWeight * inexplicableModel;
}

/* GetPFromMapCoord */
/* ---------------- */
/* We assume the readings from the lasers are independent from one
 * another, so we just return the product of the probabilities
 * associated with each one.
 *
 * CS221 Students: you may have to change this method if you add more
 * arguments to the GetProbReadingsGivenDistance method.
 */

double CustomLASensorModel::GetPFromMapCoord(MapCoord mc)
{
  double result = 1.0;
  int i;

  // iterate over each of the "lasers" in the directions, and multiply the
  // probabilities with each other.

  for(i = 0; i < NUM_DIRECTIONS; i++)
    result *= GetProbReadingGivenDistance(lsr.dist[i], bmap->GetDistanceToWall(mc, i));

  return result;
}

/** CS221 TASK 5: END EDIT CODE **/

/* =================================================================== */
/*                    LaserArrayAndBumpSensorModel                     */
/* =================================================================== */

/* CS221 Students: This class will allow us to package to sensor
 * models together, making the assumption that the two give
 * independent readings.
 */

LaserArrayAndBumpSensorModel::LaserArrayAndBumpSensorModel()
{
  lsm = NULL;
  bsm = NULL;
}

/* Initialize */
/* ---------- */
/* Just sets the internal laser and bump sensor models.  Be sure to
 * initialize the input sensor models separately before throwing them
 * in here, because we don't do that here.
 */
void LaserArrayAndBumpSensorModel::Initialize(SensorModel *l, SensorModel *b)
{
  lsm = l;
  bsm = b;
}

void LaserArrayAndBumpSensorModel::SetReadings(RobotState *robotState)
{
  if (lsm)
    lsm->SetReadings(robotState);
  if (bsm)
    bsm->SetReadings(robotState);
}

/* GetPFromMapCoord */
/* ---------------- */
/* Just assume that the probabilities from each sensor model are
 * independent from each other.
 */
double LaserArrayAndBumpSensorModel::GetPFromMapCoord(MapCoord mc)
{
  double p1 = 1.0;
  double p2 = 1.0;

  if (lsm)
    p1 = lsm->GetPFromMapCoord(mc);
  if (bsm)
    p2 = bsm->GetPFromMapCoord(mc);

  return(p1 * p2);
}
