#ifndef _H_LEDCUBE
#define _H_LEDCUBE

#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <list>
#include <mutex>
#include <vector>

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
  The copy constructor will just copy m_cube
*/

#define CUBESIZE 8
#define CUBEON   1
#define CUBEOFF  0

using namespace std;

class LedCube
{
 public:
  LedCube     ();
  LedCube     (uint32_t size);
  ~LedCube    ();
  LedCube     (LedCube &obj);  // copy constructor

  int init    (uint32_t size);
  int clear   ();
  int drawCube();

  int       setSpeed       (int newSpeed); 
  int       getSpeed       (); 
  uint32_t  getSize        ();
  
  int       set            (int x, int y, int z, uint32_t val);
  uint32_t  get            (int x, int y, int z);

  int       receiveByte    (uint8_t * ); //first byte must be 0xF2
  uint8_t * cubeToByte     (uint8_t * ); //cubeCharsP must be pre-allocated to 65 bytes
  uint8_t * cubeToByteAlloc(void );      //allocates the array for you.
  void      coutByte       (const uint8_t * data, int data_length);


  int       receiveRGB  (const uint32_t cache[]); //colorized version sends RGB struct.
  uint32_t *cubeToRGB       (uint32_t * ); 


  int       cubeToReceivers ();
  int       cubeAddReceiver (LedCube * cubeP);
  int       cubeToCube      (LedCube * cubeP); //just a couple of memcpy's
  int       cubeToFile      (const char*  filename);

  virtual   int main        ( int argc = 0, char **argv = NULL ) { return 0; } //main isn't used in the base class
  
 private:

 protected:
  uint32_t m_BWPacketSize;      //only need n^2 bytes+ 1 for black and white
  uint32_t m_RGBPacketSize;     //need n^3 bytes * sizeof(RGB)
  vector< vector < vector <uint32_t>>> m_cube; // 3d cube.
  uint32_t         m_size;      // size of one length of the cube.
  mutex            m_mutex;
  int              m_speed;

  list<LedCube*>   m_receivers; // group of cubes to copy to 
};


#endif //_H_LEDCUBE
