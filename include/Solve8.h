#ifndef _H_SOLVE8
#define _H_SOLVE8

#include "Ledcube.h"

#define DEFVAL '.'
#define BASEWORM 'A'

class Solve8Cube : public LedCube {
 public:
  Solve8Cube    ();
  ~Solve8Cube   ();
  
  int  init     ();
  void printSol ();
  bool backtrack(int& column);
  int  shareBox ();
  int  main     (int argc, char **argv);
  
 private:
  bool**      spaces;
  vector<int> state;
  int         size;
  
  bool isSafe  (int row, int column);

};

#endif //_H_SOLVE8
