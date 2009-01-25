#include "agent.hh"
#include <algorithm>
#include <iostream>
#include <map>
#include <set>

/////////
//Agent//
/////////

// Initialize
// ----------
// This function must be called before the agent can be run.
// The configuration space c is passed in, which then
// generates the start position, goal position, and numPoints
// random points.

void Agent::init(RobotConfig c, int numPoints) {
  this->start = c.getStart();
  this->goal = c.getGoal();
  this->cs = c;
  this->solution.clear();
  this->points=cs.getPoints(numPoints);
  this->points.push_back(this->goal);
  this->points.push_back(this->start); // note start node's index is size-1
}

// Accessors
std::vector<Point> Agent::getPoints() { return points; }

// Search functions
// ----------------
// Your robotic arm has a list of all the randomly generated points
// in "points."  From this list of points, you must first generate
// a roadmap of all valid paths between points.  Then you will perform
// the following searches on the roadmap.
//
// Because we are being particular about the ways ties are broken in
// the priority queue class (ties should be broken by order in which a
// node was added to the queue), it is important that you add successors
// to the queue in a set order.  In particular, please add successors to
// the queue based upon the order that their associated point  occurs in 
// the vector returned by cs.getPoints().  Note that a simple in-order 
// traversal of the vector will produce this result.
//
// In all the search functions, we will be using the Euclidean
// distance for any cost and heuristic measures.  All points in the 
// configuration space will be of type Point, which has a built
// in p1.distanceTo(p2) that will return a float with the 
// Euclidean distance between points p1 and p2.  Remember that all
// these points are in the configuration space and not in the 
// work space.  Therefore, the "shortest distance" between two
// points in the configuration space simply means the path that
// causes the robotic arm to move its joints the least, not the
// path that will cause the "hand" (end effector) of the robot
// to move the shortest distance.  Keep this in mind when you see
// the visualizations of the results of the various searches.
//
// In all your searches you must keep track of how many points
// have been checked in "nodesChecked," which must be printed 
// as shown below at the end of the search. The very first point 
// checked must be the start node and the very final point 
// must be the goal node.  In order for your "nodesChecked"
// to agree with our examples and what we will be testing you on,
// as soon as you remove a node from the fringe (we recommend using
// "priorityqueue.hh" to store your points), you add 1 to nodesChecked 
// and perform a goal check.  If the current node is the goal node, 
// do not expand any more nodes.  Else, add the current nodes 
// successors to the fringe.  It is incorrect to perform the goal 
// test on the nodes as you are adding them to the fringe.
//
// At the end of the search, all the points in the solution path
// must appear in "solution", with the first element being the 
// start node and the final element the goal node.
//
// It is very important that in the final version of your code,
// the only things printed are those that we have in this initial 
// version of the code.  We will be using an automatic script to 
// check your code which must print how many nodes are checked and 
// call "printResults()" if a solution is found.


// THE FOLLOWING FUNCTION IS PROVIDED FOR YOU IN ROBOTCONFIG.HH
// ------------------------------------------------------------
// bool checkLine(Point p1, Point p2)
//
// This function returns true if there is a straight line path
// connecting p1 and p2 in free space.  Use this to construct your
// roadmap.

// Best First Search
// -----------------
// The best first seach checks the node on the fringe that is the 
// closest to the goal.  It is ok if this node is farther from the 
// goal than the current point.

void printPath(std::list<Point>& path) {
  for(std::list<Point>::iterator it=path.begin(); it!=path.end(); ++it) {
    it->print(); std::cerr << std::endl;
  }
}

void printNode(QueueNode& n) {
  std::cerr << "Point: " << n.pointIndex << ", cost: " << n.heuristicCost << ", order: " << n.orderNumber << endl;
  std::cerr << "Path: "; printPath(n.path); std::cerr << std::endl;  
}

void Agent::bestFirst() {
  BestFirstAlgo algo;
  genericAlgo(algo); 
}

void Agent::genericAlgo(Algo& algo) {
  int nodesChecked=0;
  bool pathFound=false;
  std::list<Point> path;

  solution.clear();
  
  // ADD YOUR SEARCH HERE
  
  // std::cerr << "Start: "; this->start.print(); std::cerr << std::endl;
  // std::cerr << "Goal: "; this->goal.print(); std::cerr << std::endl;
  // for(size_t i=0; i<this->points.size(); ++i) {
    // std::cerr << "[" << i << "]: ";
    // this->points[i].print();
    // std::cerr << std::endl;
  // }  
  // create road map...
  std::map<size_t, std::set<size_t> > roadMap;
  for(size_t i=0; i<this->points.size(); ++i) {
    roadMap[i] = std::set<size_t>();
  }
  //  std::cerr << "Neighbors:\n";
  for(size_t i=0; i<this->points.size(); ++i) {
    // std::cerr << "[" << i << "]: [";
    std::set<size_t>& neighbors = roadMap[i];
    for(size_t j=0; j<this->points.size(); ++j) {
      if(i==j) continue; 
      // skip if already neighbor, distance computation may take time... 
      if(neighbors.find(j)==neighbors.end()) {  // if not neighbor, check and add....
        if(this->cs.checkLine(this->points[i],this->points[j])) { // check
          neighbors.insert(j); // add
          roadMap[j].insert(i); // mutual neighbors, so update.
        }
      }
    }
    // for(std::set<size_t>::const_iterator nit=neighbors.begin();
    //    nit!=neighbors.end(); ++nit) {
      //std::cerr << *nit << " (" << this->points[*nit].distanceTo(this->goal) << "), ";
    // }
    // std::cerr << "]" << std::endl; 
  }
  std::set<size_t> expandedNodes;
       
  // add initial start node...
  // (note convention: all paths include the point as well, so end of path is the point)
  PriorityQueue pq; 
  path.push_back(this->start);
  int order=0;
  // note we added the start node to the vector of points in init and so the index of it is the end
  // of the vector...
  QueueNode startNode(algo.getPathCost(),
           algo.getHeuristicCost(path,*this,this->start),
           path,this->points.size()-1,order++);
  //std::cerr << "Adding start node:\n";
  //printNode(startNode);
  pq.push(startNode);
  // .. added initial start node.  
  
  while(!pq.empty()) {
    QueueNode currNode = pq.top();
    pq.pop();
    if(expandedNodes.find(currNode.pointIndex)!=expandedNodes.end()) {
      continue;
    }
    nodesChecked++;
    size_t pointIndex = currNode.pointIndex;
    Point currPoint = currNode.path.back(); 
    // basic sanity check...
    if(this->points[pointIndex]!=currPoint) {
      // std::cerr << "Last point in path and point-index don't match\n";
      exit(-1);
    }
    expandedNodes.insert(currNode.pointIndex);
    // std::cerr << "Expanding [" << pointIndex << "]: "; currPoint.print(); std::cerr << ", cost: " << currNode.heuristicCost << std::endl;
    if(currPoint == this->goal) {
      // std::cerr << "Found goal: "; currPoint.print(); std::cerr << std::endl;
      pathFound = true;
      path = currNode.path;
      break;
    }
    // else go through neighbors...
    std::set<size_t>& neighbors = roadMap[pointIndex];
    for(std::set<size_t>::iterator nit=neighbors.begin();
        nit!=neighbors.end(); ++nit) {
      // if point not already in path to currPoint...
      Point& pt = this->points[*nit];
      // std::cerr << "\tExamining: " << *nit << std::endl;
      if(std::find(currNode.path.begin(),currNode.path.end(),pt)!=
          currNode.path.end()) {
        // std::cerr << "\t\tPoint already in path, skipping\n";
        continue;
      }
      std::list<Point> npath = currNode.path;
      npath.push_back(pt); 
      //QueueNode newNode(0,pt.distanceTo(this->goal),npath,*nit,order++);
       QueueNode newNode(0, algo.getHeuristicCost(npath,*this,pt), npath, *nit, order++);
      // std::cerr << "\tAdding node: \n";
      //printNode(newNode);
      pq.push(newNode);
    }
  }

  if (pathFound) {
    printf("Found path after checking %d nodes\n",nodesChecked);
    solution=path;
    printResults();
  }
  else {
    printf("NO PATH FOUND. Checked %d nodes.\n",nodesChecked);
    solution.clear();
  }
}

// Uniform Cost Search
// -------------------
// The uniform cost search checks the node on its fringe that has
// the shortest path from the start node without checking to
// see how far this node is from its goal.

void Agent::uniformCost() {
  int nodesChecked=0;
  bool pathFound=false;
  std::list<Point> path;

  solution.clear();

  // ADD YOUR SEARCH HERE
   UniformCostAlgo algo;
  genericAlgo(algo);

  if (pathFound) {
    printf("Found path after checking %d nodes\n",nodesChecked);
    solution=path;
    printResults();
  }
  else {
    printf("NO PATH FOUND. Checked %d nodes.\n",nodesChecked);
    solution.clear();
  }
}

// A-star
// ------
// A-star checks the node on its fringe with the shortest expected
// path to the goal.  The expected cost should be the actual
// cost to get from the start node to the current node plus the
// Euclidean distance from the current node to the goal node.

void Agent::aStar() {
  int nodesChecked=0;
  bool pathFound=false;
  std::list<Point> path;

  solution.clear();

  // ADD YOUR SEARCH HERE
  AStarAlgo algo;
  genericAlgo(algo);


  if (pathFound) {
    printf("Found path after checking %d nodes\n",nodesChecked);
    solution=path;
    printResults();
  }
  else {
    printf("NO PATH FOUND. Checked %d nodes.\n",nodesChecked);
    solution.clear();
  }
}

// A-star with an inadmissible heuristic
// ------------------------------------
// This version of A-star performs A-star, but instead of using
// the Euclidean distance "h" as the expected cost from any node 
// to the goal, it uses "100*h".  

void Agent::aStarInadmissible() {
  int nodesChecked=0;
  bool pathFound=false;
  std::list<Point> path;
  AStarInadmissibleAlgo algo;
  genericAlgo(algo);



  solution.clear();

  // ADD YOUR SEARCH HERE


  if (pathFound) {
    printf("Found path after checking %d nodes\n",nodesChecked);
    solution=path;
    printResults();
  }
  else {
    printf("NO PATH FOUND. Checked %d nodes.\n",nodesChecked);
    solution.clear();
  }
}


// DO NOT CHANGE THE CODE BENEATH HERE

// Print Results
// -------------
// You must keep this function as it is because we will be using
// it for our automatic testing of your code.
//
// Prints the solution path in order from start to finish.

void Agent::printResults() {
  std::list<Point> path=this->solution;
  if (solution.empty()) {
    printf("No solution has been found\n");
    return;
  }

  unsigned int i;
  float length=0.0;

  std::list<Point>::iterator itr;
  Point oldP=*path.begin();

  printf("Solution:\n");
  for (itr = path.begin(), i=1; itr != path.end(); itr++, i++) {
    if (i==1) 
      printf("%3d    Start  ", i);
    else if (i==path.size())
      printf("%3d   Finish  ", i);
    else 
      printf("%3d           ", i);
    (*itr).print();
    printf("\n");
    length+=oldP.distanceTo(*itr);
    oldP=*itr;
  }
  printf("Path length: %f\n\n", length);
}

// Run Tests
// -------------
// You must keep this function as it is because we will be using
// it for our automatic testing of your code.
// 
// Run all four searches implemented.

void Agent::runTests() {
  printf("\nPerforming best first search\n");
  this->bestFirst();
  printf("Uniform cost search\n");
  this->uniformCost();
  printf("Performing A*\n");
  this->aStar();
  printf("Performing inadmissible A*\n");
  this->aStarInadmissible();
}
