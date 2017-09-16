/*
 * Author Joshua Flank
 * September 2017
 * Just copy a cube to the serial port as binary. 
 * 
 * taken from com.c
 */

#include "Serialcube.h"

#include <unistd.h>
#include "easylogging++.h"

#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>

#define DEFAULT_SERIAL_DEVICE  "/dev/ttyUSB0"
#define BAUDRATE B9600

#define DEBUGLOG 0

void signal_handler_IO (int status);    //definition of signal handler

SerialCube::SerialCube() : LedCube()
{
  init();
  return;
}

void signal_handler_IO (int status)
{
  LOG(DEBUG) << "received invalid SIGIO signal: " << status ;
}

/*
***********************************************************
* init
***********************************************************
*/
void SerialCube::init()
{

  devicename = DEFAULT_SERIAL_DEVICE;

  BAUD     = B9600;
  DATABITS = CS8;
  STOPBITS = 0;
  PARITYON = 0;
  PARITY   = 0;

}

int SerialCube::cubeToSerial()
{
  uint8_t buffer[PACKETSIZE];
  int fd;
  struct sigaction saio;               //definition of signal action
  struct termios oldtio, newtio;       //place for old and new port settings for serial port

  //open the device(com port) to be non-blocking (read will return immediately)
  fd = open(devicename, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd < 0) {
    perror(devicename);
    return -1;
  }

  //install the serial handler before making the device asynchronous
  saio.sa_handler = signal_handler_IO;
  sigemptyset(&saio.sa_mask);   //saio.sa_mask = 0;
  saio.sa_flags = 0;
  saio.sa_restorer = NULL;
  sigaction(SIGIO,&saio,NULL);

    // allow the process to receive SIGIO
  fcntl(fd, F_SETOWN, getpid());
  // Make the file descriptor asynchronous (the manual page says only
  // O_APPEND and O_NONBLOCK, will work with F_SETFL...)
  fcntl(fd, F_SETFL, FASYNC);
  
  tcgetattr(fd,&oldtio); // save current port settings 
  // set new port settings for canonical input processing 
  newtio.c_cflag = BAUD | CRTSCTS | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
  newtio.c_lflag = 0;       //ICANON;
  newtio.c_cc[VMIN]=1;
  newtio.c_cc[VTIME]=0;
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&newtio);

  cubeToByte(buffer);

  if (DEBUGLOG == 1) {
    cout << "Writing to serial port:";
    coutByte(buffer, PACKETSIZE); 
  }
  
  for (int i = 0; i < PACKETSIZE; i ++) {
    //jhf double-check
    write(fd,&buffer[i],1);          //write 1 byte to the port
  }

   tcsetattr(fd,TCSANOW,&oldtio);

  close(fd);        //close the com port

}


void * mainSerial (void * ptr)
{
  while (1) {
    usleep(100000); // sleep for 0.10 seconds
    myPortCubeP->cubeToSerial();
  }
 
  return 0;
}


