/*
 *  basis.h
 *  game
 *
 *  Created by Ia3n Goodfellow on 11/11/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _basis
#define _basis

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

class Basis
{
	public:
		Basis();
		Basis(const Basis * other);
		void PlaceCam();
		void ApplyToGraphics();//matrixmode must be modelview
		void ApplyToVector(const GLfloat *input4,GLfloat *output4);//input and output can't be same thing, will cause bugs
		void ApplyInverseToVector(const GLfloat *input3,GLfloat *output3);
		void RotateAboutLocalX(GLfloat theta);
		void RotateAboutLocalY(GLfloat theta);
		void RotateAboutLocalZ(GLfloat theta);
		void RotateAboutGlobalX(GLfloat theta);
		void RotateAboutGlobalY(GLfloat theta);
		void RotateAboutGlobalZ(GLfloat theta);
		//void OrbitAboutGlobalX(GLfloat theta, bool turn);
		void OrbitAboutGlobalY(GLfloat theta, bool turn);
		void OrbitAboutGlobalZ(GLfloat theta, bool turn);
		//void RotateAboutLocalArbitrary(const GLfloat *axis,GLfloat theta);
		void RotateAboutGlobalArbitrary(const GLfloat *axis,GLfloat theta);
		void OrbitAboutGlobalArbitrary(const GLfloat *axis,GLfloat theta,bool turn);
		//void AlignZToVec(const GLfloat *vec3,bool flip);
		void TranslateGlobal(GLfloat x, GLfloat y, GLfloat z) { GLfloat temp[]={x,y,z}; TranslateGlobal(temp); }
		void TranslateGlobal(const GLfloat *offset);
		void TranslateLocalZ(GLfloat amt);
		void TranslateLocalZinXZ(GLfloat amt);
		
		// cfongc for debugging
		void TranslateLocalYinXY(GLfloat amt);
		//void TranslateLocal(const GLfloat *offset);
		void LoadIdentity();
		void SetLoc(const GLfloat *loc);
		const GLfloat *ReadX();
		const GLfloat *ReadY();
		const GLfloat *ReadZ();
		GLfloat *loc;
	private:
		void UpdateX();
		GLfloat matrix[16];
		GLfloat *x;
		GLfloat *y;
		GLfloat *z;
		
		bool xIsUpToDate;
};


#endif
