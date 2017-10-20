#ifndef _H_GLCUBE
#define _H_GLCUBE

#include "Ledcube.h"

#define GLMAXSPEED 1000
#define GLDEFSPEED 900
#define GLMINSPEED 1

class GLCube : public LedCube {
 public:
  GLCube           ();
  GLCube           (uint32_t size);
  ~GLCube          ();
  
  void drawSpheres (void);
  void rotateCube  (void);
  void clickLED    (int mouseX, int mouseY, int bOn);
  void keyboard    (unsigned char key, int x, int y);
  int  main        ( int argc = 0, char **argv = NULL );
  
 private:
  void init();
  
  double m_pixelDist;
  double m_pixelSize;
  double m_trans;
  double m_sphereSize;
  double m_rot_z;
  double m_rot_y;
  double m_rot_x;

 protected:
  
  
};

extern GLCube     * myGLCubeP;

#endif //_H_GLCUBE
