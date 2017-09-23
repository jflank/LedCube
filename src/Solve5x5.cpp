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


int Solve5::init()
{
  int i,j,k;
  LedCube::init();

  numiter   = 0;
  totaliter = 0;

  inctotaliter = 1023;
    
  for (i = 0; i < 5; i ++){
    for (j = 0; j < 5; j ++){
      for (k = 0; k < 5; k ++){
	m_box[i][j][k] = DEFVAL;
      }
    }
  }
  return 0;
}

int Solve5::printBox()
{
  int x,y,z;
  cout << "Total Iterations: " << totaliter << endl;
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
 * save the total iter in binary as well... 
 */
int Solve5::shareBox()
{
  int move = 1;
  pthread_mutex_lock( &m_mutex);
  int x = 0, y = 0, z = 0;
  
  uint64_t bintotal = totaliter;

  for (z = 0; z < 8 && bintotal > 0; z++) {
    for (x = 0; x < 8  && bintotal > 0; x ++) {
      //      cout << bintotal <<" : " << totaliter << endl;
      if (bintotal & 1) {
	m_cube[x][7][7-z] = CUBEON;
      } else {
	m_cube[x][7][7-z] = CUBEOFF;
      }
      bintotal = bintotal >> 1;
    }
  }
  
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

int Solve5::moveWorm (int *xP, int *yP, int *zP, char val, char dir, int erase) 
{
  switch (dir) {
  case 'u':
    *zP = *zP + 1;
    break;
  case 'd':
    *zP = *zP - 1;
    break;
  case 'e':
    *xP = *xP + 1;
    break;
  case 'w':
    *xP = *xP - 1;
    break;
  case 'n':
    *yP = *yP + 1;
    break;
  case 's':
    *yP = *yP - 1;
    break;
  }
  if (*xP < 0 || *xP > 4 ||
      *yP < 0 || *yP > 4 ||
      *zP < 0 || *zP > 4 ) {
    return 1;
  }
  
  
  if (m_box[*xP][*yP][*zP] == DEFVAL || m_box[*xP][*yP][*zP] == val) {
    if (erase == 0)
      m_box[*xP][*yP][*zP] = val;
    else 
      m_box[*xP][*yP][*zP] = DEFVAL;
  } else {
    return 1;
  }
  return 0;
}

int Solve5::addWorm(int x, int y, int z, char val, int trial, int erase)
{
  int i, x1=x,y1=y,z1=z;

  if (x < 0 || x > 5 ||
      y < 0 || y > 5 ||
      z < 0 || z > 5 ||
      val > 'Z') {
    //    cout << "invalid variables to addWorm " << x  << y << z << val << endl;
    return 1;
  }
  
  if (erase == 0)
    m_box[x][y][z] = val;
  else
    m_box[x][y][z] = DEFVAL;

  for (i = 0; directions[trial][i] != '\0'; i ++ ) {
    if (moveWorm(&x1, &y1, &z1, val, directions[trial][i], erase) == 1 &&
	erase == 0) {
      erase = 1;
      break;
    }
  }

  return erase;
}

int Solve5::solver(int wormID)
{
  int x,y,z, trial;

  if (wormID == 25) {
    // done!
    shareBox();
    cubeToReceivers();
    return 0;
  }
  
  // Send to GUI and serial every so often.
  // Speed up the sending if there's a delay
  if (m_speed > 0 && m_speed < 1000) { 
    usleep(1000000 - m_speed * 1000);
    numiter = inctotaliter;
  }
  totaliter++;
  numiter ++; 
  //  cout << "numiter:\t"<< numiter << " totaliter: " << totaliter << endl; 
  if (numiter >= inctotaliter) {
    numiter = 0;
    shareBox();
    printBox();
    cubeToReceivers();
    
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

Solve5::Solve5() : LedCube() {
  return;
}


void * mainsolve5(void * ptr) 
{
  cout << "Starting Solve5\n";
  my5CubeP->init();
  my5CubeP->printBox();
  
  my5CubeP->solver(0);
  my5CubeP->printBox();
  return 0;
}


