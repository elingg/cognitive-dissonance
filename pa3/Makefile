
#CXX=g++ -m32  #On a 64-bit machine, you may need to use this instead.
CXX=g++

CFLAGS=-g -Wall
LDFLAGS=-lm
GRAPHICS_LIBS = -lX11 -lXi -lXmu -lGL -lGLU -lglut

#LDFLAGS=-lm -L/usr/X11/lib -framework OpenGL -framework GLUT
#GRAPHICS_LIBS = -lX11 -lXi -lXmu -lGL -lGLU 

SRC = main.cpp carmodel.cpp carmodel.h \
	transitionmodel.cpp transitionmodel.h \
	reward.cpp reward.h

RUNSRC = runpolicy.cpp carmodel.cpp carmodel.h \
	transitionmodel.cpp transitionmodel.h \
        graphics.cpp nehe/mgt.cpp simulation.cpp \
        nehe/vmath.cpp nehe/MilkshapeModel.cpp \
        nehe/Model.cpp nehe/texture.cpp basis.cpp \
        matrixmath.cpp

default: all
all: main runpolicy

main: $(SRC)
	$(CXX) -o main $(SRC) $(CFLAGS) $(LDFLAGS)

runpolicy: $(RUNSRC)
	$(CXX) -o runpolicy $(RUNSRC) $(CFLAGS) $(LDFLAGS) $(GRAPHICS_LIBS)

clean:
	rm -f main runpolicy
