#include "robotconfig.hh"



///////////////
//RobotConfig//
///////////////

void RobotConfig::initNoFile(){
  int numSegments = 4;

  // set up jointangles, start, stop positions
  Point p(numSegments);
  goal = start = jointAngles = p;
  for (int i = 0; i < numSegments; i++)
    goal[i] = -M_PI_4 * (3.0/(float)numSegments);
  
  // create world
  world        = dWorldCreate();
  space        = dHashSpaceCreate(0);
  contactgroup = dJointGroupCreate(0);

  //environment
  ground       = dCreatePlane(space, 0, 0, 1, 0);

  groundBoxes.clear();
  dGeomID box;
  box = dCreateBox (space, .2, 5, 10);
  dGeomSetPosition(box, -.2, 0, 5);
  groundBoxes.push_back(box);

  box = dCreateBox (space, .1, .5, .5);
  dGeomSetPosition(box, 2, 0, 6);
  groundBoxes.push_back(box);
  
  // arms
  Segment s;
  segments.clear();
  for (int i = 0; i < numSegments; i++) {
    s.joint    = dCreateSphere(space, jointRadius);
    s.bar      = dCreateBox(space, length, width, height);
    s.length   = length;
    s.rotateX  = false;
    s.rotateY  = true;
    s.rotateZ  = false;

    segments.push_back(s);
  }

  arrangeGeoms(jointAngles);
}

void RobotConfig::printError(char* str){
  fprintf(stderr, "error parsing %s\n", str);
  exit(1);
}

void RobotConfig::load(const char* filename)
{
  if (filename == NULL) {
    initNoFile();
    return;
  }

  FILE* f;
  if ((f = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "Could not open config file %s\n", filename);
    exit(1);
  }

  // clear lists
  segments.clear();
  groundBoxes.clear();

  // create world
  world        = dWorldCreate();
  space        = dHashSpaceCreate(0);
  contactgroup = dJointGroupCreate(0);

  int numSegments;
  if (fscanf(f, "%d\n", &numSegments) != 1) printError("num segments");
  
  Segment s;
  int rotateX, rotateY, rotateZ;
  int dof = 0;
  for (int i = 0; i< numSegments; i++){
    if (fscanf(f, "%f %d %d %d\n", &s.length, &rotateX, &rotateY, &rotateZ) != 4) 
      printError("segments");
    dof += (rotateX + rotateY + rotateZ);

    //create segment
    s.joint    = dCreateSphere(space, jointRadius);
    s.bar      = dCreateBox(space, s.length, width, height);
    s.rotateX  = (rotateX == 1);
    s.rotateY  = (rotateY == 1);
    s.rotateZ  = (rotateZ == 1);
    
    segments.push_back(s);
  }

  //environment
  ground       = dCreatePlane(space, 0, 0, 1, 0);

  dGeomID box;
  box = dCreateBox (space, .2, 5, 10);
  dGeomSetPosition(box, -.2, 0, 5);
  groundBoxes.push_back(box);
  
  
  int numObstacles;
  if (fscanf(f, "%d\n", &numObstacles) != 1) printError("num obstacles");

  float x, y, z;
  float oLength, oWidth, oHeight;
  for (int i = 0; i < numObstacles; i++){
    if (fscanf(f, "%f %f %f %f %f %f\n", &oLength, &oWidth, &oHeight, &x, &y, &z) != 6) 
      printError("obstacles");
        
    box = dCreateBox (space, oLength, oWidth, oHeight);
    dGeomSetPosition(box, x, y, z);
    groundBoxes.push_back(box);
  }

  printf("%d \n", groundBoxes.size());
  //init goal, start and joint angles
  float angle;
  Point p(dof);
  start = goal = p;

  for (int i = 0; i<dof; i++){
    if (fscanf(f, "%f", &angle) != 1) printError("start pos");
    start[i] = angle;
  }
  while (!feof(f) && fgetc(f) != '\n');
  jointAngles = start;

  for (int i = 0; i<dof; i++){
    if (fscanf(f, "%f", &angle) != 1) printError("goal pos");
    goal[i] = angle;
  }
  while (!feof(f) && fgetc(f) != '\n');

  fclose(f);
  arrangeGeoms(jointAngles);
}


Point RobotConfig::getStart(){
  return start;
}

Point RobotConfig::getGoal(){
  return goal;
}

std::vector<Point> RobotConfig::getPoints(int numPoints) {
  std::vector<Point> points;
  Point p(jointAngles.size());

  printf("generating points\n");
  for (int i = 0; i<numPoints; i++) {
    for (unsigned int j = 0; j<jointAngles.size(); j++)
      p[j] = (2*M_PI * ((float)rand() / RAND_MAX) ) - M_PI;
    p.nIndex=i;
    

    
    if (isValid(p)) points.push_back(p);
    else i--;
  }
  printf("\ndone generating points\n");

  return points;
}

bool RobotConfig::isValid(Point p){
  assert(p.size() == jointAngles.size());
  bool didCollide = arrangeGeoms(p);
  arrangeGeoms(jointAngles);

  return !didCollide;
}

bool RobotConfig::checkLine(Point p1, Point p2){
  assert(p1.size() == p2.size() && p1.size() == jointAngles.size());

  bool didCollide   = arrangeGeoms(p1) || arrangeGeoms(p2);
  if (!didCollide) {
    float distance = p1.distanceTo(p2);
    if (incDistance < distance) {
      int numIncs = (int)floor(distance/incDistance);
      Point slope = (p1 - p2) * (1.0/(float)numIncs);
      Point temp  = p1;
      //printf("temp->size() %d   slope->size() %d\n", temp.size(), slope.size());
      for (int i = 0; i<numIncs-1 && !didCollide; i++) {
        temp += slope;
        didCollide = arrangeGeoms(temp);
      }
    }
  }
  arrangeGeoms(jointAngles);
  return !didCollide;
}


// this is called by dSpaceCollide when two objects in space are
// potentially colliding.
void RobotConfig::collide(void *data, dGeomID o1, dGeomID o2) {
  const int N = 10;
  int n = 0;
  dContact contact[N];

  RobotConfig *c = (RobotConfig*)data;
  
  //are any of the objects ground objects
  bool g1 = (o1 == c->ground);
  bool g2 = (o2 == c->ground);  
  for (unsigned int i = 0; i < c->groundBoxes.size(); i++){
    g1 = g1 || (o1 == c->groundBoxes[i]);
    g2 = g2 || (o2 == c->groundBoxes[i]);
  }

  //check for collisions with ground
  if (g1 ^ g2) {
    bool firstJoint = (o1 == c->segments[0].joint || o2 == c->segments[0].joint);
    n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
    
    if (!firstJoint && n > 0) {
      c->collideBool = true;
      //printf("collide world %d\n", n);
    }
  }

  //check for collisions with self
  else if (!(g1 && g2)){
    int  o2Index = -1, o1Index = -1;
    bool o1Joint = false, o2Joint = false;
   

    for (unsigned int i = 0; i< c->segments.size(); i++){
      //check object 1
      if (o1 == c->segments[i].bar || o1 == c->segments[i].joint) {
        o1Index = i;
        o1Joint = o1 == c->segments[i].joint;
      }

      //check object 2
      if (o2 == c->segments[i].bar || o2 == c->segments[i].joint) {
        o2Index = i;
        o2Joint = o2 == c->segments[i].joint;
      }
    }

    //hopefully no students see this :)
    if (o2Index == -1 || o1Index == -1){
      fprintf(stderr, "error in collide\n");
      return;
    }

    if (!(o1Joint && o2Joint)) {
      int indexDiff = o1Index - o2Index;
      //adjacent segments don't collide
      if (!(o1Joint || o2Joint) && indexDiff <= 1 && indexDiff >= -1) return;
      //joints don't collide with segments
      if (o1Joint && (indexDiff == 1 || indexDiff == 0)) return;
      if (o2Joint && (indexDiff == -1 || indexDiff == 0)) return;
    }
    
    n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
    if (n > 0) {
      //printf("collide self %d olJoint=%d o2Joint=%d o1Index=%d o2Index=%d\n", n, 
      //       o1Joint, o2Joint, o1Index, o2Index);
      c->collideBool = true;
    }
  }
}

bool RobotConfig::arrangeGeoms(Point angles) {
  assert(angles.size() == jointAngles.size());

  Transform t;
  float     x, y, z;
  dMatrix3  R;

  int angleIndex = 0;
  //initail translation
  t.translate(0, 0, startZ);
  for (unsigned int i = 0; i < segments.size(); i++) {    
    //find joint sphere position
    x = y = z = 0;
    t.transform(&x, &y, &z);
    dGeomSetPosition(segments[i].joint, x, y, z);

    //find segment box position
    if (segments[i].rotateZ) {
      t.rotateZ(angles[angleIndex]);
      angleIndex++;
    }
    if (segments[i].rotateY) {
      t.rotateY(angles[angleIndex]);
      angleIndex++;
    }
    if (segments[i].rotateX) {
      t.rotateX(angles[angleIndex]);
      angleIndex++;
    }

    x = (0.5)*(segments[i].length + 2*jointOffset);
    y = z = 0;
    t.transform(&x, &y, &z);
    dGeomSetPosition(segments[i].bar, x, y, z);

    //find segment box rotation
    t.getRotation(R);
    dGeomSetRotation(segments[i].bar, R);

    //translate to next segment
    t.translate(segments[i].length + 2*jointOffset, 0, 0);
  }

  this->collideBool = false;
  dSpaceCollide(space, this, &collide);
  return this->collideBool;
}
