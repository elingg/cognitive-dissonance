#ifndef _ROBOT_H
#define _ROBOT_H

#include <vector>
#include "Maps.h"
using namespace std;

/* CS 221 Students */
/* --------------- */
/* Not all of the information in the file is relevent to what you are
 * doing.  Probably the only thing you'd need to know about in this
 * file is the GetInitialPosition() method in the RobotSimulation
 * class.
 */

// an action is basically just a change in coordinates...
typedef MapCoord Action;

/* LaserArraySensorReading */
/* overglorified array of four doubles representing the distances
 * that the laser rangefinder found at a given position.
 */

typedef struct LaserArraySensorReading
{
  double dist[NUM_DIRECTIONS];
} LaserArraySensorReading;

/* LogEntry */
/* Stores exactly what the log file gave us... which is the dead
 * reckoning position, the 'true position', and the laser readings
 */
typedef struct LogEntry
{
  Point ptDeadReckon;
  Point ptActual;
  LaserArraySensorReading lsr;
} LogEntry;

/* RobotState */
/* basically stores the action that a robot is going to take, and the
 * sensor readings it recorded at that time.
 */
typedef struct RobotState
{
  Point deltaPos;
  Action action;
  Point ptActual;
  Point ptDeadReckon;
  LaserArraySensorReading lsr;
} RobotState;

/* RobotSimulation */
/* --------------- */
/* The robot simulation class basically is where the history of the
 * robot is stored.  The class is responsible for reading in the log
 * file.  It directly dumps all of that information into the fullLog
 * vector.  We then process this log and place a more useful version
 * of it inside of the states vector.
 * 
 * CS 221 students: The only thing you might need from the
 * RobotSimulation class is the GetInitialPosition() method, which
 * returns the map coordinates of where the robot starts out.
 *
 * It would also be good to understand that these robot "states"
 * basically store the action the robot thinks it is taking, and the
 * sensor readings that it recorded at that time.
 * 
 * You don't need to read the rest of the comments about processing... 
 * go code :)
 * 
 * In this processing, two things happen:
 * 1.  We only pull every 'logEntriesPerState' number of log entries to
 *     generate one state.  We do this because there really are too many
 *     log entries for the robot's history, and each are taken at very 
 *     very small time steps. 
 * 2.  We calculate an "action" that the robot thinks it is taking to get
 *     from one state to the next.  To do this, we simply look at two
 *     states' positions, see what grid cells that would correspond to 
 *     inside of a belief map, and find the delta x and delta y that gives
 *     that takes the robot from one state to the next.
 */

class RobotSimulation
{
protected:
  vector<LogEntry*> fullLog;
  MapCoord initialPosition;
public:

  RobotSimulation(){}
  vector<RobotState*> states;

  bool ReadLogFile(const char *filename);
  bool GenerateStates(int logEntriesPerState, BeliefMap *bmap);

  MapCoord GetInitialPosition() const { return initialPosition; };
};

#endif
