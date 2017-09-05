#ifndef _H_GLCUBE
#define _H_GLCUBE

#include "Ledcube.h"

class GLCube : public LedCube {
 public:
  GLCube();
  ~GLCube();

  void drawSpheres();
  
 private:

 protected:
  
};

extern GLCube * myGLCubeP;

extern void * mainGL(void * ptr);

#endif //_H_GLCUBE
