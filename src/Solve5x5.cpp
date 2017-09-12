/*
 * Author Joshua Flank
 * September 2017
 * Basic functionality to solve a 5x5 cube, filled wtih Z's
 *
 * This should solve the puzzle, in around 778 billion Z placements. 
 * If done 1 second per piece, it would take around 25 years
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
#include "Solve5x5.h"
#include "GLcube.h"

extern LedCube * myPortCubeP; //declared in main.cpp

const char * directions[] =
  {
    "enee", //jhf this is a cheat for 'A' but we got to start somewhere 
    //    "uuwu", //uncomment to speed up the process considerably.
    //    "uunu", //uncomment to speed up the process considerably.
    //    "nnen", //uncomment to speed up the process considerably.
    "esee",
    "euee",
    "eene",
    "eese",
    "eeue",

    "unuu", 
    "usuu",
    "ueuu",
    "uwuu",
    "uunu", //This is the B worm - move to top to speed up the processing
    "uusu",
    "uueu",
    "uuwu", //This is the C worm - move to top to speed up the processing
    // cannot go down

    "ewnww",
    "ewsww",
    "ewuww",
    "eewwnw",
    "eewwsw",
    "eewwuw",

    "nsess",
    "nswss",
    "nsuss",
    "nnsses",
    "nnssws",
    "nnssus",

    "nenn",
    "nwnn",
    "nunn",
    "nnen", //This is the D worm - move to top to speed up the processing
    "nnwn",
    "nnun",

    
    ""
  };


int SolveCube::init()
{
  int i,j,k;
  LedCube::init();
  
  for (i = 0; i < 5; i ++){
    for (j = 0; j < 5; j ++){
      for (k = 0; k < 5; k ++){
	m_box[i][j][k] = DEFVAL;
      }
    }
  }
  return 0;
}

int SolveCube::printBox()
{
  int x,y,z;
  //  printf ("----------------------\n");  
  for (z = 0; z < 5; z ++){
    for (y = 0; y < 5; y ++){
      for (x = 0; x < 5; x ++){
	printf("%c", m_box[x][y][z]);
      }
      printf(" ");
    }
    printf ("\n");
  }
  return 0;
}

/*
 * copy from m_box to m_cube, in preparation to copy to another cube.
 */
int SolveCube::shareBox()
{
  int move = 1;
  pthread_mutex_lock( &m_mutex);
  int x,y,z;
  //  printf ("----------------------\n");  
  for (z = 0; z < 5; z ++) {
    for (y = 0; y < 5; y ++) {
      for (x = 0; x < 5; x ++) {
	if (m_box[x][y][z] == DEFVAL) {
	  m_cube[x+move][y+move][z+move] = 0;
	} else {
	  rgb_t rgb;
	  rgb.word = 0;
	  if (m_box[x][y][z] % 2 == 0) {
	    rgb.color.green = 100;
	    rgb.color.red = 10 * (m_box[x][y][z] - BASEWORM);
	  } else {
	    rgb.color.red = 100;
	    rgb.color.green = 10 * (m_box[x][y][z] - BASEWORM);
	  }	    
	  rgb.color.blue =   50 *((m_box[x][y][z] - BASEWORM) % 3) ;
	  // :) convenient that there are 255 colors, and 25 worms... AND 26 chars!
	  m_cube[x+move][y+move][z+move] = rgb.word;
	}
      }
    }
  }
  pthread_mutex_unlock( &m_mutex);
  return 0;
}

int SolveCube::moveWorm (int *xP, int *yP, int *zP, char val, char dir, int erase) 
{
  switch (dir) {
  case 'u':
    *zP = *zP + 1;
    if (*zP > 4) return 1;
    break;
  case 'd':
    *zP = *zP - 1;
    if (*zP < 0) return 1;
    break;
  case 'e':
    *xP = *xP + 1;
    if (*xP > 4) return 1;
    break;
  case 'w':
    *xP = *xP - 1;
    if (*xP < 0) return 1;
    break;
  case 'n':
    *yP = *yP + 1;
    if (*yP > 4) return 1;
    break;
  case 's':
    *yP = *yP - 1;
    if (*yP < 0) return 1;
    break;
  }
  if (m_box[*xP][*yP][*zP] == DEFVAL || m_box[*xP][*yP][*zP] == val) {
    if (erase == 0)
      m_box[*xP][*yP][*zP] = val;
    else 
      m_box[*xP][*yP][*zP] = DEFVAL;
  } else 
    return 1;
  return 0;
}

int SolveCube::addWorm(int x, int y, int z, char val, int trial, int erase)
{
  int i, x1=x,y1=y,z1=z;

  if (erase == 0)
    m_box[x][y][z] = val;
  else
    m_box[x][y][z] = DEFVAL;

  for (i = 0; directions[trial][i] != '\0'; i ++ ) {
    if (moveWorm(&x1, &y1, &z1, val, directions[trial][i], erase) == 1 && erase == 0) {
      erase = 1;
      break;
    }
  }

  return erase;
}

int numiter = 0;
int totaliter = 0;
#define inctotaliter 1000

int numiter2 = 0;
int totaliter2 = 0;
#define inctotaliter2 1000000

int SolveCube::solver(int wormID)
{
  int x,y,z, trial;

  if (wormID == 25) {
    // done!
    shareBox();
    cubeToCube(myGLCubeP);
    totaliter = totaliter * inctotaliter + numiter;
    printf ("totaliter: %d * %d\n", totaliter, inctotaliter);
    return 0;
  }
  numiter ++;
  if (m_speed > 0 && m_speed < 1000000) {
    usleep(1000000 - m_speed);
    shareBox();
    printBox();
    cubeToCube(myGLCubeP); // this may do nothing if GLcube wasn't created.
    cubeToCube(myPortCubeP);// this may do nothing if Portcube wasn't created.
  }
  if (numiter >= inctotaliter) {
    numiter = 0;
    totaliter++;
    shareBox();
    cubeToCube(myGLCubeP); // this may do nothing if GLcube wasn't created.
    cubeToCube(myPortCubeP);// this may do nothing if Portcube wasn't created.
    //      printBox(); //jhf test
    //      drawCube();
  }

  numiter2 ++;
  if (numiter2 == inctotaliter2) {
    totaliter2++;
    printf ("iter: %d * %d\n", totaliter2, inctotaliter2);
    numiter2 = 0;
    printBox();
  }

  for (z = 0; z < 5; z ++) {
    for (y = 0; y < 5; y ++) {
      for (x = 0; x < 5; x ++) {
	if (m_box[x][y][z] == DEFVAL) {
	  for (trial = 0; directions[trial][0] != '\0'; trial ++) {
	    if (addWorm(x,y,z, wormID + BASEWORM, trial, 0) == 1) {
	      addWorm(x,y,z, wormID + BASEWORM, trial, 1);
	      continue;
	    }

	    if (solver(wormID + 1) == 0)
	      return 0;
	    
	    addWorm(x,y,z, wormID + BASEWORM, trial, 1);  
	  }
	  return 1;
	}
      }
    }
  }
  return 1;
}

SolveCube::SolveCube() : LedCube() {
  return;
}


void * mainsolve(void * ptr) 
{
  cout << "Starting SolveCube\n";
  mySolveCubeP->init();
  mySolveCubeP->printBox();
  
  mySolveCubeP->solver(0);
  mySolveCubeP->printBox();
  return 0;
}


