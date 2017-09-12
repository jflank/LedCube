#ifndef _H_SERIALCUBE
#define _H_SERIALCUBE

#include "Ledcube.h"


class SerialCube : public LedCube {
 public:
  SerialCube();
  ~SerialCube();
  
  int cubeToSerial(); // just calls cubetoFile with the serial port string

 private:

 protected:
  
  
};

extern SerialCube * myPortCubeP;

//extern void * mainSerial(void * ptr);

#endif //_H_SERIALCUBE
