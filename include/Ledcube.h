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
  int clear();
  int drawCube();

  int setSpeed(int newSpeed); 
  int getSpeed(); 

  int      set             (int x, int y, int z, uint32_t val);
  uint32_t get             (int x, int y, int z);

  int       receiveByte    (uint8_t * ); //first byte must be 0xF2
  uint8_t * cubeToByte     (uint8_t * ); //cubeCharsP must be pre-allocated to 65 bytes
  uint8_t * cubeToByteAlloc(void );      //allocates the array for you.
  void      coutByte       (const uint8_t * data, int data_length);


  int        receiveRGB     (uint32_t * ); //colorized version sends RGB struct. 
  uint32_t * cubeToRGB       (uint32_t * ); 


  int cubeToCube             (LedCube * cubeP); //just a couple of memcpy's
  int cubeToFile             (const char*  filename);

  
 private:

 protected:
  static const     int PACKETSIZE = (CUBESIZE * CUBESIZE + 1); //only need n^2 bytes
  static const     int COLORPACKETSIZE = (CUBESIZE * CUBESIZE * CUBESIZE *sizeof(uint32_t)); 
  uint32_t         m_cube[CUBESIZE][CUBESIZE][CUBESIZE];
  pthread_mutex_t m_mutex;
  int             m_speed;
};


#endif //_H_LEDCUBE
