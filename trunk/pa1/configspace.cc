#include "configspace.hh"


/////////
//Point//
/////////

//constructors and destructors
Point::Point() { createPoint(2); }
Point::Point(int size) { createPoint(size); }

//private functions
inline void Point::createPoint(int size) {
  assert(size > 0);
  nIndex=-1;
  this->resize(size);

  for (int i = 0; i<size; i++) 
    (*this)[i] = 0;
}

//distanace function
float Point::distanceTo(Point p) { 
  assert(p.size() == this->size());
  
  float distSquared = 0;
  for (unsigned int i = 0; i < this->size(); i++)
    distSquared += ((*this)[i] - p[i]) * ((*this)[i] - p[i]);
  
  return sqrt(distSquared);
}



///////////////
//ConfigSpace//
///////////////


Point ConfigSpace::getStart() {
  Point p;
  return p;
}

Point ConfigSpace::getGoal() {
  Point p;
  p[0] = 10;
  p[1] = 10;

  return p;
}

std::vector<Point> ConfigSpace::getPoints(int numPoints) {
  std::vector<Point> points;
  Point p;

  for (int i = 0; i<numPoints; i++) {
    p[0] = 10 * ((float)rand() / RAND_MAX);
    p[1] = 10 * ((float)rand() / RAND_MAX);
    p.nIndex=i;
    
    if (isValid(p))
      points.push_back(p);
    else
      i--;
  }

  return points;
}

bool ConfigSpace::checkLine(Point p1, Point p2) {
  return true;
}

bool ConfigSpace::isValid(Point p) {
  return true;
}

