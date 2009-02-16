/*
 *  basis.cpp
 *  game
 *
 *  Created by Ia3n Goodfellow on 11/11/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "basis.h"

#include "basis.h"
#include "matrixmath.h"

Basis::Basis()
{
	x=matrix;
	y=matrix+4;
	z=matrix+8;
	loc=matrix+12;
	LoadIdentity();
}

Basis::Basis(const Basis *other): xIsUpToDate(other->xIsUpToDate)
{
	memcpy(matrix,other->matrix,16*sizeof(GLfloat));
	x=matrix;
	y=matrix+4;
	z=matrix+8;
	loc=matrix+12;
}

void Basis::LoadIdentity()
{
	x[0]=1.0f;
	x[1]=0.0f;
	x[2]=0.0f;
	x[3]=0.0f;
	y[0]=0.0f;
	y[1]=1.0f;
	y[2]=0.0f;
	y[3]=0.0f;
	z[0]=0.0f;
	z[1]=0.0f;
	z[2]=1.0f;
	z[3]=0.0f;
	loc[0]=0.0f;
	loc[1]=0.0f;
	loc[2]=0.0f;
	loc[3]=1.0f;
	xIsUpToDate=true;
}

void Basis::ApplyToVector(const GLfloat *input4,GLfloat *output4)
{
	UpdateX();
	for (int i=0;i<4;i++)
	{
		output4[i]=0.0f;
		for (int j=0;j<4;j++)
		{
			output4[i]+=input4[j]*matrix[j*4+i];
		}
	}
}

void Basis::RotateAboutGlobalX(GLfloat theta)
{
	xIsUpToDate=false;
	RotateVectorAboutX(y,theta,y);
	RotateVectorAboutX(z,theta,z);
}

void Basis::RotateAboutGlobalY(GLfloat theta)
{
	xIsUpToDate=false;
	RotateVectorAboutY(y,theta,y);
	RotateVectorAboutY(z,theta,z);
}

void Basis::RotateAboutGlobalZ(GLfloat theta)
{
	xIsUpToDate=false;
	RotateVectorAboutZ(y,theta,y);
	RotateVectorAboutZ(z,theta,z);
}

void Basis::UpdateX()
{
	if (!xIsUpToDate)
	{
		crossproduct(y,z,x);
		xIsUpToDate=true;
	}
}

void Basis::ApplyToGraphics()
{
	UpdateX();
	glMultMatrixf(matrix);
}

void Basis::TranslateGlobal(const GLfloat *offset)
{
	loc[0]+=offset[0];
	loc[1]+=offset[1];
	loc[2]+=offset[2];
}

void Basis::SetLoc(const GLfloat *loc)
{
	memcpy(this->loc,loc,3*sizeof(GLfloat));
}

void Basis::RotateAboutLocalX(GLfloat theta)
{
	UpdateX();//We don't want to tell it to rotate around an x axis that's out of date
	RotateAboutGlobalArbitrary(x,theta);
}

void Basis::RotateAboutLocalY(GLfloat theta)
{
	RotateAboutGlobalArbitrary(y,theta);
}

void Basis::RotateAboutLocalZ(GLfloat theta)
{
	RotateAboutGlobalArbitrary(z,theta);
}

void ApplyToVector(const GLfloat *matrix, const GLfloat *input4,GLfloat *output4)
{
	for (int i=0;i<4;i++)
	{
		output4[i]=0.0f;
		for (int j=0;j<4;j++)
		{
			output4[i]+=input4[j]*matrix[j*4+i];
		}
	}
}

void Basis::RotateAboutGlobalArbitrary(const GLfloat *axis,GLfloat theta)
{
	GLTMatrix rot;
	GLfloat tempy[4],tempz[4];
	gltRotationMatrix(theta,axis[0],axis[1],axis[2],rot);
	::ApplyToVector(rot,y,tempy);
	::ApplyToVector(rot,z,tempz);
	memcpy(y,tempy,3*sizeof(GLfloat));
	memcpy(z,tempz,3*sizeof(GLfloat));
	xIsUpToDate=false;

}

const GLfloat * Basis::ReadY()
{
	return y;
}


const GLfloat * Basis::ReadZ()
{
	return z;
}

//Determinant of a 2x2 matrix
GLfloat dett(GLfloat *matrix,int a, int b, int c, int d)
{
	return matrix[a]*matrix[d]-matrix[b]*matrix[c];
}

void Basis::ApplyInverseToVector(const GLfloat *input3,GLfloat *output3)
{
	pair<GLfloat, GLfloat> polar;
	GLfloat temp[3];
	
	//invert the translation
	diff(input3,loc,temp);


	//Take the determinant of the matrix
	GLfloat det=DetIJ(matrix,3,3);
	
	//Make a 3x3 rotation matrix
	GLfloat rotationMatrix[9];
	UpdateX();
	memcpy(rotationMatrix,x,3*sizeof(GLfloat));
	memcpy(rotationMatrix+3,y,3*sizeof(GLfloat));
	memcpy(rotationMatrix+6,z,3*sizeof(GLfloat));
	
	//Build an inverse matrix out of the determinants of 4-element subsets of the 3x3 matrix
	GLfloat inverseMatrix[9];
	int a11=0,a21=1,a31=2,a12=3,a22=4,a32=5,a13=6,a23=7,a33=8;
	inverseMatrix[0]=dett(rotationMatrix,a22,a23,a32,a33);
	inverseMatrix[1]=dett(rotationMatrix,a23,a21,a33,a31);
	inverseMatrix[2]=dett(rotationMatrix,a21,a22,a31,a32);
	inverseMatrix[3]=dett(rotationMatrix,a13,a12,a33,a32);
	inverseMatrix[4]=dett(rotationMatrix,a11,a13,a31,a33);
	inverseMatrix[5]=dett(rotationMatrix,a12,a11,a32,a31);
	inverseMatrix[6]=dett(rotationMatrix,a12,a13,a22,a23);
	inverseMatrix[7]=dett(rotationMatrix,a13,a11,a23,a21);
	inverseMatrix[8]=dett(rotationMatrix,a11,a12,a21,a22);
	
	//Divide by the determinant of the original matrix
	for (int i=0;i<9;i++)
		inverseMatrix[i]/=det;	//For some reason it works even if you don't divide by the determinant. Odd. I'm still doing the division to be safe.
	
	//Multiply the vector by the 3x3 matrix
	for (int i=0;i<3;i++)
	{
		output3[i]=0.0f;
		for (int j=0;j<3;j++)
		{
			output3[i]+=temp[j]*inverseMatrix[j*3+i];
		}
	}
}

void Basis::OrbitAboutGlobalY(GLfloat theta, bool turn)
{
	RotateVectorAboutY(loc,theta,loc);
	if (turn)
		RotateAboutGlobalY(theta);
}

void Basis::OrbitAboutGlobalZ(GLfloat theta, bool turn)
{
	RotateVectorAboutZ(loc,theta,loc);
	if (turn)
		RotateAboutGlobalZ(theta);
}

void Basis::OrbitAboutGlobalArbitrary(const GLfloat *axis,GLfloat theta,bool turn)
{
	GLTMatrix rot;
	GLfloat tempLoc[4];
	gltRotationMatrix(theta,axis[0],axis[1],axis[2],rot);
	::ApplyToVector(rot,loc,tempLoc);
	memcpy(loc,tempLoc,3*sizeof(GLfloat));
	if (turn)
		RotateAboutGlobalArbitrary(axis,theta);
}

const GLfloat * Basis::ReadX()
{
	UpdateX();
	return x;
}

void Basis::TranslateLocalZ(GLfloat amt)
{
	UpdateX();
	loc[0]+=amt*z[0];
	loc[1]+=amt*z[1];
	loc[2]+=amt*z[2];
}

void Basis::TranslateLocalZinXZ(GLfloat amt)
{
	UpdateX();
	GLfloat dx=z[0], dz=z[2];
	GLfloat scale=amt/len2(dx,dz);
	loc[0]+=scale*dx;
	loc[2]+=scale*dz;
}

// cfongc for debugging
void Basis::TranslateLocalYinXY(GLfloat amt)
{
	UpdateX();
	//GLfloat dx=z[0], dy=z[1];
	//GLfloat scale=amt/len2(dx,dy);
	//loc[0]+=scale*dx;
	loc[1]+=amt;//scale*dy;
}

void Basis::PlaceCam()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(loc[0],loc[1],loc[2],loc[0]+z[0],loc[1]+z[1],loc[2]+z[2],y[0],y[1],y[2]);
}
