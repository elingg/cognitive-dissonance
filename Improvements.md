# Introduction #
Final report. As we find new information, please update the page, so we have what we tried documented here.


# Documentation of improvements attempted and results #

| **What we tried (in sequence)** | **F1 score (mug)** | **Overall F1 score** |
|:--------------------------------|:-------------------|:---------------------|
| Haar features                   | 0.00416667         | 0.00320342           |
| Clubbing overlapping rectangles | 0.209366           | 0.131148             |
| Max window length to 208 (from 104) | 0.414073           | 0.261092             |
| Kalman Filter                   | 0.440678           | 0.281667             |
| Kalman Filter with classifier returning less false positives`*`| 0.530612           | 0.32529              |
| With above, but using CvBoost now with multiclass`**` | 0.563218           | 0.246408             |

`*` The decision tree was retrained with depth = 7, classification threshold = 0.8

`**` CvBoost used with 1000 tree-forest and depth of 3

# HaarFeaturesExptRuns #