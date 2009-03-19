#include "BoostedClassifier.h"
#include "ImagesExample.h"
#include "math.h"
#include "assert.h"
#include "Timer.h"
#include <fstream>
#include <iostream>

BoostedClassifier::BoostedClassifier(const Label& label,
 size_t numtrees, size_t depth)
:m_label(label), m_num_max_trees(numtrees),m_depth(depth){
}

BoostedClassifier::~BoostedClassifier() {
  for(size_t b=0; b<m_trees.size(); b++) {
    delete m_trees[b];
  }
}

unsigned int bigrand() { return RAND_MAX*rand()+rand(); }

void BoostedClassifier::train(const vector<TrainingExample*>& examples) {
  if(examples.size()==0) return;
  assert(m_trees.size()==0);
  size_t m = examples.size();
  vector<double> distribs(m,1.0/m);
  m_sum_alphas = 0;
  for(size_t b=0; b<m_num_max_trees; b++) {
    DecisionTree* hb = new DecisionTree(m_label);
    hb->train(examples, distribs);
    double epsb = 0;
    size_t num_misclassified=0;
    for(size_t i=0; i<m; i++) {
      double predictprob= hb->predict(*examples[i]);
      bool prediction = (predictprob>0.5)?true:false;
      // cerr << "Actual label: " << egs_for_tree[i]->getLabel() << endl;
      // cerr << ", Predicted: " << m_label << " with prob " << predictprob << endl;
      bool actual = isLabelEqual(examples[i]->getLabel(), m_label);
      if(prediction!=actual) {
        epsb+=distribs[i];
        num_misclassified++;
      }
    }
    // cerr << "Number misclassified: " << num_misclassified << " of " << m << ", my label: " << m_label << endl;
    // cerr << "\tEpsb: " << epsb << endl;
    double epsilon = 0.001;
    if(epsb==0) {
      epsb=epsilon;
    } else if(epsb==1.0) {
      epsb=1-epsilon;
    }
    double alphab = 0.5*log((1-epsb)/epsb)/log(2.0);
    double z = 0.0;
    for(size_t i=0; i<m; i++) {
      double predictprob= hb->predict(*examples[i]);
    //   cerr << "My tree label: " << m_label << ", actual label: " << egs_for_tree[i]->getLabel()<< ", prediction probability: " << predictprob << endl;
      bool prediction = (predictprob>0.5)?true:false;
      bool actual = isLabelEqual(examples[i]->getLabel(),m_label);
      if(prediction!=actual) {
        // if incorrect drum up...
        distribs[i] = distribs[i]*exp(alphab);
      } else {
        // if correct drum down...
        distribs[i] = distribs[i]*exp(-alphab);
      }
      z +=distribs[i];
    }
    // cerr << "z: " << z << endl;
    for(size_t i=0; i<m; i++) {
      distribs[i] = distribs[i]/z;
    } 
    m_trees.push_back(hb);
    m_tree_alpha.push_back(alphab);
    m_sum_alphas += alphab;
    // cerr << "Trained tree with alphab: " << alphab <<endl;
  } // for each tree 
}

double BoostedClassifier::predict(const Example& example) const {
  double verdict = 0;
  for(size_t b=0; b<m_trees.size(); b++) {
    double tree_verdict = m_trees[b]->predict(example);
  //  cerr << "Tree verdict: " << tree_verdict << ", alpha: " << m_tree_alpha[b] 
  //      << ", sum of alphas: " << m_sum_alphas << endl; 
    if(tree_verdict>0.5) {
      verdict += tree_verdict*m_tree_alpha[b]/m_sum_alphas; 
    }
  }
  return verdict;
}

bool BoostedClassifier::loadState(const char* file) {
  cerr <<"TODO BoostedClassifier::loadState";
  return true;
}

bool BoostedClassifier::saveState(const char* file) const {
  cerr <<"TODO BoostedClassifier::saveState";
  return true;
}
