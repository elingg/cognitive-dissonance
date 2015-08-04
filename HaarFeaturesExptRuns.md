Experiments on CvBoost using our haar features alone (4-fold cross validation):
```
Depth	Trees	Examples	Training Error	Test Error	Time Taken
2	100	10000	0.00776667	0.0198	78.37
2	200	10000	0.0001	0.0187	119.6
2	400	10000	0	0.0166	206.24
2	800	10000	0	0.0156	386.26
2	1000	10000	0	0.0154	483.09
					
1	100	10000	0.00703333	0.0197	59.09
1	200	10000	0.0001	0.0185	81.56
1	400	10000	0	0.0159	156.06
1	800	10000	0	0.0157	228.36
1	1000	10000	0	0.016	278.89
					
1	200	10000	0.0001	0.0185	81.56
2	200	10000	0.0001	0.0187	119.6
4	200	10000	0.0002	0.0184	209.57
8	200	10000	6.66667E-05	0.0181	419.72
					
1	400	4000	0	0.02	55.31
1	400	8000	0	0.01675	100.58
1	400	16000	2.08333E-05	0.0144375	210.43
1	400	32000	0.0019375	0.0143437	524.92
1	400	46359	0.00386118	0.0133954	699.75
					
2	400	4000	0	0.0195	92.34
2	400	10000	0	0.0166	206.24
2	400	20000	0.000233333	0.01435	426.01
2	400	32000	0.00216667	0.0144375	701.27
2	400	46359	0.0039187	0.0134817	1020.47
```

Latest run (Wednesday 2.20PM). Uses depth of 1 but runs on all the examples.
Training error is close to 0, just didn't compute it to save on time: (takes ~22 minutes)
```
=================================================
Using 4 folds for validation.
Using cv for classification.
Using 1000 trees in the boosted tree.
Using max depth 1 in the tree.
Using 46359 examples
=================================================
Running experiments on total of: 46359 files
Average test errors: 
Test error: 0.0109587
Fold 0: 0.0109587
Time taken in fold : 357.44 seconds
Test error: 0.0136336
Fold 1: 0.0136336
Time taken in fold : 329.5 seconds
Test error: 0.0118216
Fold 2: 0.0118216
Time taken in fold : 319.56 seconds
Test error: 0.0128537
Fold 3: 0.0128537
Time taken in fold : 315.53 seconds
------------------------------------------------
Avg Training error       Avg Test error
-n/a-           0.0123169
------------------------------------------------
-------------------------------------------------------------
Confusion Matrix: Predicted labels ->
-------------------------------------------------------------
                mug     stapl   keybo   clock   sciss   other   
-------------------------------------------------------------
mug             182     5       3       0       1       104
stapl           4       242     6       0       0       141
keybo           5       8       134     0       1       87
clock           0       0       0       24      0       25
sciss           2       0       3       0       198     104
other           17      28      12      2       13      45008

-------------------------------------------------------------
        Prec    Recall  F-1
-------------------------------------------------------------
mug     0.867   0.617   0.721
stapl   0.855   0.616   0.716
keybo   0.848   0.570   0.682
clock   0.923   0.490   0.640
sciss   0.930   0.645   0.762
other   0.990   0.998   0.994
Time taken in entire experiment: 1322.03 seconds
```


This is the short cut run with just 400 trees and 16000 examples: (that took ~3 minutes)
```
=================================================
Using 4 folds for validation.
Using cv for classification.
Using 400 trees in the boosted tree.
Using max depth 1 in the tree.
Using 16000 examples
=================================================
Running experiments on total of: 16000 files
Average test errors: 
Test error: 0.01425
Fold 0: 0.01425
Time taken in fold : 47.33 seconds
Test error: 0.01575
Fold 1: 0.01575
Time taken in fold : 47.65 seconds
Test error: 0.013
Fold 2: 0.013
Time taken in fold : 48.67 seconds
Test error: 0.01475
Fold 3: 0.01475
Time taken in fold : 48.17 seconds
------------------------------------------------
Avg Training error       Avg Test error
-n/a-           0.0144375
------------------------------------------------
-------------------------------------------------------------
Confusion Matrix: Predicted labels ->
-------------------------------------------------------------
                mug     stapl   keybo   clock   sciss   other   
-------------------------------------------------------------
mug             52      1       1       0       0       40
stapl           0       70      2       0       0       62
keybo           0       3       45      0       0       44
clock           0       0       0       6       0       9
sciss           0       1       2       0       60      42
other           4       12      4       0       4       15536

-------------------------------------------------------------
        Prec    Recall  F-1
-------------------------------------------------------------
mug     0.929   0.553   0.693
stapl   0.805   0.522   0.633
keybo   0.833   0.489   0.616
clock   1.000   0.400   0.571
sciss   0.938   0.571   0.710
other   0.987   0.998   0.993
```

And this is the shortest cut run that takes 46 seconds and runs just on the first fold of the nfolds specified:
```
=================================================
Using 4 folds for validation.
Using cv for classification.
Using 400 trees in the boosted tree.
Using max depth 1 in the tree.
Ending ater first fold to save time.
Using 16000 examples
=================================================
Running experiments on total of: 16000 files
Average test errors: 
Test error: 0.01425
Fold 0: 0.01425
Terminating after 1 fold
Time taken in fold : 46.03 seconds
------------------------------------------------
Avg Training error       Avg Test error
-n/a-           0.01425
------------------------------------------------
-------------------------------------------------------------
Confusion Matrix: Predicted labels ->
-------------------------------------------------------------
                mug     stapl   keybo   clock   sciss   other   
-------------------------------------------------------------
mug             13      0       0       0       0       8
stapl           0       13      2       0       0       14
keybo           0       1       13      0       0       14
clock           0       0       0       2       0       2
sciss           0       0       0       0       19      10
other           1       2       0       0       3       3883

-------------------------------------------------------------
        Prec    Recall  F-1
-------------------------------------------------------------
mug     0.929   0.619   0.743
stapl   0.812   0.448   0.578
keybo   0.867   0.464   0.605
clock   1.000   0.500   0.667
sciss   0.864   0.655   0.745
other   0.988   0.998   0.993
Time taken in entire experiment: 46.03 seconds
```