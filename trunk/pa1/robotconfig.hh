#ifndef ROBOTCONFIG_HH
#define ROBOTCONFIG_HH

#include <ode/ode.h>
#include "transform.hh"
#include "configspace.hh"

class Segment {
public:
  Segment() {};
  ~Segment() {};
  
  float   length;
  bool    rotateX;
  bool    rotateY;
  bool    rotateZ;
  dGeomID bar;
  dGeomID joint;
};

class RobotConfig : public ConfigSpace {
public:
  // arm constants
  static const float length      = .8; 
  static const float width       = .2; 
  static const float height      = .2;
  static const float jointOffset = .12;
  static const float jointRadius = .25;
  static const float startZ      = 5;
  static const float incDistance = .1;

  //arm
  std::vector<Segment>  segments;
  
  //static obstacles
  dGeomID ground;
  std::vector<dGeomID> groundBoxes;

  //constructors 
  RobotConfig() : armCollide(false), jointIndex(0) {};
  ~RobotConfig() {};

  //initialization function
  void load(const char* filename);

  //planned path
  std::list<Point> path;
  Point            pathSlope;
  
  // arm info
  bool     armCollide;
  int      jointIndex;
  Point    jointAngles;
  Point    start, goal;  
  
  //config space functions
  Point getStart();
  Point getGoal();
  std::vector<Point> getPoints(int i);
  bool checkLine(Point p1, Point p2);
  bool isValid(Point p);
  
  //point manupilation functions
  bool arrangeGeoms(Point angles);

private:
  //print error when loading file
  void printError(char *str);

  //default config
  void initNoFile();

  //the space and world ect
  dWorldID world;
  dSpaceID space;
  dJointGroupID contactgroup;
  
  // hack for collision
  bool collideBool;
  static void collide(void *data, dGeomID o1, dGeomID o2);
};


#endif
