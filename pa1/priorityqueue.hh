

#ifndef PRIORITYQUEUE_HH
#define PRIORITYQUEUE_HH

#include <queue>

using namespace std;

//////////////////
//PRIORITY QUEUE//
//////////////////

// Implemented using the STL priority queue.
// Note that all variables are only suggestions.  Use them only if they will help your
// particular implementation.  You may add or change variables as you see fit.

class QueueNode {
public:
  // Constructors and destructors
  QueueNode(float pathCost, float heuristicCost, std::list<Point> path) : 
    pathCost(pathCost), heuristicCost(heuristicCost), path(path) {};
  QueueNode() {};
  ~QueueNode() {};
  
  // Stores the total cost of all operations done on this path through (including) the current node
  float pathCost;

  // Stores the estimated heuristic from the current point to the goal
  float heuristicCost;

  // A list of all points visited from the start node to the current node
  std::list<Point> path;

  // ******CHANGE THIS OPERATOR*******
  // This should return true if "b" has a higher priority  than "a"
  // Ie, if we want "b" to be popped off the priority queue before "a", return true.
  // In the case of a tie, the priority queue should behave in a first-in
  // first-out (FIFO) manner; that is, if "a" and "b" have the same
  // priority, we want the node that was added first to be popped off
  // first.  Note that this priority queue does _not_ automatically
  // preserve order like this; you need to break ties carefully here.
  //
  // This is the function that the priority queue will call to order all its elements.  
  bool operator()(QueueNode a, QueueNode b) {
    return true;
  }

};

// Define what kind elements will be stored in this priority queue and how
typedef priority_queue<QueueNode, vector<QueueNode>, QueueNode> PriorityQueue;


#endif  //PRIORITYQUEUE_HH