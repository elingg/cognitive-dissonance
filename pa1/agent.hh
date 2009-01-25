#ifndef AGENT_HH
#define AGENT_HH

#include <unistd.h>
#include <assert.h>
#include <math.h>
#include <list>
#include <vector>
#include "robotconfig.hh"
#include "priorityqueue.hh"


using namespace std;

/////////
//Agent//
/////////

// This is the class that performs all the search for the robotic arm.
// You may add functions and variables as you wish as long as you keep
// the ones that are currently listed.
class Algo;

class Agent {
public: 
  // Constructors and destructors
  Agent() {};
  ~Agent() {};
  
  // Returns the positions of all the points in the configuration 
  // space we are working with.
  std::vector<Point> getPoints();

  // Store your solution after every search in this list of points.  The first
  // element should be the start position of your robot, the final element
  // should be the goal position, and all intermediate elements are the
  // intermediate points listed in order.
  std::list<Point> solution;

  void init(RobotConfig c, int numPoints);

  // Print the results in a standard format 
  // We will be using this for our automatic grading
  void printResults();

  // Function to run all the different searches for our test
  // We will be using this for our automatic grading
  void runTests();
  
  // Functions to perform all our searches
  // Complete descriptions are in agent.cc
  void bestFirst();
  void uniformCost();
  void aStar();
  void aStarInadmissible();

  Point getGoal() { return goal; }
private:

  void genericAlgo(Algo& algo);
  
  // Private variables for the configuration space, the robot start position, 
  // the goal position, and the randomly generated points from the 
  // configuration space.
  RobotConfig cs;
  Point start;
  Point goal;
  std::vector<Point> points;
};

class Algo { 
public: 
  virtual ~Algo() {}; 
  virtual float getPathCost() = 0; 
  virtual float getHeuristicCost(Agent& agent, Point& p) = 0;   
}; 
 
class BestFirstAlgo : public Algo { 
public: 
  float getPathCost() { return 0; } 
  float getHeuristicCost(Agent& agent, Point& p) { 
          return p.distanceTo(agent.getGoal()); 
  } 
}; 


#endif //AGENT_HH

