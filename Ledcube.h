#ifndef _H_LEDCUBE
#define _H_LEDCUBE

#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>

/*
  The copy constructor will just copy all protected data.
*/

#define CUBESIZE 8
#define CUBEON   1
#define CUBEOFF  0

#define PACKETSIZE (CUBESIZE * CUBESIZE +1)

using namespace std;

class LedCube
{
 public:
  LedCube();
  ~LedCube();
  //  LedCube() ( const LedCube &obj);  // copy constructor

  int init();
  int set(int x, int y, int z, int val);
  int get(int x, int y, int z);

  int receiveCube(uint8_t * cubeCharsP); //first byte must be 0xF2

  uint8_t * cubeToCharAlloc(void ); //allocates the array for you.
  uint8_t * cubeToChar(uint8_t * cubeCharsP); //cubeCharsP must be pre-allocated to 65 bytes
  int cubeToFile(const char*  filename);
  int cubeToSerial(); // just calls cubetoFile with the serial port string
  int cubeToCube(LedCube * cubeP);

  int clear();
	      
  int drawCube();

 private:

 protected:
  int m_cube[CUBESIZE][CUBESIZE][CUBESIZE];
  pthread_mutex_t m_mutex;
  
};
#endif //_H_LEDCUBE
