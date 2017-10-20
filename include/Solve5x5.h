#ifndef _H_SOLVE55
#define _H_SOLVE55

#include "Ledcube.h"

#define DEFVAL '.'
#define BASEWORM 'A'

class Solve5 : public LedCube {
 public:
  Solve5      ();
  ~Solve5     ();
  
  int init    ();
  int printBox();
  int solver  (int wormID);

  int main    (int argc, char **argv);

 private:
  uint64_t inctotaliter;
  uint64_t totaliter;
  uint64_t numiter;
  char     m_box[5][5][5] = {{{DEFVAL}}};

  int addWorm  (int x, int y, int z, char val, int trial, int erase);
  int moveWorm (int *xP, int *yP, int *zP, char val, char dir, int erase);
  int shareBox ();

};

#endif //_H_SOLVE55
