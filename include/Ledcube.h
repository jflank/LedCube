#ifndef _H_LEDCUBE
#define _H_LEDCUBE

#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>

typedef union {
  struct {
    uint8_t  blue;
    uint8_t  green;
    uint8_t  red;
  } color;
  uint32_t word;
  uint8_t  byte[3];
} rgb_t;

/*
  The copy constructor will just copy all protected data.
*/

#define CUBESIZE 8
#define CUBEON   1
#define CUBEOFF  0

using namespace std;

class LedCube
{
 public:
  LedCube();
  ~LedCube();
  //  LedCube() ( const LedCube &obj);  // copy constructor

  int init();
  int set(int x, int y, int z, uint32_t val);
  uint32_t get(int x, int y, int z);

  int receiveCube     (uint8_t * cubeCharsP); //first byte must be 0xF2
  int receiveColorCube(uint32_t * bufferP); //first byte must be 0xF2

  uint8_t * cubeToCharAlloc(void ); //allocates the array for you.
  uint8_t * cubeToChar     (uint8_t * cubeCharsP); //cubeCharsP must be pre-allocated to 65 bytes
  uint32_t * cubeToColorChar (uint32_t * bufferP); 
  int cubeToCube             (LedCube * cubeP); //just a couple of memcpy's

  int cubeToFile  (const char*  filename);

  int clear();
	      
  int drawCube();
  
  int setSpeed(int newSpeed); 
  int getSpeed(); 
  
 private:

 protected:
  static const     int PACKETSIZE = (CUBESIZE * CUBESIZE + 1); //only need n^2 bytes
  static const     int COLORPACKETSIZE = (CUBESIZE * CUBESIZE * CUBESIZE *sizeof(uint32_t)); 
  uint32_t         m_cube[CUBESIZE][CUBESIZE][CUBESIZE];
  pthread_mutex_t m_mutex;
  int             m_speed;
};


#endif //_H_LEDCUBE
