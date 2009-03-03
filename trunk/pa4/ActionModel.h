#ifndef _ACTIONMODEL_H
#define _ACTIONMODEL_H

// Forward Declarations
class RobotState;
class MapCoord;

/* CS221 Students: It's important to read the following description of
 * the ActionModel
 */

/* ActionModel */
/* ----------- */
/* The action model is essentially a successor function that returns probabilites
 * associated with a location.  Remember, the action model basically gives us
 * P(s' | s, a)... the probability that the robot will end up in s' = (x',y') 
 * given that it started at s = (x,y) and tried to do action a.
 *
 * To use any action model, 
 * 1. Call SetAction to set the action
 * 2. Call SetCurrentPosition to set (x,y) (the position the robot 'was' in 
 *    before taking the action)
 * 3. Of course, since the robot does not perform the action a perfectly, it may 
 *    end up in many possible positions, so there are many possible (x',y') 
 *    coordinates.  So use GetNumNewLocations to find out how many there are.  
 *    You can then access the ith (x',y') and the its associated probability by 
 *    using GetNewLocCoord and GetProbNewLocGivenAction.
 */

class ActionModel
{
public:
  virtual ~ActionModel() {}
  virtual void SetAction(RobotState *robotState) = 0;
  virtual void SetCurrentPosition(MapCoord cp) = 0;
  virtual int GetNumNewLocations() const = 0;
  virtual MapCoord GetNewLocCoord(int i) = 0;
  virtual double GetProbNewLocGivenAction(int i) = 0;
};

/* -------------------------------- */
/* Implementations of Action Models */
/* -------------------------------- */

/* ActionGrid */
/* ---------- */
/* Used to store probabilities for a 3x3 grid of cells.  Used by the
 * SimpleActionModel.
 */
typedef struct
{
  double p[3][3];
} ActionGrid;


/* SimpleActionModel */
/* ----------------- */
class SimpleActionModel : public ActionModel
{
private:
  ActionGrid *templates[4];
  ActionGrid curGrid;
  Action action;
  MapCoord curPos;
  bool InitTemplates();
  void CleanUpTemplates();
  void LoadActionGrid(int x, int y, bool flipXY);
public:
  SimpleActionModel();
  virtual ~SimpleActionModel();
  bool ReadFromFile(const char *filename);
  virtual void SetAction(RobotState *robotState);
  virtual void SetCurrentPosition(MapCoord cp);
  virtual int GetNumNewLocations() const;
  virtual MapCoord GetNewLocCoord(int i);
  virtual double GetProbNewLocGivenAction(int i);
};

/* RandomActionModel */
/* ----------------- */
class RandomActionModel : public ActionModel
{
  MapCoord curPos;
public:
  RandomActionModel();
  virtual ~RandomActionModel(){}
  virtual void SetAction(RobotState *robotState);
  virtual void SetCurrentPosition(MapCoord cp);
  virtual int GetNumNewLocations() const;
  virtual MapCoord GetNewLocCoord(int i);
  virtual double GetProbNewLocGivenAction(int i);
};

#endif
