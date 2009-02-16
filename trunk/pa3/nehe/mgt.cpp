#include "mgt.h"
#include "vmath.h"
#include <stdio.h>
#include <iostream>
#include "MilkshapeModel.h"
using namespace std;

#define WIDTH 900
#define HEIGHT 900

GLfloat viewrotation[16];

MilkshapeModel * carModel;


void draw_car()
{
 carModel->draw();
}

void initialize()
{
	init_opengl();

       carModel = new MilkshapeModel();									// Memory To Hold The Model
	if ( carModel->loadModelData( "nehe/data/f360.ms3d" ) == false )		// Loads The Model And Checks For Errors
	{
	cerr<<"Couldn't load the model nehe/data\\f360.ms3d"<<endl;
	exit(0);												// If Model Didn't Load Quit
	}
}

void init_lighting() {
  glLightfv(GL_LIGHT0, GL_POSITION, Vector4f(0.0,0.0,1.0,1.0));
  glLightfv(GL_LIGHT0, GL_SPECULAR, Vector4f(0.1,0.1,0.1,1.0));

  glLightfv(GL_LIGHT1, GL_DIFFUSE, Vector4f(0.2,0.2,0.2,1.0)); //white
  glLightfv(GL_LIGHT1, GL_AMBIENT, Vector4f(0.2,0.2,0.2,1.0)); //grey?
  glLightfv(GL_LIGHT1, GL_SPECULAR, Vector4f(0.1,0.1,0.1,1.0)); //white

  //glLightfv(GL_LIGHT1, GL_POSITION, Vector4f(0.0,LIGHTHEIGHT,GAMEZDEPTH,1.0));
  //glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, Vector4f(0.0,-1.0,0.0,1.0));
  
  glEnable(GL_RESCALE_NORMAL);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_DEPTH_TEST);
  
}

void init_opengl() {
  //rotDir1 = rotDir2 = 0;

  //openGL init code goes here
  glEnable(GL_POLYGON_SMOOTH);
  
  //glBlendFunc(GL_DST_ALPHA, GL_ONE);

  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glPolygonMode(GL_FRONT, GL_FILL);
  glPolygonMode(GL_BACK, GL_NONE);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  //glPolygonMode(GL_BACK, GL_POINT);
  glShadeModel(GL_SMOOTH);
  
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glGetFloatv(GL_MODELVIEW_MATRIX, viewrotation);

  printf("About to run lighting code\n");
  init_lighting();

  glClearColor(0.4,0.4,0.4,1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glutSwapBuffers();
}
