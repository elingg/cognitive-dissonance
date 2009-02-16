#include "simulation.h"
#include "carmodel.h"
#include "graphics.h"
#include "nehe/mgt.h"
#include "basis.h"

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


double carX;
double carY;
double carTheta;

Basis camera;
Basis car;

#define SCALE 10.0

void TimerFunc(int unused);

void InitGraphics(int argc, char* argv[])
{  
  
  InitGlut(argc, argv);
       
  initialize();

  camera.TranslateGlobal(0,60.0,-180.0); 
  camera.RotateAboutGlobalX(20.0/360.0 * 6.28);
}

void LaunchGUI()
{
  glutMainLoop();
}

void UpdateCar(double x, double y, double theta)
{
  car.TranslateGlobal((y-carY) * SCALE, 0.0, (x-carX) * SCALE);
  car.RotateAboutGlobalY(theta-carTheta);

 carX = x;
 carY = y;
 carTheta = theta;
}

void Render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  camera.PlaceCam();

  //  glColor3f(0.0,1.0,0.0);

  glPushMatrix();
  car.ApplyToGraphics();
  glEnable(GL_TEXTURE_2D);
  draw_car();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  glDepthFunc(GL_LEQUAL);
  
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  glColor3f(.1,.1,.1);
  glBegin(GL_QUADS);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(100.0,-15.0,-50000.0);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(-100.0,-15.0,-50000.0);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(-100.0,-15.0,50000.0);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(100.0,-15.0,50000.0);
  glEnd();

  glPolygonOffset(-0.15, -10);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glColor4f(1,1,0,1);
  glBegin(GL_QUADS);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(100.0,-15.0,-50000.0);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(95.0,-15.0,-50000.0);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(95.0,-15.0,50000.0);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(100.0,-15.0,50000.0);
  glEnd();  

  glBegin(GL_QUADS);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(-95.0,-15.0,-50000.0);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(-100.0,-15.0,-50000.0);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(-100.0,-15.0,50000.0);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(-95.0,-15.0,50000.0);
  glEnd();  
  glDisable(GL_POLYGON_OFFSET_FILL);

  printf("Car at %f,%f,%f\n",carX,carY,carTheta);
  glutSwapBuffers();
}

void KeyboardFunc(unsigned char key, int x, int y)
{
  switch (key)
    {
    case 'q':
      exit(0);
      break;

    case 's':
      camera.TranslateLocalZinXZ(-5.0);
      glutPostRedisplay();
      break;

    case 'w':
	camera.TranslateLocalZinXZ(5.0);
        glutPostRedisplay();
      break;

    case 'a':
      camera.RotateAboutGlobalY(-0.1);
        glutPostRedisplay();
      break;

    case 'd':
      camera.RotateAboutGlobalY(0.1);
        glutPostRedisplay();
      break;

    case 'g':
      TimerFunc(0);
      break;
    }
}

void ReshapeFunc(int w, int h)
{
 if (h == 0)
	 h = 1;
 
  glViewport(0,0,w,h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, (float) w / (float) h, 10.0, 20000.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void TimerFunc(int unused)
{
  glutPostRedisplay();
  UpdateState();
  //Sleep for about DT seconds between frames
  glutTimerFunc((int) (DT * 300), TimerFunc, 0); // HACKED
}

void InitGlut(int argc, char* argv[])
{
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
 glutCreateWindow("Car Simulator"); 
 glutReshapeFunc(ReshapeFunc);
 glutKeyboardFunc(KeyboardFunc);
 glutDisplayFunc(Render);
}
