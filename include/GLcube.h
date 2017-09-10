#ifndef _H_GLCUBE
#define _H_GLCUBE

#include "Ledcube.h"

#define GLMAXSPEED 1000
#define GLDEFSPEED 900
#define GLMINSPEED 1

// 8 pixels - the distance from pixel0 to pixel7 is 1/(cubesize-1)
static const double CUBEPIXELDIST = (CUBESIZE-1);

class GLCube : public LedCube {
 public:
  GLCube();
  ~GLCube();
  int setSpeed(int speed);
  
  void drawSpheres();
  
 private:

 protected:
  
  
};

extern GLCube * myGLCubeP;

extern void * mainGL(void * ptr);

#endif //_H_GLCUBE
