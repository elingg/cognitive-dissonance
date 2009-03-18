## ---------------------------------------------------------------------------
## CS221 Computer Vision Project, 2006
## Copyright (c) 2006, Stanford University
##
## Makefile
##
## LINUX INSTALLATION NOTES:
##  1. Install ffmpeg (if not already installed)
##      ./configure --prefix=<home> --enable-shared && make && make install
##  2. Install opencv (if not already installed)
##      ./configure --prefix=<home> CPPFLAGS="-I<home>/include" \
##          LDFLAGS="-L<home>/lib"&& make && make install
##  3. Make CS221 project
##      make
## ---------------------------------------------------------------------------

include Make.defaults

PROGS = test train evaluate

HDR = objects.h classifier.h replay.h utils.h CXMLParser.h HaarFeatures.h EdgeDetectionFeatures.h DecisionTree.h ImagesExample.h MotionTracker.h Hough.h KalmanFilter.h LucasKanade.h AbstractBinaryClassifier.h AbstractMulticlassClassifier.h BoostedClassifier.h MulticlassClassifier.h Label.h CvBoostedClassifier.h TrainingExample.h Timer.h Stats.h

SRC = objects.cpp classifier.cpp replay.cpp utils.cpp CXMLParser.cpp HaarFeatures.cpp EdgeDetectionFeatures.cpp DecisionTree.cpp ImagesExample.cpp MotionTracker.cpp Hough.cpp KalmanFilter.cpp LucasKanade.cpp BoostedClassifier.cpp MulticlassClassifier.cpp Label.cpp CvBoostedClassifier.cpp  Stats.cpp

#MORE_LIBS=-ltiff -lpthread -lpng -lm
#GTK_LIBS=$(shell pkg-config gtk+ --libs)
#GT_LIBS=$(shell pkg-config gthread-2.0 --libs)
CV_LIBS=-lcxcore -lcv -lhighgui -lcvaux -lml
LIBS=$(MORE_LIBS) $(GT_LIBS) $(GTK_LIBS) $(CV_LIBS)



DEFS = -O3
CC = g++
#OPENCV_LIB_DIR = /afs/ir.stanford.edu/class/cs221/lib64/

CCFLAGS = -Wall $(DEFS) -I$(OPENCV_INC_DIR)
LDFLAGS = -L$(OPENCV_LIB_DIR)

SUFFIX = $(firstword $(suffix $(SRC)))
OBJ = $(SRC:$(SUFFIX)=.o)

# rule for making everything
all: $(OBJ) $(PROGS)

# specify how to build binaries
test: test.o $(OBJ)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(OBJ) test.o -o $@ $(LIBS) 

train: train.o $(OBJ)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(OBJ) train.o -o $@ $(LIBS) 

evaluate: evaluate.o $(OBJ)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(OBJ) evaluate.o -o $@ $(LIBS) 

tune: tune.cpp tune.o $(OBJ)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(OBJ) tune.o -o $@ $(LIBS) 

unittests: UnitTests.o $(OBJ)
	$(CC) $(CCFLAGS) -c UnitTests.cpp UnitTests.o 
	$(CC) $(CCFLAGS) $(LDFLAGS) UnitTests.o Stats.o Label.o -o unittests

unittestshaar: TestHaarFeatures.h TestHaarFeatures.cpp HaarFeatures.h HaarFeatures.cpp $(OBJ)
	$(CC) $(CCFLAGS) -c TestHaarFeatures.cpp -o TestHaarFeatures.o
	$(CC) $(CCFLAGS) -c HaarFeatures.cpp -o HaarFeatures.o 
	$(CC) $(CCFLAGS) $(LDFLAGS) TestHaarFeatures.o HaarFeatures.o -o unittestshaar $(LIBS)

# define compilation rule for .o files
%.o : %.cpp
	$(CC) $(CCFLAGS) -c $< -o $@

# clean
.PHONY : clean
clean:
	rm -f *.o $(PROGS) core unittests

