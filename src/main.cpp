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
#include "AnimCube.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;

static void show_usage()
{
  cerr << "Usage: " << endl;
  cout << "Options:\n"
       << "\t-h\tShow this help message\n"
       << "\t-g\tShow the cube in OpenGL <uijkm, control the rotation>.\n"
       << "\t-a #\tSet the solver speed (-a 1 is around 1 piece per second)\n"
       << "\t-s [5|8]\tShow 5x5 solver or the 8 queens solver\n"
       << "\t-p\tSend data over the serial port\n"
       << "\t-z [0-10]\tRun different animations\n"
       << "\t\t 0-2 - planes moving up/down left/right.\n"
       << "\t\t 3-4 - sine wave up/down left/right.\n"
       << "\t\t 5   - pulse.\n"
       << "\t\t 6   - rain.\n"
       << "\t\t 7   - spiral.\n"
       << "\t\t 8   - random Expand/contract.\n"
       << "\t\t 9   - display characters - ex: '-z 9HELLOWORLD'.\n"
       << "\t\t 10  - Current time, moving around the cube.\n"
    
    
       << endl;
}

SerialCube * myPortCubeP  = NULL;
Solve5     * my5CubeP     = NULL;
GLCube     * myGLCubeP    = NULL;
Solve8Cube * my8CubeP     = NULL;
AnimCube   * myAnimCubeP  = NULL;

int main(int argc, char *argv[])
{

  pthread_t thread1, thread2, thread3, thread4, thread5;
  int ret;
  int gflag = 0;
  int aflag = 0;
  int eflag = 0;
  int sflag = 0;
  int pflag = 0;
  int zflag = 0;
  char *avalue = NULL;
  char *zvalue = NULL;
  char *svalue = NULL;
  int c;

  opterr = 0;

  el::Configurations conf("./log.conf");
  el::Loggers::reconfigureAllLoggers(conf);

  LOG(INFO) << "Starting" ;
  while ((c = getopt (argc, argv, "egs:pa:z:")) != -1) {
    switch (c) {
    case 'g':
      gflag = 1;
      break;
    case 's':
      sflag = 1;
      svalue = optarg;
      break;
    case 'e':
      eflag = 1;
      break;
    case 'a':
      avalue = optarg;
      break;
    case 'z':
      zflag = 1;
      zvalue = optarg;
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
    ret = pthread_create( &thread3, NULL, mainSerial, NULL);
    if (ret != 0) {
      return 1;
    }
  }

  if (gflag) {
    myGLCubeP = new GLCube();
    pflag && myGLCubeP->cubeAddReceiver(myPortCubeP);

    ret = pthread_create( &thread1, NULL, mainGL, NULL);
    if (ret != 0) {
      return 1;
    }
  }
  
  if (sflag && svalue[0] == '8') {
    my8CubeP = new Solve8Cube();
    pflag && my8CubeP->cubeAddReceiver(myPortCubeP);
    gflag && my8CubeP->cubeAddReceiver(myGLCubeP);
    
    ret = pthread_create( &thread3, NULL, mainsolve8, NULL);
    if (ret != 0) {
      return 1;
    }
  }
  
  if (sflag && svalue[0] == '5') {
    my5CubeP = new Solve5();
    pflag && my5CubeP->cubeAddReceiver(myPortCubeP);
    gflag && my5CubeP->cubeAddReceiver(myGLCubeP);

    if (avalue != NULL) {
      int spd = strtol(avalue, &avalue, 10);
      my5CubeP->setSpeed(spd);
    }
    ret = pthread_create( &thread2, NULL, mainsolve5, NULL);
    if (ret != 0) {
      return 1;
    }
  }

  if (zvalue) {
    myAnimCubeP = new AnimCube();
    pflag && myAnimCubeP->cubeAddReceiver(myPortCubeP);
    gflag && myAnimCubeP->cubeAddReceiver(myGLCubeP);

    ret = pthread_create( &thread1, NULL, mainAnim, zvalue);
    if (ret != 0) {
      return 1;
    }
  }

  if (gflag) pthread_join( thread1, NULL);
  if (sflag) pthread_join( thread2, NULL);
  if (eflag) pthread_join( thread3, NULL);
  if (pflag) pthread_join( thread4, NULL);
  if (zflag) pthread_join( thread5, NULL);

  return 0;

}

