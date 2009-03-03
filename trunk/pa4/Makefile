##
## General Makefile for C++
##

## the -D__USE_FIXED_PROTOTYPES__ is essential for using the real
## ansi headers that ship with gcc

CC = g++
CPPFLAGS = -g -O3 -Wall -D__USE_FIXED_PROTOTYPES__

INCDIR = 

LIBDIR = 
LIBS = 

SRCS = main.cpp Robot.cpp PpmUtil.cpp Maps.cpp SensorModel.cpp ActionModel.cpp 
OBJS = $(SRCS:.c=.o)
PROGS = localize

default : $(PROGS) 

$(OBJS) :
	$(CC)  $(CPPFLAGS) $(INCDIR) -c -o $@ $(@:.o=.c)

$(PROGS) :  depend $(OBJS)
	$(CC) $(CPPFLAGS) $(LIBDIR) -o $@ $(OBJS)  $(LIBS)

pure: $(PROGS).purify

$(PROGS).purify: $(OBJS)
	purify -cache-dir=/tmp/$(USER) \
$(CC) $(CPPFLAGS) $(LIBDIR) -o $@ $(OBJS)  $(LIBS)


# The dependencies below make use of make's default rules,
# under which a .o automatically depends on its .c and
# the action taken uses the $(CC) and $(CFLAGS) variables.
# These lines describe a few extra dependencies involved.

depend:: Makefile.dependencies $(SRCS) $(HDRS)

Makefile.dependencies:: $(SRCS) $(HDRS)
	$(CC) $(CPPFLAGS) $(INCDIR) -MM $(SRCS) > Makefile.dependencies

-include Makefile.dependencies

clean : 
	/bin/rm -f *.o *~ a.out core $(PROGS) $(PROGS).purify Makefile.dependencies

TAGS : $(SRCS) $(HDRS)
	etags -t $(SRCS) $(HDRS)







