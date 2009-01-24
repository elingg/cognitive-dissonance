#ifndef CONFIGSPACE_HH
#define CONFIGSPACE_HH

#include <unistd.h>
#include <assert.h>
#include <math.h>
#include <list>
#include <vector>

using namespace std;

class Point : public std::vector<float>{
public:
  //a global index for each point
  int nIndex;

  // constructors and destructors
  Point();
  Point(int size);
  ~Point() {};
  
  // distance function
  float  distanceTo(Point p);

  void print(){
    printf("(");
    for (unsigned int i = 0; i<this->size()-1; i++)
      printf("%f, ", (*this)[i]);
    printf("%f)", (*this)[this->size()-1]);
  }

  Point operator-(const Point &p) const {
    assert(p.size() == this->size());

    Point t(this->size());    
    for (unsigned int i = 0; i<p.size();i++)
      t[i] = (*this)[i] - p[i];    
    return t;
  }

  Point operator+(const Point &p) const {
    assert(p.size() == this->size());

    Point t(this->size());    
    for (unsigned int i = 0; i<p.size();i++)
      t[i] = (*this)[i] + p[i];    
    return t;
  }
  
  Point operator*(float s) const {
    Point t(this->size());    
    for (unsigned int i = 0; i<this->size();i++)
      t[i] = (*this)[i] * s;
    return t;
  }
  
  void operator-=(const Point &p) {
    assert(p.size() == this->size());
    for (unsigned int i = 0; i<p.size();i++)
      (*this)[i] -= p[i];
  }
  
  void operator+=(const Point &p) {
    assert(p.size() == this->size());
    for (unsigned int i = 0; i<p.size();i++)
      (*this)[i] -= p[i];
  }                                                                             

  friend bool operator==(const Point & p1, const Point &p2) {
    assert(p1.size() == p2.size());
    for (unsigned int i = 0; i<p1.size();i++)
      if(p1[i] != p2[i])
        return false;
    return true;
  }                                                                             
private:
  inline void createPoint(int size);  
};

class ConfigSpace {
public:
  ConfigSpace() {};
  virtual ~ConfigSpace() {};

  virtual Point getStart();
  virtual Point getGoal();
  virtual std::vector<Point> getPoints(int numPoints);
  virtual bool checkLine(Point p1, Point p2);
  virtual bool isValid(Point p);
};

#endif //CONFIGSPACE_HH
