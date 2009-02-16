/*
 *  matrixmath.h
 *  game
 *
 *  Created by Ia3n Goodfellow on 11/11/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _matrixmath_
#define _matrixmath_

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

#include <iostream> //debug
using namespace std; //debug

GLfloat len(const GLfloat * loc);
GLfloat len2(const GLfloat * loc);
GLfloat len2(GLfloat x,GLfloat y);
GLfloat dist(const GLfloat *loc1, const GLfloat *loc2);
void diff(const GLfloat *p1, const GLfloat *p2, GLfloat *ans);//Saves p1-p2 in ans. All must be initialized 3 elem.
void sum(const GLfloat *p1, const GLfloat *p2, GLfloat *ans);
void crossproduct(const GLfloat *v, const GLfloat *w, GLfloat *ans);
float dotproduct(const GLfloat *v, const GLfloat *w);
void normaltotriangle(const GLfloat *p1, const GLfloat *p2, const GLfloat *p3, GLfloat *ans);
void makeunitvector(const GLfloat *input, GLfloat *output);// input and output can point to same place
void makeunitvector(GLfloat *v);
void makescaledvector(const GLfloat *input, int scaledmag, GLfloat *output);// input and output can point to same place
inline bool sign(float n) {return n>=0;}
inline GLfloat abs(GLfloat n) {return (n>0) ? n: -n; }
void AlignVecXToVec(const GLfloat *input, const GLfloat *vec, GLfloat *output, bool flip);
void RotateVectorAboutX(const GLfloat *input,GLfloat dtheta,GLfloat *output);
void RotateVectorAboutY(const GLfloat *input,GLfloat dtheta,GLfloat *output);
void RotateVectorAboutZ(const GLfloat *input,GLfloat dtheta,GLfloat *output);
void RotateASuchThatBIsOnX(GLfloat *temp,const GLfloat *aim);
pair<GLfloat,GLfloat> RectToPolar(GLfloat x,GLfloat y);//(r,theta) where theta is -PI to PI, based on acos



//from glTools
typedef GLfloat GLTMatrix[16];      // A column major 4x4 matrix of type GLfloat
void gltLoadIdentityMatrix(GLTMatrix m);
float DetIJ(const GLTMatrix m, int i, int j);
void gltRotationMatrix(float angle, float x, float y, float z, GLTMatrix mMatrix);



#endif

