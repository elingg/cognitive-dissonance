#include "robotsim.hh"
#include "transform.hh"


////////////
//RobotSim//
////////////

// start simulation - set viewpoint
void RobotSim::init() {
  static float xyz[3] = {4, -9, 7};
  static float ypr[3] = {90.0000f, 0, 0.0000f};
  dsSetViewpoint (xyz,ypr);
}

void RobotSim::initPath()
{
  if (agent.solution.size() > 1) {
    config.path        = agent.solution;
    config.jointAngles = config.path.front();
    config.path.pop_front();

    float distance   = config.jointAngles.distanceTo(config.path.front());
    config.pathSlope = (config.jointAngles - config.path.front()) * (config.incDistance/distance);
  }
}

// called when a key pressed
void RobotSim::command(int cmd)
{
  if (!config.path.empty()) {
    if (cmd == 'Q' || cmd == 'q') config.path.clear();
    return;
  }

  Point oldAngles = config.jointAngles;
  int maxIndex = config.jointAngles.size() - 1;
  
  switch(cmd){
  case '0': config.jointIndex = maxIndex > 0 ? 0 : maxIndex; break;
  case '1': config.jointIndex = maxIndex > 1 ? 1 : maxIndex; break;
  case '2': config.jointIndex = maxIndex > 2 ? 2 : maxIndex; break;
  case '3': config.jointIndex = maxIndex > 3 ? 3 : maxIndex; break;
  case '4': config.jointIndex = maxIndex > 4 ? 4 : maxIndex; break;
  case '5': config.jointIndex = maxIndex > 5 ? 5 : maxIndex; break;
  case '6': config.jointIndex = maxIndex > 6 ? 6 : maxIndex; break;
  case '7': config.jointIndex = maxIndex > 7 ? 7 : maxIndex; break;
  case '8': config.jointIndex = maxIndex > 8 ? 8 : maxIndex; break;
  case '9': config.jointIndex = maxIndex > 9 ? 9 : maxIndex; break;
  case '[':
    config.jointAngles[config.jointIndex] -= M_PI/18;
    break;
  case ']':
    config.jointAngles[config.jointIndex] += M_PI/18;
    break;
  case 'c':
  case 'C':
    config.armCollide = !config.armCollide;
    break;
  case 'E':
  case 'e':
    config.jointAngles = config.goal;
    break;
  case 'R':
  case 'r':
    config.jointAngles = config.start;
    break;
  case 'A':
  case 'a':
    agent.aStar();
    initPath();
    break;
  case 'U':
  case 'u':
    agent.uniformCost();
    initPath();
    break;
  case 'B':
  case 'b':
    agent.bestFirst();
    initPath();
    break;
  case 'I':
  case 'i':
    agent.aStarInadmissible();
    initPath();
    break;
  case 'P':
  case 'p':
    initPath();
  }

  if (config.armCollide && config.arrangeGeoms(config.jointAngles)) {
    config.jointAngles = oldAngles;
  }
  config.arrangeGeoms(config.jointAngles);
}

// simulation loop
void RobotSim::simLoop(int pause) {
  if (!config.path.empty()) {
    if (config.jointAngles.distanceTo(config.path.front()) < config.incDistance) {
      config.jointAngles = config.path.front();
      config.path.pop_front();
      if (!config.path.empty()){
        float distance   = config.jointAngles.distanceTo(config.path.front());
        config.pathSlope = (config.jointAngles - config.path.front()) * (config.incDistance/distance);
      }
    }
    else config.jointAngles += config.pathSlope;
    usleep(10000);
    
    config.arrangeGeoms(config.jointAngles);
  }
  
  

  //draw joints
  dsSetColor(1, 0, 0);
  dsSetTexture(DS_NONE);
  for (unsigned int i = 0; i< config.segments.size(); i++) 
    dsDrawSphere(dGeomGetPosition(config.segments[i].joint), 
                 dGeomGetRotation(config.segments[i].joint), RobotConfig::jointRadius);
  
  //draw segments
  dVector3 ss;
  dsSetColor(0,1,1);
  dsSetTexture(DS_WOOD);
  for (unsigned int i = 0; i < config.segments.size(); i++) {
    dGeomBoxGetLengths(config.segments[i].bar, ss);
    dsDrawBox(dGeomGetPosition(config.segments[i].bar), 
              dGeomGetRotation(config.segments[i].bar), ss);
  }


  //draw ground boxes
  for (unsigned int i = 0; i < config.groundBoxes.size(); i++) {
    if (i==0)
      dsSetColor(0,0,1);
    else
      dsSetColor(1,1,0);

    dGeomBoxGetLengths(config.groundBoxes[i],ss);
    dsDrawBox(dGeomGetPosition(config.groundBoxes[i]),
              dGeomGetRotation(config.groundBoxes[i]),ss);
  }
}

void RobotSim::run(int argc, char **argv)
{
  // setup pointers to drawstuff callback functions
  dsFunctions fn;
  fn.version = DS_VERSION;
  fn.start   = &init;
  fn.step    = &simLoop;
  fn.command = &command;
  fn.stop    = 0;
  fn.path_to_textures = "/afs/ir/class/cs221/ode-0.5/drawstuff/textures";

  // run simulation
  dsSimulationLoop (argc,argv,500,400,&fn);
}

