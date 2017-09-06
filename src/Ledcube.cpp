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


int LedCube::drawCube() {
  int x,y,z;
  pthread_mutex_lock( &m_mutex );
  for (z = 0; z < CUBESIZE; z ++) {
    for (y = 0; y < CUBESIZE; y ++) {
      for (x = 0; x < CUBESIZE; x ++) {
	printf("%1x", m_cube[x][y][z]);
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
	m_cube[x][y][z] = 0;
      }
    }
  }
  m_mutex = PTHREAD_MUTEX_INITIALIZER;
  return 0;
}

int LedCube::set(int x, int y, int z, int val)
{
  if (x >= CUBESIZE || x < 0 ||
      y >= CUBESIZE || y < 0 ||
      z >= CUBESIZE || z < 0) {
    assert(0);
    return -1;
  }
  pthread_mutex_lock( &m_mutex);
  m_cube[x][y][z] = val;
  pthread_mutex_unlock( &m_mutex);
  return 0;
}

int LedCube::get(int x, int y, int z)
{
  return m_cube[x][y][z];
}

//
// Receive a cube the same as if it was on the led
// this only works with an 8x8x8 cube right now.
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
	  m_cube[x][y][z] = 1;
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
    /* old
    for (int i=0; i < 64; i ++) { // The next 64 char's are the data 
    m_cube[0][i/8][i%8] = cubeCharsP[i+1];
    cout << "Reading: " << i << ":" << (int)m_cube[0][i/8][i%8] << endl;
  }
    */
  pthread_mutex_unlock( &m_mutex);
  return 0;
}

void coutpackage(const uint8_t * data, int data_length)
{
  for(int i=0; i<data_length; ++i) {
    //printf("%02x", data[i]);
    std::cout <<std::bitset<8>(data[i]);
  }
  cout <<endl;
 
}

/* change to byte[] */
uint8_t * LedCube::cubeToChar     (uint8_t * cache)
{
  //  unsigned char * cache = new unsigned char [PACKETSIZE];
  
  memset(cache, 0x00, PACKETSIZE);

  cache[0] = 242; // hex-to-decimal of 0xF2

  //  char *dot = (char*)m_cube;
  pthread_mutex_lock( &m_mutex);


  for (int x = 0; x < CUBESIZE; x++) {
    for (int y = 0; y < CUBESIZE; y++) {
      for (int z = 0; z < CUBESIZE; z++) {
	if (m_cube[x][y][z] == 1) {
	  cache[x+8*y+1] |= 0x01 << z;
	}
      }
    }
  }
  /*  
  for (int i = 0; i < PACKETSIZE-1; i ++) {
    int index = i/8;
    if (dot[i]) {
      cache[index+1] += (0x01 << (i % 8));
    }
    cout << i << ": " << index+1;
    printf(": %02x :", dot[i]);
    std::cout <<std::bitset<8>(cache[index+1]) << endl;

  }
  */
  //jhf  coutpackage(cache, PACKETSIZE);
  /*
  for(z = 0; z < CUBESIZE; z++){
    for(y = 0; y < CUBESIZE; y++){
      for(x = 0; x < CUBESIZE; x++){
	if(m_cube[x][y][z] == 1) {
	  cubeCharsP[i] |= 0x01<<((CUBESIZE-1)-x);
	}
      }
      i++;
    }
  }
  */
  pthread_mutex_unlock( &m_mutex);
  return cache;
}

uint8_t * LedCube::cubeToCharAlloc(void)
{
  uint8_t * cubeCharsP = new uint8_t [PACKETSIZE];
  cubeToChar(cubeCharsP);
  return cubeCharsP;
}

//this is a little inefficient, but it's better to have two copies than deal with mutex management right now.

int LedCube::cubeToCube(LedCube * cubeP)
{
  uint8_t buffer[PACKETSIZE];
  cubeToChar(buffer);

  cubeP->receiveCube(buffer);
  return 0;
}


// just write to serial port "/dev/ttyUSB1" instead of file to send this to serial port
int LedCube::cubeToFile(const char * filename)
{
   ofstream outfile;
   outfile.open(filename, ios::out | ios::binary);
   uint8_t buffer[PACKETSIZE];
   cubeToChar(buffer);

   outfile.write((const char*)buffer, PACKETSIZE);
   return 0;
}

int LedCube::cubeToSerial()
{
  return cubeToFile("/dev/ttyUSB1");
}

int LedCube::clear()
{
    memset(m_cube, 0, sizeof(m_cube));
    return 0;
}
  
