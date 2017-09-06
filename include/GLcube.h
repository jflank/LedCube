#ifndef _H_GLCUBE
#define _H_GLCUBE

#include "Ledcube.h"

#define GLMAXSPEED 1000
#define GLDEFSPEED 900
#define GLMINSPEED 1

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
