#ifndef _H_SOLVE55
#define _H_SOLVE55

#include "Ledcube.h"

#define DEFVAL '.'
#define BASEWORM 'A'

class SolveCube : public LedCube {
 public:
  SolveCube();
  ~SolveCube();
  
  int init();
  int printBox();
  int solver(int wormID);
  
 private:
  int addWorm(int x, int y, int z, char val, int trial, int erase);
  int moveWorm (int *xP, int *yP, int *zP, char val, char dir, int erase);
  int shareBox ();
  char m_box[5][5][5] = {{{DEFVAL}}};
};

void * mainsolve(void * ptr);

#endif //_H_SOLVE55
