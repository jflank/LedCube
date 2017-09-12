/*
 * Author Joshua Flank
 * September 2017
 * Shows off the LED cube. This also solves the 5x5x5 Z cube puzzle
 */
#include "easylogging++.h"
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
#include "Ledcube.h"
#include "Serialcube.h"
#include "GLcube.h"
#include "Solve5x5.h"
#include "Solve8.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;

static void show_usage()
{
  cerr << "Usage: " << endl;
  cout << "Options:\n"
       << "\t-h\tShow this help message\n"
       << "\t-g\tSee The OpenGL threads only\n"
       << "\t-a\tSet the solver speed (-a 1 is around 1 piece per second)\n"
       << "\t-s\tSee The 5x5 solver in action\n"
       << "\t-e\tSee The 8 queens solver in action\n"
       << "\t-p\tSend data over the serial port\n"
       << endl;
}

SerialCube * myPortCubeP  = NULL;
SolveCube  * mySolveCubeP = NULL;
GLCube     * myGLCubeP    = NULL;
Solve8Cube * my8CubeP     = NULL;

int main(int argc, char *argv[])
{

  pthread_t thread1, thread2, thread3;
  int ret;
  int gflag = 0;
  int aflag = 0;
  int eflag = 0;
  int sflag = 0;
  int tflag = 0;
  int pflag = 0;
  char *avalue = NULL;
  int c;

  opterr = 0;

  el::Configurations conf("./log.conf");
  el::Loggers::reconfigureAllLoggers(conf);

  LOG(INFO) << "Starting" ;
  while ((c = getopt (argc, argv, "egstpa:")) != -1) {
    switch (c) {
    case 'g':
      gflag = 1;
      break;
    case 's':
      sflag = 1;
      break;
    case 'e':
      eflag = 1;
      break;
    case 'a':
      avalue = optarg;
      break;
    case 't':
      tflag = 1;
      break;
    case 'p':
      pflag = 1;
      break;
    case '?':
    default:
      if (optopt == 'a')
	fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint (optopt))
	fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
	fprintf (stderr,
		 "Unknown option character `\\x%x'.\n",
		 optopt);
      show_usage();
      return 1;
    }
  }
  
  if (pflag) {
    myPortCubeP = new SerialCube();
  }
	     
  if (eflag) {
    my8CubeP = new Solve8Cube();
    ret = pthread_create( &thread3, NULL, mainsolve8, NULL);
    if (ret != 0) {
      return 1;
    }
  }
  
  if (gflag && tflag == 0) {
    ret = pthread_create( &thread1, NULL, mainGL, NULL);
    if (ret != 0) {
      return 1;
    }
  }
  
  if (sflag) {
    mySolveCubeP = new SolveCube();
    if (avalue != NULL) {
      int spd = strtol(avalue, &avalue, 10);
      mySolveCubeP->setSpeed(spd);
    }
    if (tflag && gflag == 0) {
      myGLCubeP = new GLCube();
    }
    ret = pthread_create( &thread2, NULL, mainsolve, NULL);
    if (ret != 0) {
      return 1;
    }
  }
  
  if (aflag) {
    cout << avalue << endl;
    return 1;
  }

  if (pflag) {
    while (1) {
      usleep(100000); // sleep for 0.10 seconds
      myPortCubeP->cubeToSerial();
    }
  }
  
  if (gflag) pthread_join( thread1, NULL);
  if (sflag) pthread_join( thread2, NULL);
  if (eflag) pthread_join( thread3, NULL);

  return 0;

}

