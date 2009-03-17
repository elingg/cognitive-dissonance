// Author: Cognitive-Dissonance team

#pragma once
#include <vector>
#include <string>
#include <set>
#include "TrainingExample.h"
#include "Label.h"

using namespace std;

// Decision tree operates on generic TrainingExamples and predicts
// classes for Examples (which could also be TrainingExamples)...
class DecisionTree {
public:
  DecisionTree(const Label& positive_label)
  :m_root(0),m_positive_label(positive_label){}
  ~DecisionTree() {}

  void train(const vector<TrainingExample*>& examples);
  double predict(const Example& example) const;
  double getFeatureThreshold(size_t feature_index) const;
  void printTree() const; 
  bool loadState(ifstream& ifs);
  bool saveState(ofstream& ofs) const;

  class Node {
  public:
    Node(const DecisionTree* tree):m_left(0),m_right(0),m_tree(tree) {}
    Node(const DecisionTree* tree, size_t feature_index); // internal node
    Node(const DecisionTree* tree, double probability); // leaf node
    ~Node();
    
    void setRightChild(Node* right_child);
    void setLeftChild(Node* left_child);
    const Node* getRightChild() const;
    const Node* getLeftChild() const;

    void printTree() const;
    bool loadState(ifstream& ifs); 
    bool saveState(ofstream& ofs) const;

    double predictClassLabel(const Example& example) const;
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
  string getFeatureName(size_t feature_index) const; 
  Node* recursiveTrainTree(const vector<TrainingExample*>& examples,
                           const set<size_t>& used_feature_indeces);
  vector<double> m_feature_thresholds;
  Node* m_root;
  Label m_positive_label;
};

