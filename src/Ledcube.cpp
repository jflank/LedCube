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
#include <errno.h>
#include <iostream>
#include "assert.h"
#include <bitset>

#include "Ledcube.h"

using namespace std;

LedCube::LedCube ()
{
  m_speed = 0; // default speed is human visible
  init();
}

LedCube::~LedCube ()
{
  //nothing to be done in destructor yet.
}

/*
 * copy constructer just copies and uses the c++11 locking mechanism to avoid 
 * deadlocks.
 */
LedCube::LedCube( LedCube &obj)
{
  m_speed = 0; // default speed is human visible

  unique_lock<mutex> lock1(obj.m_mutex, std::defer_lock);
  unique_lock<mutex> lock2(    m_mutex, std::defer_lock);

  // lock both unique_locks without deadlock
  lock(lock1, lock2);

  memcpy(m_cube, obj.m_cube, sizeof(m_cube));
  
}

/* only works in monochrome */
int LedCube::drawCube() {
  int x,y,z;
  unique_lock<mutex> lock(m_mutex);
  for (z = 0; z < CUBESIZE; z ++) {
    for (y = 0; y < CUBESIZE; y ++) {
      for (x = 0; x < CUBESIZE; x ++) {
	cout << (m_cube[x][y][z] > 0 ? "1":"0");
      }
      cout  << " ";
    }
    cout << endl;
  }

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
  //  m_mutex = PTHREAD_MUTEX_INITIALIZER;
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
  unique_lock<mutex> lock(m_mutex);  
  m_cube[x][y][z] = val;
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
  unique_lock<mutex> lock(m_mutex);
  uint32_t ret = m_cube[x][y][z];
  return ret;
}

/*
 * Receive a cube the same as if it was on the monochrome led
 * this only works with an 8x8x8 cube right now.
 */
int LedCube::receiveByte(uint8_t * buffer)
{
  if (buffer == 0) {
    return -1;
  }

  if (buffer[0] != 0xF2) { // first byte must be 0xF2 to be read correctly.
    return -1;
  }
  unique_lock<mutex> lock(m_mutex);

  memset(m_cube, 0, sizeof(m_cube));

  for (int x = 0; x < CUBESIZE; x++) {
    for (int y = 0; y < CUBESIZE; y++) {
      for (int z = 0; z < CUBESIZE; z++) {
	if ((buffer[x+8*y+1] & (0x01 << z))) {
	  m_cube[x][y][z] = CUBEON;
	}
      }
    }
  }
  return 0;
}

/*
 * color version, without any headers
 */
int LedCube::receiveRGB(uint32_t * cache)
{
  if (cache == 0) {
    return -1;
  }

  unique_lock<mutex> lock(m_mutex);

  memcpy(m_cube, cache, COLORPACKETSIZE);

  return 0;
}


/*
 * print out the monochrome byte stream
 */
void LedCube::coutByte(const uint8_t * data, int data_length)
{
  for(int i=0; i<data_length; ++i) {
    cout <<std::bitset<8>(data[i]);
  }
  cout <<endl;
 
}

// todo: add a registration capability to register cubes to each other, rather 
// than hard-code it in each file

/* 
 * change to monochrome byte stream 
 */
uint8_t * LedCube::cubeToByte      (uint8_t * cache)
{
  memset(cache, 0x00, PACKETSIZE);

  cache[0] = 242; // hex-to-decimal of 0xF2

  unique_lock<mutex> lock(m_mutex);

  for (int x = 0; x < CUBESIZE; x++) {
    for (int y = 0; y < CUBESIZE; y++) {
      for (int z = 0; z < CUBESIZE; z++) {
	if (m_cube[x][y][z] > 0) { 
	  cache[x+8*y+1] |= 0x01 << z;
	}
      }
    }
  }
  return cache;
}

/* 
 * change to uint32_t byte stream without a header
 */
uint32_t * LedCube::cubeToRGB   (uint32_t * cache)
{
  if (cache == NULL) {
    return NULL;
  }

  unique_lock<mutex> lock(m_mutex);

  memcpy(cache, m_cube, COLORPACKETSIZE);

  return cache;
}

uint8_t * LedCube::cubeToByteAlloc(void)
{
  uint8_t * buffer = new uint8_t [PACKETSIZE];
  cubeToByte(buffer);
  return buffer;
}

/* 
 * Periodically, push this cube to a series of receiving cubes.
 */
int LedCube::cubeToReceivers()
{
  for (std::list<LedCube*>::iterator it=m_receivers.begin();
       it != m_receivers.end();
       ++it) {
    cubeToCube(*it);
  }
}  

int LedCube::cubeAddReceiver(LedCube *cubeP)
{
  m_receivers.push_back(cubeP);
  return 0;
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
  cubeToRGB        (buffer);
  cubeP->receiveRGB(buffer);
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
   cubeToByte(buffer);

   outfile.write((const char*)buffer, PACKETSIZE);
   return 0;
}

int LedCube::clear()
{
    memset(m_cube, 0, sizeof(m_cube));
    return 0;
}
  
