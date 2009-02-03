#include "DecisionTree.h"
#include "math.h"
#include <cfloat>
#include <iostream>

double H(double p) {
  return -p*(log10(p)/log10(2)) - (1-p)*(log10(1-p)/log10(2));
}

size_t countPositiveClassLabels(const vector<TrainingExample*>& examples) {
  size_t count = 0;
  for(size_t iex=0; iex<examples.size(); ++iex) {
    if(examples[iex]->getClassLabel()) {
      count++;
    }
  }
  return count;
}

void splitExamplesByThreshold(
 vector<TrainingExample*>& positive_examples,
 vector<TrainingExample*>& negative_examples,
 const vector<TrainingExample*>& examples,
 size_t feature_index, double threshold) {
  for(size_t iex=0; iex<examples.size(); ++iex) {
    if(examples[iex]->getFeatureDoubleValue(feature_index)>threshold) {
      positive_examples.push_back(examples[iex]);
    } else {
      negative_examples.push_back(examples[iex]);
    }
  }
}

size_t chooseFeature(const vector<TrainingExample*>& examples,
 const vector<string>& feature_names, 
 const vector<double>& feature_thresholds,
 const set<size_t>& used_feature_indeces) {
  size_t positives = countPositiveClassLabels(examples);
  double ptot = (double)(positives)/(double)(examples.size());
  double mtot = examples.size()*H(ptot);

  double max_gain = -DBL_MAX;
  size_t chosen_feature = -1; 
  // for each feature in feature_names...
  for(size_t ifeat=0;ifeat<feature_names.size(); ++ifeat) {
    if(used_feature_indeces.find(ifeat)!=used_feature_indeces.end()) {
      // index is used...
      continue;
    }
    vector<TrainingExample*> positive_examples;
    vector<TrainingExample*> negative_examples;
    splitExamplesByThreshold(positive_examples,
                             negative_examples,
                             examples,
                             ifeat,
                             feature_thresholds[ifeat]);
    // get number of examples with positive class labels...
    double pplus = countPositiveClassLabels(positive_examples)/
                          (double)(positive_examples.size());
    double mplus = positive_examples.size()*H(pplus);
    double pminus = countPositiveClassLabels(negative_examples)/
                          (double)(negative_examples.size());
    double mminus = negative_examples.size()*H(pminus);
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

bool DecisionTree::Node::predictClassLabel(const Example& example) const {
  if(example.getFeatureDoubleValue(m_feature_index)>
     m_tree->getFeatureThreshold(m_feature_index)) { 
    if(m_right==0) { // leaf...
      assert((m_probability>=0) && (m_probability<=1));
      return (m_probability>0.5);
    }
    return m_right->predictClassLabel(example);
  } 
  if(m_left==0) {
    assert((m_probability>=0) && (m_probability<=1));
    return (m_probability>0.5);
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
  if(used_feature_indeces.size()==m_features.size()) { // all used up
    // stop criteria 3...
    double pl = (double)(countPositiveClassLabels(examples))/
                               (double)(examples.size());
    Node* T = new Node(this, pl); // leaf
    return T;
  }
 
  size_t chosen_feature_index = 
               chooseFeature(examples, m_features, 
                             m_feature_thresholds, used_feature_indeces);
  set<size_t> now_used_feature_indeces = used_feature_indeces;
  now_used_feature_indeces.erase(chosen_feature_index);

  // make this the root of the tree
  Node* T = new Node(this, chosen_feature_index);
  // instead of for loop over values v of feature, we just have two...
  vector<TrainingExample*> positive_examples;
  vector<TrainingExample*> negative_examples;
  cerr << "Split examples:\n";
  splitExamplesByThreshold(positive_examples,
                           negative_examples,
                           examples,
                           chosen_feature_index,
                           m_feature_thresholds[chosen_feature_index]);
  Node* right_child=0;
  if(positive_examples.size()>0) {
    cerr << "Recursive train tree for positive examples:\n";
    right_child = recursiveTrainTree(positive_examples, 
                                     now_used_feature_indeces);
  } else { // stop criteria 1, 2... (use parents examples)...
    double pl = (double)(countPositiveClassLabels(examples))/
                 (double)(examples.size());
    right_child = new Node(this,pl); // leaf
  }
  T->setRightChild(right_child);
 
  Node* left_child=0; 
  if(positive_examples.size()>0) {
    cerr << "Recursive train tree for negative examples:\n";
    left_child = recursiveTrainTree(negative_examples,
                                    now_used_feature_indeces);
  } else { // stop criteria 1, 2... (use parents examples)...
    double pl = (double)(countPositiveClassLabels(examples))/
                 (double)(examples.size());
    right_child = new Node(this,pl); // leaf
  }
  T->setLeftChild(left_child);
  
  return T; 
}

void DecisionTree::trainTree
(const vector<TrainingExample*>& examples) {
  cerr << "Train tree:\n";
  if(m_root) {
    delete m_root;
    m_root = 0;
  }
  m_feature_thresholds.clear();
 
  size_t num_features = m_features.size();
  size_t num_examples = examples.size();

  // compute feature averages that we shall use as thresholds...
  vector<double> feature_averages;

  feature_averages.resize(num_features,0.0);
  for(size_t ie=0; ie<num_examples; ++ie) {
    const TrainingExample* ex = examples[ie];
    assert(ex->getNumberOfFeatures()==num_features);
    for(size_t ifeat=0; ifeat<num_features; ifeat++) {
      double fv = ex->getFeatureDoubleValue(ifeat);
      feature_averages[ifeat]+=fv; 
    }
  }
  cerr << "Average thresholds:\n";
  for(size_t ifeat=0; ifeat<num_features; ifeat++) {
    feature_averages[ifeat]/=num_examples;
    cerr << ifeat << ": " << feature_averages[ifeat] << endl;
  }
  m_feature_thresholds = feature_averages;
 
  cerr << "Recursive train tree:\n";
  set<size_t> empty_used_features;
  m_root = recursiveTrainTree(examples,empty_used_features);
}

bool DecisionTree::predictClassLabel(const Example& example) const {
  cerr << "Predicting class label\n";
  return m_root->predictClassLabel(example); 
}

