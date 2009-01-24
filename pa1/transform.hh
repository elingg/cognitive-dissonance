#ifndef TRANSFORM_HH
#define TRANSFORM_HH

#include <stdio.h>
#include <math.h>
#include <stdio.h>

class Transform
{
public:
  //default constructor and destructor
  Transform() {
    setIdentity();
  };
  ~Transform() {};
  
  void operator=(Transform rhs){
    for(int r = 0; r < 4; r++) 
      for(int c = 0; c < 4; c++)
        matrix[r][c] = rhs.matrix[r][c];
  };
  
  inline void print(char *str) {
    fprintf(stderr, "%s:\n", str);
    for(int r = 0; r < 4; r++) {
      for(int c = 0; c < 4; c++)
        fprintf(stderr, "%8.2f ", matrix[r][c]);
      fprintf(stderr, "\n");
    }
    fprintf(stderr, "%s:\n", str);
  };
  
  inline void setIdentity() {
    identityMatrix(matrix);
  };

  inline void rotateX(float theta) {
    float temp[4][4];
    float ctheta = cos(theta), stheta = sin(theta);
    
    identityMatrix(temp);
    temp[1][1] = ctheta; 
    temp[1][2] = -stheta;
    temp[2][1] = stheta;
    temp[2][2] = ctheta;
    rightMultiply(temp);
  };

  inline void rotateY(float theta) {
    float temp[4][4];
    float ctheta = cos(theta), stheta = sin(theta);
    
    identityMatrix(temp);
    temp[0][0] = ctheta; 
    temp[0][2] = stheta;
    temp[2][0] = -stheta;
    temp[2][2] = ctheta;
    rightMultiply(temp);
  };

  inline void rotateZ(float theta) {
    float temp[4][4];
    float ctheta = cos(theta), stheta = sin(theta);
    
    identityMatrix(temp);
    temp[0][0] = ctheta; 
    temp[0][1] = -stheta;
    temp[1][0] = stheta;
    temp[1][1] = ctheta;
    rightMultiply(temp);
  };

  inline void translate(float x, float y, float z){
    transform(&x, &y, &z);

    matrix[0][3] = x;
    matrix[1][3] = y;
    matrix[2][3] = z;
  };

  inline void transform(float *x, float *y, float *z){
    float v1[3], v2[3];
    int r, c;
    
    v1[0] = *x;
    v1[1] = *y;
    v1[2] = *z;
    for(r = 0; r < 3; r++) {
      v2[r] = 0;
      for(c = 0; c < 3; c++)
        v2[r] += matrix[r][c] * v1[c];
      v2[r] += matrix[r][3];
    }
    *x = v2[0];
    *y = v2[1];
    *z = v2[2];    
  };

  inline void transform(dVector3 p){
    float x, y, z;
    transform(&x, &y, &z);
    p[0] = x;
    p[1] = y;
    p[2] = z;
  }

  inline void getRotation(dMatrix3 R){
    for (int i = 0; i < 3; i++){
      for (int j = 0; j < 3; j++){
        R[4*i + j] = matrix[i][j];
      }
    }
    
  }
  
private:
  float matrix[4][4];

  inline void rightMultiply(float t2[4][4]) {
    float result[4][4];
    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++) {
        result[i][j] = 0;
        for(int k = 0; k < 4; k++)
          result[i][j] += matrix[i][k] * t2[k][j];
      }
    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++)
        matrix[i][j] = result[i][j];
  };

  inline void leftMultiply(float t2[4][4]) {
    float result[4][4];
    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++) {
        result[i][j] = 0;
        for(int k = 0; k < 4; k++)
          result[i][j] += t2[i][k] * matrix[k][j];
      }
    for(int i = 0; i < 4; i++)
      for(int j = 0; j < 4; j++)
        matrix[i][j] = result[i][j];
  };

  inline void identityMatrix(float m[4][4]){
    for(int r = 0; r < 4; r++)
      for(int c = 0; c < 4; c++)
        if(r == c)
          m[r][c] = 1;
        else
          m[r][c] = 0;   
  };
};

#endif
