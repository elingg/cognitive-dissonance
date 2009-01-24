ARCH=$(shell /bin/uname -m)
LIN=i686
LIN64=x86_64
ifeq ($(ARCH),$(LIN64)) # 64bit linux (myth)
	INCLUDE      = -I/afs/ir/class/cs221/linux64/include
	LIBDIR       = -L/afs/ir/class/cs221/linux64/lib
else
	ifeq ($(ARCH),$(LIN)) # 32bit linux
		INCLUDE      = -I/afs/ir/class/cs221/linux/include
		LIBDIR       = -L/afs/ir/class/cs221/linux/lib
	else # solaris (elaine)
		INCLUDE      = -I /afs/ir/class/cs221/solaris/includesol
		LIBDIR       = -L /afs/ir/class/cs221/solaris/libsol
	endif
endif

LIBRARIES    = -lode -ldrawstuff -lGL -lX11 -lGLU -lm

#compiler info
CC     = g++
CFLAGS = -Wall -g $(INCLUDE) -Wno-deprecated

#objects and executable
OBJS = configspace.o main.o agent.o robotconfig.o cmdline.o robotsim.o
EXEC = rpplan

all: rpplan 


$(EXEC):  $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(LIBDIR) $(CFLAGS) $(OBJS) $(LIBRARIES)

%.o: %.cc
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o $(EXEC)
