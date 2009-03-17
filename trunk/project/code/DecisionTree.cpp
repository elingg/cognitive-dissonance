#include "DecisionTree.h"
#include "math.h"
#include <cfloat>
#include <iostream>
#include "assert.h"
#include <fstream>
#include <sstream>

size_t getMaxDepth() {
  return 3;
}

double getClassificationThreshold() {
  return 0.5;
}

double H(double p) {
  if(p==0.0 || p==1.0) { 
    return 0.0;
  } 

  return -p*(log10(p)/log10(2)) - (1-p)*(log10(1-p)/log10(2));
}

size_t countPositiveClassLabels(const Label& label,
 const vector<TrainingExample*>& examples) {
  size_t count = 0;
  for(size_t iex=0; iex<examples.size(); ++iex) {
    if(isLabelEqual(examples[iex]->getLabel(),label)) {
      count++;
    }
  }
  return count;
}

void splitExamplesByThreshold(
 vector<TrainingExample*>& above_examples,
 vector<TrainingExample*>& below_examples,
 const vector<TrainingExample*>& examples,
 size_t feature_index, double threshold) {
  //double new_threshold=0.0;
  //for(size_t iex=0; iex<examples.size(); ++iex) {
  //  new_threshold+=examples[iex]->getFeatureDoubleValue(feature_index);
  //}
  //new_threshold /= examples.size();
  //threshold = new_threshold;
  for(size_t iex=0; iex<examples.size(); ++iex) {
    if(examples[iex]->getFeatureDoubleValue(feature_index)>threshold) {
      above_examples.push_back(examples[iex]);
    } else {
      below_examples.push_back(examples[iex]);
    }
  }
}

size_t chooseFeature(const Label& positive_label,
 const vector<TrainingExample*>& examples,
 const vector<double>& feature_thresholds,
 const set<size_t>& used_feature_indeces) {
  size_t positives = countPositiveClassLabels(positive_label, examples);
  double ptot = (double)(positives)/(double)(examples.size());
  double mtot = examples.size()*H(ptot);
  // cerr << "Positive: " << positives << ", ptot: " << ptot << ", mtot: " << mtot <<", number of used indeces: " << used_feature_indeces.size() <<  endl;
  double max_gain = -DBL_MAX;
  size_t chosen_feature = -1; 
  // for each feature in feature_names...
  assert(examples.size()>0);
  size_t num_features = examples[0]->getNumberOfFeatures();
  for(size_t ifeat=0;ifeat<num_features; ++ifeat) {
    if(used_feature_indeces.find(ifeat)!=used_feature_indeces.end()) {
      // index is used...
      // cerr << "Feature " << ifeat << " used, skipping\n";
      continue;
    }
    vector<TrainingExample*> above_examples;
    vector<TrainingExample*> below_examples;
    splitExamplesByThreshold(above_examples,
                             below_examples,
                             examples,
                             ifeat,
                             feature_thresholds[ifeat]);
    // get number of examples with positive class labels...
    double pplus = countPositiveClassLabels(positive_label, above_examples)/
                          (double)(above_examples.size());
    double mplus = above_examples.size()*H(pplus);
    double pminus = countPositiveClassLabels(positive_label, below_examples)/
                          (double)(below_examples.size());
    double mminus = below_examples.size()*H(pminus);
    double gain = mtot - (mplus + mminus);
    if(gain>max_gain) {
      max_gain = gain; 
      chosen_feature = ifeat;
    } 
  }
  return chosen_feature;
}

DecisionTree::Node::Node(const DecisionTree* tree, double pl):
m_feature_index(-1), m_left(0), m_right(0),
m_parent(0), m_probability(pl), m_tree(tree) {
}

DecisionTree::Node::Node(const DecisionTree* tree, size_t feature_index):
m_feature_index(feature_index), m_left(0), m_right(0),
m_parent(0), m_probability(-1), m_tree(tree) {
}

DecisionTree::Node::~Node() {
  if(m_left) {
    delete m_left;
  }
  if(m_right) {
    delete m_right;
  }
}

double DecisionTree::Node::getLeafProbability() const {
  return m_probability;
}

bool DecisionTree::Node::isLeaf() const { 
  if(m_left==0) { 
    assert(m_right==0);
    return true;
  }
  return false;
}

void DecisionTree::Node::setRightChild(Node* right_child) { 
  if(m_right) {
    delete m_right;
  }
  m_right = right_child;
  right_child->m_parent = this; 
}

void DecisionTree::Node::setLeftChild(Node* left_child) { 
  if(m_left) {
    delete m_left;
  }
  m_left = left_child; 
  left_child->m_parent = this; 
}

double DecisionTree::Node::predictClassLabel(const Example& example) const {
  if(m_right==0) { // leaf...
    assert(m_left==0);
    assert((m_probability>=0) && (m_probability<=1));
    // return (m_probability>getClassificationThreshold());
    return m_probability;
  }
  if(example.getFeatureDoubleValue(m_feature_index)>
     m_tree->getFeatureThreshold(m_feature_index)) { 
    return m_right->predictClassLabel(example);
  } 
  return m_left->predictClassLabel(example);
}

double DecisionTree::getFeatureThreshold(size_t feature_index) const {
  assert(feature_index<m_feature_thresholds.size());
  return m_feature_thresholds[feature_index];
}

// Stop criteria:
// 1) When all the training examples agree at each leaf node: in this
//    case we can just return either 1.0 or 0.0 for each leaf node.
// 2) When we have no more examples down a particular branch:
//    we can then assign a default value, such as the overall fraction of
//    positive examples in the entire training set, or the overall fraction 
//    of positive examples under the parent node
// 3) When we have no more attributes to split on: Then we let pl be the
//    proportion of positive examples in that part of the tree.
DecisionTree::Node* DecisionTree::recursiveTrainTree
(const vector<TrainingExample*>& examples, 
 const set<size_t>& used_feature_indeces) {
  // split on feature that gives greatest increase in the log likelihood..
  // (the feature that gives greatest reduction in entropy)..
  // if((used_feature_indeces.size()==m_features.size())) {
  assert(examples.size()>0);
  size_t num_features = examples[0]->getNumberOfFeatures();
  if((used_feature_indeces.size()==num_features) || 
     (used_feature_indeces.size()==getMaxDepth())) { // all used up
    // stop criteria 3...
    double pl = (double)(countPositiveClassLabels(m_positive_label, examples))/
                               (double)(examples.size());
    Node* T = new Node(this, pl); // leaf
    // cerr << "encountered main leaf: features used up.\n";
    return T;
  }
 
  size_t chosen_feature_index = 
               chooseFeature(m_positive_label, examples, 
                             m_feature_thresholds, used_feature_indeces);
  if(chosen_feature_index==(size_t)(-1)) { // all examples are one or the other
    double pl = (double)(countPositiveClassLabels(m_positive_label, examples))/
                               (double)(examples.size());
    Node* T = new Node(this, pl); // leaf
    // cerr << "encountered main leaf: invalid chosen_feature_index, all examples one or the other.\n";
    return T;
  }
  set<size_t> now_used_feature_indeces = used_feature_indeces;
  now_used_feature_indeces.insert(chosen_feature_index);
  // cerr << "Chosen feature ["<< chosen_feature_index<< "]: " << m_features[chosen_feature_index] << endl;
  // make this the root of the tree
  Node* T = new Node(this, chosen_feature_index);
  // instead of for loop over values v of feature, we just have two...
  vector<TrainingExample*> above_examples;
  vector<TrainingExample*> below_examples;
  splitExamplesByThreshold(above_examples,
                           below_examples,
                           examples,
                           chosen_feature_index,
                           m_feature_thresholds[chosen_feature_index]);
  Node* right_child=0;
  size_t count_positive_class_labels_for_above_examples = 
                 countPositiveClassLabels(m_positive_label, above_examples);
  if(above_examples.size()==0) { // stop criteria 2... (use parents examples)...
    double pl = (double)(count_positive_class_labels_for_above_examples)/
                 (double)(examples.size());
    right_child = new Node(this,pl); // leaf
    // cerr << "encountered right leaf: no above examples\n";
  } else if(count_positive_class_labels_for_above_examples
                  ==above_examples.size()) {
    // stop criteria 1... (use probability 1)...
    right_child = new Node(this, 1.0); // leaf
    // cerr << "encountered right leaf: no negative examples in class 'above'\n";
  } else if(count_positive_class_labels_for_above_examples==0) {
    // stop criteria 1... (use probability 0)...
    right_child = new Node(this, 0.0); // leaf
    // cerr << "encountered right leaf: no positive examples in class 'above'\n";
  } else {
    // cerr << "Splitting right on feature [" << chosen_feature_index << "]: \"" << m_features[chosen_feature_index];
    // cerr << "\" using " << above_examples.size() << " 'above' examples\n";
    right_child = recursiveTrainTree(above_examples, 
                                     now_used_feature_indeces);
  }
  T->setRightChild(right_child);
 
  Node* left_child=0; 
  size_t count_positive_class_labels_for_below_examples = 
                 countPositiveClassLabels(m_positive_label, below_examples);
  if(below_examples.size()==0) { // stop criteria 2... (use parents examples)...
    double pl = (double)(count_positive_class_labels_for_below_examples)/
                 (double)(examples.size());
    left_child = new Node(this,pl); // leaf
    // cerr << "encountered left leaf: no examples in class 'below'\n";
  } else if(count_positive_class_labels_for_below_examples
                  ==below_examples.size()) {
    // stop criteria 1... (use probability 1)...
    left_child = new Node(this, 1.0); // leaf
    // cerr << "encountered left leaf: no positive examples in class 'below'\n";
  } else if(count_positive_class_labels_for_below_examples==0) {
    // stop criteria 1... (use probability 0)...
    left_child = new Node(this, 0.0); // leaf
    // cerr << "encountered left leaf: no negative examples in class 'below'\n";
  } else {
    // cerr << "Splitting left on feature [" << chosen_feature_index << "]: \"" << m_features[chosen_feature_index];
    // cerr << "\" using " << below_examples.size() << " 'below' examples\n";
    left_child = recursiveTrainTree(below_examples,
                                    now_used_feature_indeces);
  }
  T->setLeftChild(left_child);
  
  return T; 
}

void DecisionTree::train
(const vector<TrainingExample*>& examples) {
  // cerr << "Training tree with " << examples.size() << " training examples\n";
  // cerr << "Tree max-depth used: " << getMaxDepth() << endl;
  // cerr << "Classification threshold used: " << getClassificationThreshold() << endl;
  if(m_root) {
    delete m_root;
    m_root = 0;
  }
  m_feature_thresholds.clear();
  assert(examples.size()>0); 
  size_t num_features = examples[0]->getNumberOfFeatures();
  size_t num_examples = examples.size();

  // compute feature averages that we shall use as thresholds...
  vector<double> feature_averages;
  // cerr << "Number of examples: " << num_examples << ", Number of features: " << num_features << endl;
  feature_averages.resize(num_features,0.0);
  for(size_t ie=0; ie<num_examples; ++ie) {
    // cerr << "Processing example: " << ie << endl;
    const TrainingExample* ex = examples[ie];
    // cerr << "Number of features: " << examples[ie]->getNumberOfFeatures() << endl;
    assert(ex->getNumberOfFeatures()==num_features);
    for(size_t ifeat=0; ifeat<num_features; ifeat++) {
      // cerr << "Feature: " << ifeat << ", value: " << ex->getFeatureDoubleValue(ifeat);
      double fv = ex->getFeatureDoubleValue(ifeat);
      feature_averages[ifeat]+=fv; 
    }
  }
  // cerr << "Average thresholds:\n";
  for(size_t ifeat=0; ifeat<num_features; ifeat++) {
    feature_averages[ifeat]/=num_examples;
    // cerr << ifeat << ": " << feature_averages[ifeat] << "\t";
  }
  m_feature_thresholds = feature_averages;
 
  // cerr << "Recursive train tree:\n";
  set<size_t> empty_used_features;
  m_root = recursiveTrainTree(examples,empty_used_features);
  // printTree();
}

double DecisionTree::predict(const Example& example) const {
  return m_root->predictClassLabel(example);
}

const DecisionTree::Node* DecisionTree::Node::getLeftChild() const {
  return m_left;
}

const DecisionTree::Node* DecisionTree::Node::getRightChild() const {
  return m_right;
}

string DecisionTree::getFeatureName(size_t feature_index) const {
  stringstream result; 
  result << "feat";
  result << feature_index;
  return result.str();
}

bool DecisionTree::Node::loadState(ifstream& ifs) {
  string dummy;
  ifs >> dummy; assert(dummy=="feat_index");
  ifs >> m_feature_index;
  ifs >> dummy; assert(dummy=="p");
  ifs >> m_probability;
  bool has_left, has_right;
  ifs >> has_left;
  if(has_left) {
    string leftkey;
    ifs >> leftkey; 
    assert(leftkey=="left");
    m_left = new Node(m_tree);
    m_left->loadState(ifs);
    m_left->m_parent = this;
  }
  ifs >> has_right;
  if(has_right) {
    string rightkey;
    ifs >> rightkey; 
    assert(rightkey=="right");
    m_right = new Node(m_tree);
    m_right->loadState(ifs);
    m_right->m_parent = this;
  }
  return true;
}
 
bool DecisionTree::Node::saveState(ofstream& ofs) const {
  ofs << "feat_index "<< m_feature_index << " ";
  ofs << "p " << m_probability << " ";
  ofs << (m_left!=0) << " ";
  if(m_left) {
    ofs << "left "; 
    m_left->saveState(ofs);
  }
  ofs << (m_right!=0) << " ";
  if(m_right) {
    ofs << "right "; 
    m_right->saveState(ofs);
  }
  return true;
}

void DecisionTree::Node::printTree() const {
  // cerr << "[";
  if(isLeaf()) {
  //   cerr << "LEAF: "<< m_probability<<" ";
     if(m_probability>0 && m_probability<1) 
       cerr << m_probability << " ";
  } else {
  //  cerr << m_tree->getFeatureName(m_feature_index);
  }
  if(!isLeaf()) {
  //  cerr << "-> L: ";
    getLeftChild()->printTree();
  //  cerr << " ";
  }
  if(!isLeaf()) {
  //  cerr << "-> R: ";
    getRightChild()->printTree();
  //  cerr << " ";
  }
  // cerr << "]";
}

void DecisionTree::printTree() const {
  cerr << endl;
  cerr << "Leaves with probabilities other than 0 or 1: " << endl;
  m_root->printTree();
  cerr << endl;
}
 
bool DecisionTree::loadState(ifstream& ifs) {
  cerr << "DecisionTree loadState\n";
  size_t num_features;
  ifs >> num_features;
  // cerr << "num_features: ["<<num_features<< "]";
  for(size_t i=0; i<num_features; ++i) {
    double threshold;
    ifs >> threshold;
    m_feature_thresholds.push_back(threshold);
    // cerr << "[" << threshold << "]";
  }
  // cerr << "..done DecisionTree loadState\n";
  m_root = new Node(this);
  bool ret_val = m_root->loadState(ifs);
  return ret_val;
}

bool DecisionTree::saveState(ofstream& ofs) const {
  cerr << "DecisionTree saveState\n";
  ofs << m_feature_thresholds.size() << " ";
  for(size_t i=0; i<m_feature_thresholds.size(); ++i) {
    ofs << m_feature_thresholds[i] << " ";
  }
  cerr << "..done DecisionTree saveState\n";
  bool ret_val = m_root->saveState(ofs);
  return ret_val;
}
