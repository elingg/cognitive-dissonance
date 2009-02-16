#ifndef _texture_h_12345_
#define _texture_h_12345_

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


GLuint LoadGLTexture( const char *filename );						// Load Bitmaps And Convert To Textures


#endif
