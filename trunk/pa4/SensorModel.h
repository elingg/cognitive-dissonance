#ifndef _SENSORMODEL_H
#define _SENSORMODEL_H

class RobotState;
class BeliefMap;
class MapCoord;
class LaserArraySensorReading;

/* CS221 Students: It's important to read the following description of
 * the SensorModel, as well as CustomLASensorModel if you're
 * implementing your own custom laser array sensor model.
 */

/* SensorModel */
/* ----------- */
/* This is the parent class of all other SensorModel classes.  To use
 * any sensor model, pretty much you'll need to know how the
 * SensorModel's methods work.  Remember that a sensor model
 * essentially gives us P(o' | s'), i.e. the probability of getting
 * sensor readings o', given that we are at s' = (x',y').
 *
 * To use any sensor model, 
 * 1. Call SetReadings to set what readings the robot recorded.
 * 2. You may then call GetPFromMapCoord(MapCoord mc) to find P(o' | s') 
 *    where s' = (x',y') are in MapCoord mc.
 */
class SensorModel
{
public:
  virtual ~SensorModel() {}
  virtual void SetReadings(RobotState *robotState) = 0;
  virtual double GetPFromMapCoord(MapCoord mc) = 0;
};

/* -------------------------------- */
/* Implementations of Sensor Models */
/* -------------------------------- */

/* BumpSensorModel */
/* --------------- */
/* Bump sensor model basically is centered around a sensor that checks
 * to see if the robot bumped into anything.  Technically, we should
 * be reading this sensor readings from the log file, but we just
 * assume that the robot never actually ran into anything throughout
 * the course of its journey.  So... really when we're getting the
 * probabilities, we just check the map coordinates given and see if
 * its a free cell.  If it is, then the robot shouldn't have bumped.
 * If it isn't then the robot should have.
 *
 * The BumpSensorModel must be initialized before being used.
 */ 

class BumpSensorModel : public SensorModel
{
private:
  BeliefMap *bmap;        // need the map so we can see if the robot
			  // should have bumped at a cell
  RobotState *robotState; // technically, i don't think this is used
			  // even.
public:
  BumpSensorModel();
  virtual ~BumpSensorModel(){}
  void Initialize(BeliefMap *dm);

  virtual void SetReadings(RobotState *robotState);
  virtual double GetPFromMapCoord(MapCoord mc);
};

/* LaserArraySensorModel */
/* ---------------- */
/* For decomposition purposes, we've placed the lasers sensor model
 * here.  This is the simplistic lasers sensor model that doesn't
 * really take much into account.  It pretty much uses two insights:
 *
 * 1.  The readings from each of the four lasers are independent of 
 *     one another.
 * 2.  A laser sensor cannot give negative values.
 * 3.  The probability of a laser sensor getting some reading
 *     sensorDistance becomes less probable as sensorDistance gets
 *     farther off from the "trueDistance"
 *
 * Thus we can focus on deriving the probability of getting a reading
 * for just one laser sensor, and then multiply all four together to
 * get the probability of getting all four readings.
 *
 * Note, this should get initialized once before being used.
 *
 * Note that Sensor Model used in the experiments is actually
 * LaserArrayAndBumpSensorModel which basically just combines
 * LaserArraySensorModel with the Bump Sensor model.
 */

class LaserArraySensorModel : public SensorModel
{
protected:
  double stddev;
  BeliefMap *bmap;
  LaserArraySensorReading lsr;

public:
  LaserArraySensorModel();
  virtual ~LaserArraySensorModel() {}

  void Initialize(BeliefMap *bm, double stddev);

  virtual void SetReadings(RobotState *robotState);
  virtual double GetPFromMapCoord(MapCoord mc);

protected:
  virtual double GetProbReadingGivenDistance(double sensor, double distance);
};

/* LaserArrayAndBumpSensorModel */
/* ----------------------- */
/* The LaserArrayAndBumpSensorModel basically just combines the
 * LaserArraySensorModel and the the BumpSensorModel by assuming that the
 * sensors are indepenedent from one another.  Thus the probability of
 * getting a readings <bump, laserdistances> is equal to the
 * probability of getting the reading <bump> times the probability of
 * getting the reading <laserdistances>.  This is pretty much how
 * LaserArrayAndBumpSensorModel is implemented.
 *
 * The individual sensor models need to get initialized before getting
 * passed into the Initialize method.
 *
 * CS221 Students: You can use this to package the BumpSensorModel
 * with your custom laser array sensor model. (actually, this is
 * already done for you)
 */ 

class LaserArrayAndBumpSensorModel : public SensorModel
{
protected:
  SensorModel *lsm;
  SensorModel *bsm;

public:
  LaserArrayAndBumpSensorModel();
  ~LaserArrayAndBumpSensorModel(){}

  void Initialize(SensorModel *lsm, SensorModel *bsm);
  virtual void SetReadings(RobotState *robotState);
  virtual double GetPFromMapCoord(MapCoord mc);
};

/* CustomLASModel */
/* -------------- */
/* CS221 Students: Currently this class is a copy of the
 * LaserArraySensorModel.  At the very least, you will need to change
 * the method "GetProbReadingGivenDistance".  You are welcome, however
 * to change anything else as well.  Remember, though, you will be
 * using this class with the BumpSensorModel by loading them both into
 * the LaserArrayAndBumpSensorModel class above (actually, this is
 * already done for you).
 */

/** CS221 TASK 5: BEGIN EDIT CODE **/

class CustomLASensorModel : public SensorModel
{
protected:
  double stddev;
  BeliefMap *bmap;
  LaserArraySensorReading lsr;

public:
  CustomLASensorModel();
  virtual ~CustomLASensorModel() {}

  void Initialize(BeliefMap *bm, double stddev);

  virtual void SetReadings(RobotState *robotState);
  virtual double GetPFromMapCoord(MapCoord mc);

protected:
  virtual double GetProbReadingGivenDistance(double sensor, double distance);
};

/** CS221 TASK 5: END EDIT CODE **/

#endif
