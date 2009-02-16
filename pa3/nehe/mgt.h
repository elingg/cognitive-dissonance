//mgt.h
// Contains code copied from main function of NEHE tutorial on car network game
// Edited for CS221 PA3 purposes by Ian Goodfellow <ia3n@stanford.edu>

#ifndef _mgt_
#define _mgt_


#ifdef __APPLE__
#include <Carbon/Carbon.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <stdio.h>
#endif

void initialize();
void init_opengl();
void draw_car();

#endif
