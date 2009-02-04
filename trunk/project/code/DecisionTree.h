// Author: Cognitive-Dissonance team

#pragma once
#include <vector>
#include <string>
#include <set>

using namespace std;

// an example is just a bunch of features... (could be a test or training
// example). generic enough to just call 'data', but might have name clashes 
// so using 'Example' for now (but applies to test data as well)...
class Example {
public:
  Example() {}
  virtual ~Example() {}

  virtual size_t getNumberOfFeatures() const = 0;

  // features could be any type, so if adding int/string features
  // provide other functions, for now just double... 
  virtual double getFeatureDoubleValue(int feature_index) const = 0;
};

// training example is an example for which we know the class label...
class TrainingExample : public Example {
public:
  TrainingExample() {}
  virtual ~TrainingExample() {}

  virtual bool getClassLabel() const = 0; // 0/1 for now
};

// Decision tree operates on generic TrainingExamples and predicts
// classes for Examples (which could also be TrainingExamples)...
class DecisionTree {
public:
  DecisionTree(const vector<string>& feature_names)
     :m_features(feature_names),m_root(0) {}
  ~DecisionTree() {}

  void trainTree(const vector<TrainingExample*>& examples);
  bool predictClassLabel(const Example& example) const;
  double getFeatureThreshold(size_t feature_index) const;
  string getFeatureName(size_t feature_index) const; 
  void printTree() const; 

  class Node {
  public:
    Node(const DecisionTree* tree, size_t feature_index); // internal node
    Node(const DecisionTree* tree, double probability); // leaf node
    ~Node();
    
    void setRightChild(Node* right_child);
    void setLeftChild(Node* left_child);
    const Node* getRightChild() const;
    const Node* getLeftChild() const;

    void printTree() const;

    bool predictClassLabel(const Example& example) const;
    bool isLeaf() const;
    double getLeafProbability() const; // only sane if isLeaf() is true.
  private:
    size_t m_feature_index;
    Node *m_left, *m_right;
    Node* m_parent;
    double m_probability; // if leaf node (m_left=m_right=0)
    const DecisionTree* m_tree;
  };
private:
  Node* recursiveTrainTree(const vector<TrainingExample*>& examples,
                           const set<size_t>& used_feature_indeces);
  vector<double> m_feature_thresholds;
  vector<string> m_features;
  Node* m_root;
};

