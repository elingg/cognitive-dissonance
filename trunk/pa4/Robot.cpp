#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "Robot.h"

#define C_BUF   100 // size of buffer

/* =================================================================== */
/*                         RobotSimulation                             */
/* =================================================================== */

/* ReadLogFile */
/* ----------- */
/* Reads the given log file.  Note: We require that there be no '\n'
 * on the final line.  Yes... it seems hacky (at the very least its
 * not robust)...  but it works just fine for now.
 */

bool RobotSimulation::ReadLogFile(const char *filename)
{
  ifstream ifp;
  LogEntry *entry;

  ifp.open(filename, ios::in);

  if (!ifp) 
  {
    cerr << "Failure - Cannot open " << filename << endl;
    goto FAILURE;
  }

  while(!ifp.eof())
  {
    entry = new LogEntry;
    if (!entry)
    {
      cerr << "Failure - Ran out of memory" << endl;
      goto FAILURE;
    }
    ifp >> entry->ptDeadReckon.x >> entry->ptDeadReckon.y
        >> entry->lsr.dist[EAST] >> entry->lsr.dist[NORTH]
        >> entry->lsr.dist[WEST] >> entry->lsr.dist[SOUTH]
        >> entry->ptActual.x >> entry->ptActual.y;

    // it would be nice if i knew how to get ifp to just eat up whitespace
    // and newlines at this point.

    fullLog.push_back(entry);
  }

  return true;
FAILURE:
  return false;
}

/* SetStateFromLogEntries */
/* ---------------------- */
/* Sets a robot state for a log given entry.  Needs the other stuff (previous log entry, 
 * deadReckonToTruth, and the bmap so that we can figure out what movement brought the 
 * robot to this state.  
 */
void SetStateFromLogEntries(RobotState *state, LogEntry *prevEntry, LogEntry *curEntry, Point deadReckonToTruth, BeliefMap *bmap)
{
  int i;
  MapCoord prevDeadReckonCoord;
  MapCoord curDeadReckonCoord;

  // note(jjt): may have to switch these from dead reckoning delta positions to
  // actual positions + noise if things don't work correctly.
  // on second thought, i don't think we need this.
  state->deltaPos = curEntry->ptDeadReckon - prevEntry->ptDeadReckon;

  // note(jjt): it may be better to use the current entrie's actual point to figure out
  // this stuff instead of the deadReckonToTruth...  maybe...

  // find out where on the grid the deadreckoning thinks it is.  subtract the coordinates of
  // the grid positions to get the action (i.e. cell displacement expected)... which is what 
  // we use for the movement update.
  prevDeadReckonCoord = bmap->GetMapCoordFromMeters(prevEntry->ptDeadReckon + deadReckonToTruth);
  curDeadReckonCoord = bmap->GetMapCoordFromMeters(curEntry->ptDeadReckon + deadReckonToTruth); 
  state->action = curDeadReckonCoord - prevDeadReckonCoord;
 
  // set the laser range finder readings.
  for(i = 0; i < NUM_DIRECTIONS; i++)
    state->lsr.dist[i] = curEntry->lsr.dist[i];

  // track the actual position of the robot at this time...
  state->ptActual = curEntry->ptActual;
  // track the position the robot thinks its in at this time...
  state->ptDeadReckon = curEntry->ptDeadReckon + deadReckonToTruth;
}

/* GenerateStates */
/* -------------- */
/* Convert the LogEntries into RobotStates.  Skip over
 * logEntriesPerState log entries to generate a single RobotState.
 */
bool RobotSimulation::GenerateStates(int logEntriesPerState, BeliefMap *bmap)
{
  LogEntry *prevEntry, *curEntry;
  Point deadReckonToTruth;
  RobotState *state;
  vector<LogEntry*>::iterator it;

  assert(logEntriesPerState > 0);
  assert(fullLog.size() > 0);

  prevEntry = fullLog.at(0);

  initialPosition = bmap->GetMapCoordFromMeters(prevEntry->ptActual);

  // use this to find where on the discrete map the robot thinks it is, assuming
  // that it knew its location in the beginning.
  deadReckonToTruth = prevEntry->ptActual - prevEntry->ptDeadReckon;

  for(it = fullLog.begin(); it < fullLog.end(); it += logEntriesPerState)
  {
    curEntry = (*it);
    state = new RobotState;
    if(!state)
    {
      cerr << "Failure - Ran out of memory" << endl;
      goto FAILURE;
    }
    SetStateFromLogEntries(state, prevEntry, curEntry, deadReckonToTruth, bmap);
    states.push_back(state);
    prevEntry = curEntry;
  }

  return true;

FAILURE:
  return false;
}
