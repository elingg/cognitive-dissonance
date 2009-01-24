#ifndef ROBOTSIM_HH
#define ROBOTSIM_HH


#include <unistd.h>
#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include "agent.hh"
#include "robotconfig.hh"


// select correct drawing functions
#ifdef dDOUBLE
  #define dsDrawBox dsDrawBoxD
  #define dsDrawSphere dsDrawSphereD
  #define dsDrawCylinder dsDrawCylinderD
  #define dsDrawCappedCylinder dsDrawCappedCylinderD
#endif


extern RobotConfig config;
extern Agent       agent;

class RobotSim {
public:
  RobotSim() {};
  ~RobotSim() {};
  void run(int argc, char** argc);
  
private:
  
  static void init();
  static void initPath();
  static void simLoop(int pause);
  static void command(int cmd);
};


#endif //ROBOTSIM_HH
