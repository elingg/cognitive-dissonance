# Introduction #
Please add items that you think need to be done for the vision project to this page. Items with owners can be marked with your name, so two people don't work on the same thing. Items finished can be marked finished as well.

# Items to do #
## Classifier tasks ##
  * (**DONE**) Replace decision tree with OpenCV's decision tree (add unit test) (**Anand**)
  * (**DONE**) OpenCV Bagging and Boosting (**Anand**)
  * (**DONE**) Add support for multi-class (**Anand**)
  * Create Haar features for other classes ([Tutorial](http://lab.cntl.kyutech.ac.jp/~kobalab/nishida/opencv/OpenCV_ObjectDetection_HowTo.pdf))
  * Implement 'Attentional Cascade' described in the Viola and Jones paper.
  * Use CvSVM after honing down on features to use.

## Feature tasks ##
  * Implement edge detection (sobel filter) (add unit test) (**Liz**)
  * Implement line detection (hough filter) (add unit test) (Don't try because we don't know what to do with it)
  * Implement corner detection (harris filter) (add unit test) (Don't try because we don't know what to do with it)
  * Optical Flow (**Alec**)
  * Kalman Filter (**Alec**)
  * Particle filters
  * “Sharing visual features for multiclass and multiview object detection.”
  * “Rapid object detection using a boosted cascade of simple features”
  * "A new biologically motivated framework for robust object recognition”
  * “Distinctive image features from scale-invariant keypoints”

## Other tasks ##
  * (**[DONE](Improvements.md)**) Implement removing of false positive frames (apparently lots of students got better F1 scores doing this says Ian), by coalescing the windows. Uses a threshold of 0.05 percent of overlap for coalescing, can be one parameter to tweak to see what improves F1-scores. Currently improved FINAL F1-score from 0.003 to 0.13 (**Anand**)
  * (**[DONE](Improvements.md)**) Increased length of windows from 104 to 208. Improved FINAL F1-score from 0.13 to 0.26. (**Anand**)
  * Use built in OpenCV samples folder such as blob detection (which works surprisingly well!)

## Infrastructure tasks ##
  * (**DONE**) Provide a way to do k-fold cross validation with outputing of confusion matrix to diagnose problems with features. (**Anand**)
  * This counts as a valid extension now:
    * Run experiments and produce graphs for just haar features and CvBoost.
    * F1/Precision vs depth of boost tree (1, 2, 3, 4 atleast). Also plot time to train.
    * F1/Precision vs size of forest (100, 200, 500, 1000). Also plot time to train.
    * Write up report portion for this.