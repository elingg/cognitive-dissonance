#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "Robot.h"
#include "Maps.h"
#include "ActionModel.h"

#define C_BUF   100 // size of buffer


void NormalizeActionGrid(ActionGrid *grid);
/* =================================================================== */
/*                           ActionModel                               */
/* =================================================================== */

bool SimpleActionModel::InitTemplates()
{
  templates[0] = new ActionGrid[1];
  templates[1] = new ActionGrid[2];
  templates[2] = new ActionGrid[3];
  templates[3] = new ActionGrid[4];

  if (templates[0] == NULL || 
      templates[1] == NULL || 
      templates[2] == NULL || 
      templates[3] == NULL)
  {
    CleanUpTemplates();
    return false;
  }
  return true;
}

void SimpleActionModel::CleanUpTemplates()
{
  int i;
  for(i = 0; i < 4; i++)
    if(templates[i])
      delete templates[i];
}

SimpleActionModel::SimpleActionModel()
{
  templates[0] = NULL;
  templates[1] = NULL;
  templates[2] = NULL;
  templates[3] = NULL;
}

SimpleActionModel::~SimpleActionModel()
{
  CleanUpTemplates();
}

bool SimpleActionModel::ReadFromFile(const char *filename)
{
  ifstream ifp;

  if (!InitTemplates())
  {
    cerr << "Failure - Cannot allocate memory" << endl;
    goto FAILURE;
  }

  ifp.open(filename, ios::in);

  if (!ifp) 
  {
    cerr << "Failure - Cannot open " << filename << endl;
    goto FAILURE;
  }

  while(!ifp.eof())
  {
    int x, y;
    ifp >> x >> y;
    ifp >> templates[x][y].p[0][0] >> templates[x][y].p[1][0] >> templates[x][y].p[2][0]
	>> templates[x][y].p[0][1] >> templates[x][y].p[1][1] >> templates[x][y].p[2][1]
	>> templates[x][y].p[0][2] >> templates[x][y].p[1][2] >> templates[x][y].p[2][2];
    // it would be nice if i knew how to get ifp to just eat up whitespace
    // and newlines at this point.
    NormalizeActionGrid(&(templates[x][y]));
  }

  return true;
FAILURE:
  CleanUpTemplates();
  return false;
}

void NormalizeActionGrid(ActionGrid *grid)
{
  int i, j;
  double total = 0.0;

  for(i = 0; i < 3; i++)
    for(j = 0; j < 3; j++)
      total += grid->p[i][j];

  for(i = 0; i < 3; i++)
    for(j = 0; j < 3; j++)
      grid->p[i][j] = grid->p[i][j] / total;
}

void SimpleActionModel::SetAction(RobotState *robotState)
{
  int x, y;
  bool flipXY;

  action = robotState->action;

  assert(abs(action.x) < 4 && abs(action.y) < 4);

  if(abs(action.x) > abs(action.y))
  {
    flipXY = false;
    x = action.x;
    y = action.y;
  } else {
    flipXY = true;
    x = action.y;
    y = action.x;
  }

  LoadActionGrid(x, y, flipXY);
}

// LoadActionGrid
// --------------
// Load the template action grid into the current grid, applying any
// necessary transformations.

void SimpleActionModel::LoadActionGrid(int x, int y, bool flipXY)
{
  double temp;
  int i, j;
  int absX = abs(x);
  int absY = abs(y);

  // need to flip across x = y
  if (!flipXY)
  {
    for(i = 0; i < 3; i++)
      for(j = 0; j < 3; j++)
        curGrid.p[i][j] = templates[absX][absY].p[i][j];
  } else {
    for(i = 0; i < 3; i++)
      for(j = 0; j < 3; j++)
        curGrid.p[i][j] = templates[absX][absY].p[j][i];
    //        curGrid.p[i][j] = templates[absX][absY].p[2-j][2-i];
  }

  // need to flip across x-axis
  if (x < 0)
    for(j = 0; j < 3; j++)
    {
      temp = curGrid.p[0][j];
      curGrid.p[0][j] = curGrid.p[2][j];
      curGrid.p[2][j] = temp;
    }

  // need to flip across y-axis
  if (y < 0)
    for(i = 0; i < 3; i++)
    {
      temp = curGrid.p[i][0];
      curGrid.p[i][0] = curGrid.p[i][2];
      curGrid.p[i][2] = temp;
    }
}

void SimpleActionModel::SetCurrentPosition(MapCoord cp)
{
  curPos = cp;
}

int SimpleActionModel::GetNumNewLocations() const
{
  return 9;
}

MapCoord SimpleActionModel::GetNewLocCoord(int i)
{
  int x,y;
  MapCoord delta;
  assert(i >= 0 && i < 9);

  x = i / 3;
  y = i - x * 3;

  delta.x = x - 1;
  delta.y = y - 1;

  return action + curPos + delta;
}

double SimpleActionModel::GetProbNewLocGivenAction(int i)
{
  int x, y;
  assert(i >= 0 && i < 9);

  x = i / 3;
  y = i - x * 3;

  return curGrid.p[x][y];
}


RandomActionModel::RandomActionModel(){}
void RandomActionModel::SetAction(RobotState *robotState){}
void RandomActionModel::SetCurrentPosition(MapCoord cp)
{
  curPos = cp;
}

int RandomActionModel::GetNumNewLocations() const
{
  return(9);
}
MapCoord RandomActionModel::GetNewLocCoord(int i)
{
  int x,y;
  MapCoord delta;
  x = i / 3;
  y = i - x * 3;
  delta.x = x - 1;
  delta.y = y - 1;
  return curPos + delta;
}
double RandomActionModel::GetProbNewLocGivenAction(int i)
{
  return (1.0 / GetNumNewLocations()); 
}
