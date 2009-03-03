#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <assert.h>

#include "PpmUtil.h"

using namespace std;

#define C_BUF  100 // Size of buffer

// hmmmm... no comments in here!  lazy ta!

PpmImg::PpmImg()
{
  width = 0;
  height = 0;
  rgbArray = NULL;
}

PpmImg::~PpmImg()
{
  if(rgbArray)
    delete rgbArray;
}

PpmImg::PpmImg(int width, int height, RGB color)
{
  int i;
  assert(width > 0 && height > 0);
  rgbArray = new RGB[width * height];

  if(!rgbArray){
    cerr << "Failure: Could not allocate memory" << endl;
    width = 0;
    height = 0;
  }else{
    this->width = width;
    this->height = height;

    for(i = 0; i < width*height; i++)
    {
      rgbArray[i].r = color.r;
      rgbArray[i].g = color.g;
      rgbArray[i].b = color.b;
    }
  }
}

PpmImg* PpmImg::Copy() const
{
  PpmImg *newImg = NULL;
  int i;

  if (!this->rgbArray)
    goto FAILURE;

  newImg = new PpmImg();
  if (!newImg)
  {
    cerr << "Failure <PpmImg::Copy> - Could not allocate memory" << endl;
    goto FAILURE;
  }
  newImg->width = width;
  newImg->height = height;

  newImg->rgbArray = new RGB[height * width];

  if (!newImg->rgbArray)
  {
    cerr << "Failure <PpmImg::Copy> - Could not allocate memory" << endl;
    goto FAILURE;
  }

  for(i = 0; i < width * height; i++)
    newImg->rgbArray[i] = rgbArray[i];

  return newImg;

FAILURE:
  if (newImg)
  {
    delete newImg;
    newImg = NULL;
  }
  return NULL;
}

// width is M
// height is N
bool PpmImg::WritePPM(const char *filename)
{
  int i, j;
  int maxval = 255;
  unsigned char *charImage = NULL;
  ofstream ofp;
  RGB pixel;
  bool result = false;

  charImage = (unsigned char *) new unsigned char [3 * width * height];

  if (!charImage)
  {
    cerr << "Failure - Ran out of memory" << endl;
    goto FAILURE;
  }

  for(i = 0; i < height; i++)
    for(j = 0; j < width; j++) {
      pixel = GetRGB(j, i);
      charImage[(i * width + j) * 3 + 0] = (unsigned char)pixel.r;
      charImage[(i * width + j) * 3 + 1] = (unsigned char)pixel.g;
      charImage[(i * width + j) * 3 + 2] = (unsigned char)pixel.b;
    }

    ofp.open(filename, ios::out | ios::binary);

    if (!ofp) 
    {
      cerr << "Failure - Cannot open file: " << filename << endl;
      goto FAILURE;
    }

    ofp << "P6" << endl;
    ofp << width << " " << height << endl;
    ofp << maxval << endl;

    ofp.write( reinterpret_cast<char *>(charImage), (3 * width * height)*sizeof(unsigned char));

    if (ofp.fail()) {
      cout << "Failure - Unable to write image :" << filename << endl;
      goto FAILURE;
    }

    result = true;
    ofp.close();

FAILURE:
    if (charImage)
      delete charImage;
    return result;
}

bool PpmImg::ReadPPM(const char *filename)
{

  int i, j, index;
  int maxval;
  char buffer[C_BUF];  
  unsigned char *charImage = NULL;
  ifstream ifp;
  char *ptr;
  bool result = false;

  ifp.open(filename, ios::in | ios::binary);

  if (!ifp) 
  {
    cout << "Failure - Cannot open " << filename << endl;
    goto FAILURE;
  }

  ifp.getline(buffer, C_BUF, '\n');

  if ((buffer[0]!= 'P') || (buffer[1]!= '6') )
  {
    cout << "Failure - " << filename << " is not PPM" << endl;
    goto FAILURE;
  }

  ifp.getline(buffer, C_BUF, '\n');
  while (buffer[0] == '#')
    ifp.getline(buffer, C_BUF,'\n');

  width = strtol(buffer,&ptr,0);
  height = atoi(ptr);

  ifp.getline(buffer,100,'\n');
  maxval = strtol(buffer,&ptr,0);

  charImage = (unsigned char *) new unsigned char [3*width*height];

  if(charImage == NULL)
  {
    cout << "Failure - Could not allocate memory" << endl;
    goto FAILURE;
  }

  ifp.read( reinterpret_cast<char *>(charImage), (3*width*height)*sizeof(unsigned char));

  if (ifp.fail()) 
  {
    cout << "Image " << filename << " has wrong size" << endl;
    goto FAILURE;
  }

  ifp.close();

  if (rgbArray)
  {
    delete rgbArray;
    rgbArray = NULL;
  }

  rgbArray = new RGB[width*height];

  if (!this->rgbArray)
  {
    cout << "Failure - Could not allocate memory" << endl;
    goto FAILURE;
  }

  /* Convert the unsigned characters to integers */
  // this should now be a guaranteed successful operation
  result = true;

  this->width = width;
  this->height = height;
  for(i=0; i < height; i++)
    for(j=0; j < width; j++) 
    {
      index = (i*width+j);
      rgbArray[index].r = (int)charImage[index * 3 + 0];
      rgbArray[index].g = (int)charImage[index * 3 + 1];
      rgbArray[index].b = (int)charImage[index * 3 + 2];
    }

FAILURE:

    // cleanup if necessary...

    if (charImage) 
      delete [] charImage;

    if (!rgbArray)
    {
      width = 0;
      height = 0;
    }

    return result; 
}


RGB PpmImg::GetRGB(int x, int y) const
{
  RGB result;

  if (x < 0 || y < 0 || x >= width || y >= height)
  {
    result.r = -1;
    result.g = -1;
    result.b = -1;
    cerr << "Error: array bounds exceeded accessing rgb array: " << x << ", " << y << endl;
  } else {
    result = rgbArray[y * width + x];
  }
  return result;
}

bool PpmImg::SetRGB(int x, int y, RGB color)
{
  if (x < 0 || y < 0 || x >= width || y >= height)
  {
    cerr << "Error: array bounds exceeded accessing rgb array: " << x << ", " << y << endl;
    return false;
  } else {
    rgbArray[y * width + x] = color;
    return true;
  } 
}

// x and y are the top left coordinates
void PpmImg::DrawSquare(int x, int y, int length, RGB color)
{
  DrawLine(x, y, length, true, color);
  DrawLine(x, y, length, false, color);
  DrawLine(x + length - 1, y, length, false, color);
  DrawLine(x, y + length - 1, length, true, color);
}

// x and y are the top left coordinates
void PpmImg::DrawFilledSquare(int x, int y, int length, RGB color)
{
  int i, j;
  for (i = 0; i < length; i++)
    for (j = 0; j < length; j++)
      SetRGB(x + i, y + j, color);
}

void PpmImg::DrawLine(int x, int y, int length, bool horizontal, RGB color)
{
  int i;
  if(horizontal)
    for(i = 0; i < length; i++)
      SetRGB(x + i, y, color);
  else
    for(i = 0; i < length; i++)
      SetRGB(x, y + i, color);
}
