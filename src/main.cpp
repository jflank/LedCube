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
#include <errno.h>
#include "LedcubeFactory.h"

#include "Ledcube.h"
#include "Serialcube.h"
#include "GLcube.h"
#include "Solve5x5.h"
#include "Solve8.h"
#include "AnimCube.h"
#include "Dijkstracube.h"

INITIALIZE_EASYLOGGINGPP

using namespace std;

static void show_usage()
{
  cerr << "Usage: " << endl;
  cout << "Options:\n"
       << "\t-h\tShow this help message\n"
       << "\t-g\tShow the cube in OpenGL <uijkm for rotation. -/= for size>.\n"
       << "\t-a #\tSet the solver speed (-a 1 is around 1 piece per second)\n"
       << "\t-s [5|8|d2|d3]\n"
       << "\t\t 5   - Show 5x5 solver\n"
       << "\t\t 8   - Show 8 queens solver\n"
       << "\t\t d2  - Dijskstra's in 2d - edges unidirectional\n"
       << "\t\t d3  - Dijskstra's in 3d - edges unidirectional\n"
       << "\t\t d30 - Dijskstra's in 3d - edges going in both directions\n"
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
       << "\t-S #\tSet the size of the Cube (supported for g right now.)\n" 
    
       << endl;
}

GLCube     * myGLCubeP    = NULL;

int main(int argc, char *argv[])
{

  SerialCube * myPortCubeP  = NULL;
  Solve5     * my5CubeP     = NULL;
  Solve8Cube * my8CubeP     = NULL;
  AnimCube   * myAnimCubeP  = NULL;
  DJCube     * myDJCubeP  = NULL;
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
  int size = 8;
  
  LedCubeFactory * cubesP = &LedCubeFactory::Get();
  
  opterr = 0;

  el::Configurations conf("./log.conf");
  el::Loggers::reconfigureAllLoggers(conf);

  LOG(INFO) << "Starting" ;
  while ((c = getopt (argc, argv, "egs:pa:z:S:")) != -1) {
    switch (c) {
    case 'g':
      gflag = 1;
      break;
    case 'S':
      size = atoi(optarg);
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

  //TODO: move all cube add receiver registration to the LedFactory
  if (pflag) {
    myPortCubeP = (SerialCube*) cubesP->Create('p');
  }

  if (gflag) {
    myGLCubeP = (GLCube*) cubesP->Create('g', size);
    pflag && myGLCubeP->cubeAddReceiver(myPortCubeP);
  }
  
  if (sflag && svalue[0] == '8') {
    my8CubeP = (Solve8Cube*) cubesP->Create('8');
    pflag && my8CubeP->cubeAddReceiver(myPortCubeP);
    gflag && my8CubeP->cubeAddReceiver(myGLCubeP);    
  }

  if (sflag && svalue[0] == 'd') {
    int action = strtol(&svalue[1], NULL, 10);
    if (action != 2 && action != 3 && action != 30) {
      action = 2;
    }
    myDJCubeP = (DJCube*) cubesP->Create('d', size);
    myDJCubeP->setOption(action);
    pflag && myDJCubeP->cubeAddReceiver(myPortCubeP);
    gflag && myDJCubeP->cubeAddReceiver(myGLCubeP);    
  }
  
  if (sflag && svalue[0] == '5') {
    my5CubeP = (Solve5*)cubesP->Create('5');
    pflag && my5CubeP->cubeAddReceiver(myPortCubeP);
    gflag && my5CubeP->cubeAddReceiver(myGLCubeP);

    if (avalue != NULL) {
      int spd = strtol(avalue, &avalue, 10);
      my5CubeP->setSpeed(spd);
    }
  }

  if (zvalue) {
    string str = "";
    int action = strtol(zvalue, NULL, 10);
    if (action == 9) {
      str = (char*) &zvalue[1]; // skip the 9, as in "9Hello" should just be "Hello"
    }
    myAnimCubeP = (AnimCube*) cubesP->Create('z');
    myAnimCubeP->setAnim(action);
    myAnimCubeP->setStr(str);
    
    pflag && myAnimCubeP->cubeAddReceiver(myPortCubeP);
    gflag && myAnimCubeP->cubeAddReceiver(myGLCubeP);

  }

  // Always run until program is killed. 
  while (1) {    
    usleep(1000000); // sleep 1 second
  }
  return 0;

}

