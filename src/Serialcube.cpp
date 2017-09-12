/*
 * Author Joshua Flank
 * September 2017
 * Just copy a cube to the serial port as binary. It's a pretty small class, 
 * but if serial port setup is needed, it can be done here.
 */

#include "Serialcube.h"

#include <unistd.h>
#include "easylogging++.h"


int SerialCube::cubeToSerial()
{
  return cubeToFile("/dev/ttyUSB1");
}

SerialCube::SerialCube() : LedCube()
{
  return;
}


