/*
 *  matrixmath.cpp
 *  game
 *
 *  Created by Ia3n Goodfellow on 11/11/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "matrixmath.h"
#include <math.h>

#define PI 3.1415926535

void makeunitvector(GLfloat *v)
{ makeunitvector(v,v); }

GLfloat len(const GLfloat * loc)
{
	GLfloat ret=0;
	for(int i=0;i<3;i++)
		ret+=loc[i]*loc[i];
	return sqrt(ret);
}

GLfloat len2(const GLfloat * loc)
{
	GLfloat ret=0;
	for(int i=0;i<2;i++)
		ret+=loc[i]*loc[i];
	return sqrt(ret);
}

GLfloat len2(GLfloat x,GLfloat y)
{
	return sqrt(x*x+y*y);
}

GLfloat dist(const GLfloat * loc1, const GLfloat * loc2)
{
	GLfloat temp[3];
	diff(loc1,loc2,temp);
	return len(temp);
}

void diff(const GLfloat *p1, const GLfloat *p2, GLfloat *ans)
{
	ans[0]=p1[0]-p2[0];
	ans[1]=p1[1]-p2[1];
	ans[2]=p1[2]-p2[2];
}

void sum(const GLfloat *p1, const GLfloat *p2, GLfloat *ans)
{
	ans[0]=p1[0]+p2[0];
	ans[1]=p1[1]+p2[1];
	ans[2]=p1[2]+p2[2];
}

//shortcuts
#define V1 v[0]
#define V2 v[1]
#define V3 v[2]
#define W1 w[0]
#define W2 w[1]
#define W3 w[2]
void crossproduct(const GLfloat *v, const GLfloat *w, GLfloat *ans)
{
	ans[0]=V2*W3-V3*W2;
	ans[1]=V3*W1-V1*W3;
	ans[2]=V1*W2-V2*W1;
}

float dotproduct(const GLfloat *v, const GLfloat *w)
{
	//cout<<v[0]<<','<<v[1]<<','<<v[2]<<','<<w[0]<<','<<w[1]<<','<<w[2]<<endl;
	//cout<<v[0]*w[0]<<"+"<<v[1]*w[1]<<"+"<<v[2]*w[2]<<"="<<v[0]*w[0]+v[1]*w[1]+v[2]*w[2]<<endl;
	return v[0]*w[0]+v[1]*w[1]+v[2]*w[2];
}

void normaltotriangle(const GLfloat *p1, const GLfloat *p2, const GLfloat *p3, GLfloat *ans)
{
	GLfloat v1[3],v2[3];
	diff(p1,p2,v1);
	diff(p1,p3,v2);
	crossproduct(v1,v2,ans);
	makeunitvector(ans,ans);
}

void makeunitvector(const GLfloat *input, GLfloat *output)
{
	GLfloat mag=len(input);
	for (int i=0;i<3;i++)
		output[i]=input[i]/mag;
}

void makescaledvector(const GLfloat *input, int scaledmag, GLfloat *output)// input and output can point to same place
{
	GLfloat mag=len(input);
	for(int i=0;i<3;i++)
		output[i]=input[i]*scaledmag/mag;
}

void RotateVectorAboutZ(const GLfloat *input,GLfloat dtheta,GLfloat *output)
{
	if ((input[0]==0.0f)&&(input[1]==0.0f))
		return;
	GLfloat mag=sqrt(input[1]*input[1]+input[0]*input[0]);//horizontal magnitude
	GLfloat thetao=acos(input[0]/mag);
	
	if (input[1]<0)
		thetao=-thetao;
	GLfloat thetaf=thetao+dtheta;
	output[0]=mag*cos(thetaf);
	output[1]=mag*sin(thetaf);
	output[2]=input[2];
}

void RotateVectorAboutY(const GLfloat *input,GLfloat dtheta,GLfloat *output)
{
	if ((input[2]==0.0f)&&(input[0]==0.0f))
		return;
	GLfloat mag=sqrt(input[2]*input[2]+input[0]*input[0]);//magnitude in 2D
	GLfloat thetao=acos(input[0]/mag);
	if (input[2]<0)
		thetao=-thetao;
	GLfloat thetaf=thetao+dtheta;
	output[0]=mag*cos(thetaf);
	output[1]=input[1];
	output[2]=mag*sin(thetaf);
}

void RotateVectorAboutX(const GLfloat *input,GLfloat dtheta,GLfloat *output)
{
	if ((input[1]==0.0f)&&(input[2]==0.0f))
		return;
	GLfloat mag=sqrt(input[2]*input[2]+input[1]*input[1]);//magnitude in 2D
	GLfloat thetao=acos(input[1]/mag);
	if (input[2]<0)
		thetao=-thetao;
	GLfloat thetaf=thetao+dtheta;
	output[0]=input[0];
	output[1]=mag*cos(thetaf);
	output[2]=mag*sin(thetaf);
}

void AlignVecXToVec(const GLfloat *input, const GLfloat *vec, GLfloat *output, bool flip)
{
//Viewing the missile and velocity vector only in the xy plane, rotate them to match
	GLfloat xyvec=sqrt(vec[0]*vec[0]+vec[1]*vec[1]);
	if (xyvec!=0)
	{
		if (vec[1]>0)//acos is clamped to (0,PI) so we must keep in mind it could actually be (PI,TWO_PI)
			RotateVectorAboutZ(input,acos(vec[0]/xyvec),output);
		else
			RotateVectorAboutZ(input,-acos(vec[0]/xyvec),output);
	}
//Now do the same in the xz plane (but now the coordinates of the velocity vector don't match the pre-existing coordinate space)
	GLfloat xzvec=len(vec);
	if (xzvec!=0)
		RotateVectorAboutY(output,-asin(vec[2]/xzvec),output);
		
	if (flip)
		RotateVectorAboutX(output,PI,output);
}

void RotateASuchThatBIsOnX(GLfloat *temp,const GLfloat *aim)
{
	GLfloat xyvec=sqrt(aim[0]*aim[0]+aim[1]*aim[1]);
	cout<<"A"<<endl;
	cout<<temp[0]<<'\t'<<temp[1]<<'\t'<<temp[2]<<endl;
	if (xyvec!=0)
	{
		if (aim[1]>0)//acos is clamped to (0,PI) so we must keep in mind it could actually be (PI,TWO_PI)
			RotateVectorAboutZ(temp,-acos(aim[0]/xyvec),temp);
		else
			RotateVectorAboutZ(temp,acos(aim[0]/xyvec),temp);
	}
	cout<<"B"<<endl;
//Now do the same in the xz plane (but now the coordinates of the velocity vector don't match the pre-existing coordinate space)
	cout<<temp[0]<<'\t'<<temp[1]<<'\t'<<temp[2]<<endl;
	GLfloat xzvec=len(aim);
	cout<<"C"<<endl;
	cout<<temp[0]<<'\t'<<temp[1]<<'\t'<<temp[2]<<endl;
	if (xzvec!=0)
		RotateVectorAboutY(temp,asin(aim[2]/xzvec),temp);
	cout<<"D"<<endl;
}

pair<GLfloat, GLfloat> RectToPolar(GLfloat x,GLfloat y)
{
	if ((x==0.0f)&&(y==0.0f))
		return pair<GLfloat,GLfloat>(0.0f,0.0f);
	GLfloat r=sqrt(x*x+y*y);
	GLfloat theta=acos(x/r);
	if (y<0)
		theta=-theta;
	return pair<GLfloat,GLfloat>(r,theta);
}














//From glTools
///////////////////////////////////////////////////////////////////////////////
// Load a matrix with the Idenity matrix
void gltLoadIdentityMatrix(GLTMatrix m)
	{
	static GLTMatrix identity = { 1.0f, 0.0f, 0.0f, 0.0f,
                                     0.0f, 1.0f, 0.0f, 0.0f,
                                     0.0f, 0.0f, 1.0f, 0.0f,
                                     0.0f, 0.0f, 0.0f, 1.0f };

	memcpy(m, identity, sizeof(GLTMatrix));
	}


///////////////////////////////////////////////////////////////////////////////
// Multiply two 4x4 matricies. Assumes normal OpenGL column major ordering
void gltMultiplyMatrix(const GLTMatrix m1, const GLTMatrix m2, GLTMatrix mProduct )
    {
    mProduct[0]     = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
    mProduct[4]     = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
    mProduct[8]     = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
    mProduct[12]    = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
        
    mProduct[1]     = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
    mProduct[5]     = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
    mProduct[9]     = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
    mProduct[13]    = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
        
    mProduct[2]     = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
    mProduct[6]     = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
    mProduct[10]    = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
    mProduct[14]    = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
        
    mProduct[3]     = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];
    mProduct[7]     = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];
    mProduct[11]    = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];
    mProduct[15]    = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];     
    }

///////////////////////////////////////////////////////////////////////////////
// Creates a 4x4 rotation matrix, takes radians NOT degrees
void gltRotationMatrix(float angle, float x, float y, float z, GLTMatrix mMatrix)
    {
    float vecLength, sinSave, cosSave, oneMinusCos;
    float xx, yy, zz, xy, yz, zx, xs, ys, zs;

    // If NULL vector passed in, this will blow up...
    if(x == 0.0f && y == 0.0f && z == 0.0f)
        {
        gltLoadIdentityMatrix(mMatrix);
        return;
        }

    // Scale vector
    vecLength = (float)sqrt( x*x + y*y + z*z );

    // Rotation matrix is normalized
    x /= vecLength;
    y /= vecLength;
    z /= vecLength;
        
    sinSave = (float)sin(angle);
    cosSave = (float)cos(angle);
    oneMinusCos = 1.0f - cosSave;

    xx = x * x;
    yy = y * y;
    zz = z * z;
    xy = x * y;
    yz = y * z;
    zx = z * x;
    xs = x * sinSave;
    ys = y * sinSave;
    zs = z * sinSave;

    mMatrix[0] = (oneMinusCos * xx) + cosSave;
    mMatrix[4] = (oneMinusCos * xy) - zs;
    mMatrix[8] = (oneMinusCos * zx) + ys;
    mMatrix[12] = 0.0f;

    mMatrix[1] = (oneMinusCos * xy) + zs;
    mMatrix[5] = (oneMinusCos * yy) + cosSave;
    mMatrix[9] = (oneMinusCos * yz) - xs;
    mMatrix[13] = 0.0f;

    mMatrix[2] = (oneMinusCos * zx) - ys;
    mMatrix[6] = (oneMinusCos * yz) + xs;
    mMatrix[10] = (oneMinusCos * zz) + cosSave;
    mMatrix[14] = 0.0f;

    mMatrix[3] = 0.0f;
    mMatrix[7] = 0.0f;
    mMatrix[11] = 0.0f;
    mMatrix[15] = 1.0f;
    }

////////////////////////////////////////////////////////////////////////////
/// This function is not exported by library, just for this modules use only
// 3x3 determinant
float DetIJ(const GLTMatrix m, int i, int j)
    {
    int x, y, ii, jj;
    float ret, mat[3][3];

    x = 0;
    for (ii = 0; ii < 4; ii++)
        {
        if (ii == i) continue;
        y = 0;
        for (jj = 0; jj < 4; jj++)
            {
            if (jj == j) continue;
            mat[x][y] = m[(ii*4)+jj];
            y++;
            }
        x++;
        }

    ret =  mat[0][0]*(mat[1][1]*mat[2][2]-mat[2][1]*mat[1][2]);
    ret -= mat[0][1]*(mat[1][0]*mat[2][2]-mat[2][0]*mat[1][2]);
    ret += mat[0][2]*(mat[1][0]*mat[2][1]-mat[2][0]*mat[1][1]);

    return ret;
    }
