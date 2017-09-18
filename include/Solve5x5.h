#ifndef _H_SOLVE55
#define _H_SOLVE55

#include "Ledcube.h"

#define DEFVAL '.'
#define BASEWORM 'A'

class Solve5 : public LedCube {
 public:
  Solve5();
  ~Solve5();
  
  int init();
  int printBox();
  int solver(int wormID);
  
 private:
  uint64_t inctotaliter;
  uint64_t totaliter;
  uint64_t numiter;

  int addWorm(int x, int y, int z, char val, int trial, int erase);
  int moveWorm (int *xP, int *yP, int *zP, char val, char dir, int erase);
  int shareBox ();
  char m_box[5][5][5] = {{{DEFVAL}}};

};

extern Solve5 * my5CubeP;

void * mainsolve5(void * ptr);

#endif //_H_SOLVE55
