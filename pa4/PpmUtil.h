#ifndef _PPMUTIL_H
#define _PPMUTIL_H

/* CS221 Students */
/* Go away.  You don't need to be here ;p
 */

typedef struct
{
  int r;
  int g;
  int b;
} RGB;

class PpmImg
{
public:
  RGB *rgbArray;
  int width;
  int height;

public:
  PpmImg();
  ~PpmImg();
  PpmImg(const char *filename);
  PpmImg(int width, int height, RGB initColor);
  PpmImg *Copy() const;
  bool WritePPM(const char *filename);
  bool ReadPPM(const char *filename);
  RGB GetRGB(int x, int y) const;
  bool SetRGB(int x, int y, RGB color);
  void DrawSquare(int x, int y, int length, RGB color);
  void DrawFilledSquare(int x, int y, int length, RGB color);
  void DrawLine(int x, int y, int length, bool horizontal, RGB color);
};

bool readPPM(char *filename, PpmImg **ptrPpmImg);

#endif
