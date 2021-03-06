#ifndef _H_SERIALCUBE
#define _H_SERIALCUBE

#include "Ledcube.h"


class SerialCube : public LedCube {
 public:
  SerialCube      ();
  ~SerialCube     ();

  void init       ();
  int cubeToSerial(); // just calls cubetoFile with the serial port string
  
  int  main       (int argc, char **argv);
  
 private:
  
 protected:
  
  const char * devicename;
  long BAUD;                      // derived baud rate from command line
  long DATABITS;                  // one byte
  long STOPBITS;
  long PARITYON;
  long PARITY;  // 00 = NONE, 01 = Odd, 02 = Even, 03 = Mark, 04 = Space
  int fd;
 
};

#endif //_H_SERIALCUBE
