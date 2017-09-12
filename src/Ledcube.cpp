/*
 * Author Joshua Flank
 * September 2017
 * the purpose of this file is to just model the 8x8x8 cube 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <pthread.h>
#include <errno.h>
#include <iostream>
#include "Ledcube.h"
#include "assert.h"
#include <bitset>

using namespace std;

LedCube::LedCube () {
  m_speed = 0; // default speed is human visible
  init();
}

/* don't want copy constructor yet
LedCube::LedCube(const LedCube &obj)
{
  pthread_mutex_lock( obj.&m_mutex );
  memcpy(m_cube, obj.m_cube);
  pthread_mutex_unlock( obj.&m_mutex );
  
  m_mutex = PTHREAD_MUTEX_INITIALIZER;

}
*/
/* only works in monochrome */
int LedCube::drawCube() {
  int x,y,z;
  pthread_mutex_lock( &m_mutex );
  for (z = 0; z < CUBESIZE; z ++) {
    for (y = 0; y < CUBESIZE; y ++) {
      for (x = 0; x < CUBESIZE; x ++) {
	cout << (m_cube[x][y][z] > 0 ? "1":"0");
      }
      cout  << " ";
    }
    cout << endl;
  }
  pthread_mutex_unlock( &m_mutex );

  return 0;
}

int LedCube::init()
{
  int x,y,z;
  for (x = 0; x < CUBESIZE; x ++) {
    for (y = 0; y < CUBESIZE; y ++) {
      for (z = 0; z < CUBESIZE; z ++) {
	m_cube[x][y][z] = CUBEOFF;
      }
    }
  }
  m_mutex = PTHREAD_MUTEX_INITIALIZER;
  return 0;
}

/*
  if invalid coordinates, assert, or just return 0;
*/
int LedCube::set(int x, int y, int z, uint32_t val)
{
  if (x >= CUBESIZE || x < 0 ||
      y >= CUBESIZE || y < 0 ||
      z >= CUBESIZE || z < 0) {
    assert(0);
    return 0;
  }
  pthread_mutex_lock( &m_mutex);
  m_cube[x][y][z] = val;
  pthread_mutex_unlock( &m_mutex);
  return 0;
}
/*
 *  if invalid coordinates, assert, or just return 0;
 */
uint32_t LedCube::get(int x, int y, int z)
{
  if (x >= CUBESIZE || x < 0 ||
      y >= CUBESIZE || y < 0 ||
      z >= CUBESIZE || z < 0) {
    assert(0);
    return 0;
  }
  pthread_mutex_lock( &m_mutex);
  uint32_t ret = m_cube[x][y][z];
  pthread_mutex_unlock( &m_mutex);
  return ret;
}

/*
 * Receive a cube the same as if it was on the monochrome led
 * this only works with an 8x8x8 cube right now.
 */
int LedCube::receiveCube(uint8_t * cache)
{
  if (cache == 0) {
    return -1;
  }

  if (cache[0] != 0xF2) { // first byte must be 0xF2 to be read correctly.
    return -1;
  }
  pthread_mutex_lock( &m_mutex);

  memset(m_cube, 0, sizeof(m_cube));

  for (int x = 0; x < CUBESIZE; x++) {
    for (int y = 0; y < CUBESIZE; y++) {
      for (int z = 0; z < CUBESIZE; z++) {
	if ((cache[x+8*y+1] & (0x01 << z))) {
	  m_cube[x][y][z] = CUBEON;
	}
      }
    }
  }
  /*
  int * dot = (int*)m_cube;

  for (int i = 0; i < PACKETSIZE-1; i ++) {
    int index = i/8;
    if (cache[index+1] & (0x01 << (i % 8))) {
      dot[i] = 1;
    }
  }
  */    
  pthread_mutex_unlock( &m_mutex);
  return 0;
}

/*
 * color version, without any headers
 */
int LedCube::receiveColorCube(uint32_t * cache)
{
  if (cache == 0) {
    return -1;
  }

  pthread_mutex_lock( &m_mutex);

  memcpy(m_cube, cache, COLORPACKETSIZE);

  pthread_mutex_unlock( &m_mutex);
  return 0;
}


/*
 * print out the monochrome byte stream
 */
void coutpackage(const uint8_t * data, int data_length)
{
  for(int i=0; i<data_length; ++i) {
    cout <<std::bitset<8>(data[i]);
  }
  cout <<endl;
 
}

/* 
 * change to monochrome byte stream 
 */
uint8_t * LedCube::cubeToChar      (uint8_t * cache)
{
  memset(cache, 0x00, PACKETSIZE);

  cache[0] = 242; // hex-to-decimal of 0xF2

  pthread_mutex_lock( &m_mutex);

  for (int x = 0; x < CUBESIZE; x++) {
    for (int y = 0; y < CUBESIZE; y++) {
      for (int z = 0; z < CUBESIZE; z++) {
	if (m_cube[x][y][z] > 0) { 
	  cache[x+8*y+1] |= 0x01 << z;
	}
      }
    }
  }
  pthread_mutex_unlock( &m_mutex);
  return cache;
}

/* 
 * change to uint32_t byte stream without a header
 */
uint32_t * LedCube::cubeToColorChar   (uint32_t * cache)
{
  if (cache == NULL) {
    return NULL;
  }

  pthread_mutex_lock( &m_mutex);

  memcpy(cache, m_cube, COLORPACKETSIZE);

  pthread_mutex_unlock( &m_mutex);
  return cache;
}

uint8_t * LedCube::cubeToCharAlloc(void)
{
  uint8_t * cubeCharsP = new uint8_t [PACKETSIZE];
  cubeToChar(cubeCharsP);
  return cubeCharsP;
}

/*
 * This is a little inefficient, but it's better to have two copies than deal 
 * with mutex management right now.
 * Also, may want to just overload = operator...
 *
 * This should work with monochrome and color, as long as the byte string is 
 * set correctly.
 */
int LedCube::cubeToCube(LedCube * cubeP)
{
  uint32_t buffer[PACKETSIZE*CUBESIZE];
  if (cubeP == NULL) {
    return 1;
  }
  cubeToColorChar        (buffer);
  cubeP->receiveColorCube(buffer);
  return 0;
}

// default speeds should be human-visible
int LedCube::setSpeed(int newSpeed)
{
  int oldSpeed = m_speed;
  m_speed = newSpeed;
  return oldSpeed;
}

int LedCube::getSpeed()
{
  return m_speed;
}

/* 
 * Write cube in binary format to a file.
 */
int LedCube::cubeToFile(const char * filename)
{
   ofstream outfile;
   if (filename == NULL) {
     return 1;
   }
   outfile.open(filename, ios::out | ios::binary);
   uint8_t buffer[PACKETSIZE];
   cubeToChar(buffer);

   outfile.write((const char*)buffer, PACKETSIZE);
   return 0;
}

int LedCube::clear()
{
    memset(m_cube, 0, sizeof(m_cube));
    return 0;
}
  
