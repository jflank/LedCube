
/*
 * Author: Joshua Flank
 *
 * Taken from:
 * Nicholas K Schulze
 * www.HowNotToEngineer.com
 * 12/3/2011
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "AnimCube.h"
#include <unistd.h>
#include "easylogging++.h"

//constants
#define PI 3.14159265358979323846
//#define size 8
#define centre 3.5

//SETS THE ANIMATION MEMORY STRUCTURE TO ITS DEFAULT VALUES
void AnimCube::defaultMem(void){
  AMEM.error = 0;
  AMEM.phase = 0;
  AMEM.wipePos = 0;
  AMEM.wipeDir = 1;
  AMEM.firstPass = 1;
  AMEM.snakeStart = 1;
  AMEM.buttonPress = 0;
  AMEM.pressHandled = 0;
  AMEM.snakeDie = 0;
  AMEM.disappearDone = 1;
  AMEM.typeStringPos = 0;
  AMEM.letter = 0x00;
  AMEM.rotatePos = 0;
  AMEM.cubeGrowDir = 2;
  AMEM.cubeGrowSize = 7;
  AMEM.randDir = 1;
  AMEM.disappearAxis = XAXIS;
}

void AnimCube::setAnim(int anim)
{
  unique_lock<mutex> lock(m_mutex);
  m_anim = anim;
}

void AnimCube::setStr(string str)
{
  unique_lock<mutex> lock(m_mutex);
  m_str = str;
}

//THIS IS RUN IN THE ANIMATION THREAD
void AnimCube::animate(char ch) {
  //set the structure data that is used in other functions
  unique_lock<mutex> lock(m_mutex);
  
  if(!AMEM.error){
    switch(m_anim){
    case 0: //wipe X
      wipeFullPlane(XAXIS);
      break;
    case 1: //wipe Y
      wipeFullPlane(YAXIS);
      break;
    case 2: //wipe Z
      wipeFullPlane(ZAXIS);
      break;
    case 3: 
      sine(); 
      break;
    case 4:
      sideSine();
      break;
    case 5:
      cubePulse(); 
      break;
    case 6:
      rain();
      break;
    case 7:
      spiral();
      break;
    case 8:
      randomExpand();
      break;
    case 9:
      //      type(ch);
      AMEM.disappearAxis = ZAXIS;
      displayString();
      break;
    case 10:
      clockA();
      break;
    default:
      clear();
      clearDummy();
      break;
    }
  }
		
  //increment the slow counter 
  //generate and send the characters
  //hold the image
  AMEM.slowCount++;
}
//////////////
//ANIMATIONS//
//////////////

//USE wipeImage and loadPlane TO CREATE FULL PLANE WIPE ANIMATION IN DIRECTION OF wipeAxis
void AnimCube::wipeFullPlane(axes_t axis)
{
  if(AMEM.firstPass){
    //if this is the initial call load the full plane
    loadPlane(axis, 0);
    AMEM.wipePos = 0;
    AMEM.firstPass = 0;
  }
  if(AMEM.slowCount > 10 - m_speed){
    //animate
    wipeImage(axis);
    AMEM.slowCount = 0;
  }
}

//FILLS A PLANE AT planePos IN DIRECTION OF wipeAxis
void AnimCube::loadPlane(axes_t axis, int planePos)
{
  //fills and entire plane at the the chosen location along the chosen axis
  int x, y, z, px, py, pz;
  for(x = 0; x < CUBESIZE; x++){
    for(y = 0; y < CUBESIZE; y++){
      for(z = 0; z < CUBESIZE; z++){
	px = axis == XAXIS ? planePos:x;
	py = axis == YAXIS ? planePos:y;
	pz = axis == ZAXIS ? planePos:z;

	m_cube[px][py][pz] = CUBEON;
      }
    }
  }
}

//LOADS AN 8x8 ARRAY ONTO THE DUMMY CUBE
void AnimCube::loadArray(int *inArray){
  int i, j;
  for(i = 0; i < CUBESIZE; i++){
    for(j = 0; j < CUBESIZE; j++){
      dummyCube[CUBESIZE - 1 - j][CUBESIZE-1][CUBESIZE - 1 -i] = *inArray++;
    }
  }
}

/*
//LOADS BITMAP FILE TO A LAYER
//NOT FINISHED CURRENTLY UNDER DEVELOPMENT!!
void AnimCube::loadBMP(void){
  //  BITMAPFILEHEADER bmfh;
  //  BITMAPINFOHEADER bmih;
  BYTE *aBitmapBits, *pixelData;
  FILE *in;
  DWORD bSize; 
  LONG byteWidth, padWidth;
  LONG width, height;
  DWORD bpp, diff;
  int offset, j, i;
	
  in = fopen("image_small_black.bmp","rb");
  //  fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, in);
  //  fread(&bmih, sizeof(BITMAPINFOHEADER), 1, in);
	
  bSize = bmfh.bfSize - bmfh.bfOffBits;
  width = bmih.biWidth;
  height = bmih.biHeight;
  bpp = bmih.biBitCount;
	
  byteWidth = (LONG)((float)width*(float)bpp/8.0);
  padWidth = byteWidth;
	
  while(padWidth%4 != 0){
    padWidth++;
  }
	
  aBitmapBits = (BYTE*)malloc(bSize);
  fread(aBitmapBits, sizeof(BYTE), bSize, in);
	
  diff = height*byteWidth;
  pixelData = (BYTE*)malloc(diff);
	
  if(height > 0){
    j = bSize - 3;
    offset = padWidth - byteWidth;
    for(i = 0; i < size; i+=3){
      if((i+1)%padWidth == 0){
	i += offset;
      }
      *(pixelData + j + 2) = *(aBitmapBits + i);
      *(pixelData + j + 1) = *(aBitmapBits + i + 1);
      *(pixelData + j) = *(aBitmapBits + i + 2);
      j++;
    }
  }else{
    height = height *= -1;
    offset = 0;
    do{
      memcpy(pixelData+(offset*byteWidth), aBitmapBits+(offset*padWidth), byteWidth);
      offset++;
    }while(offset < height);
  }
	
  fclose(in);
  free(aBitmapBits);
  free(pixelData);
}
*/
//WIPES AN IMAGE BETWEEN 0 AND SIZE IN DIRECTION OF wipeAxis
void AnimCube::wipeImage(axes_t axis){
  int x, y, z;
  switch(axis){
  case XAXIS: //x axis
    for(y = 0; y < CUBESIZE; y++){
      for(z = 0; z < CUBESIZE; z++){
	//flip direction at ends
	if(AMEM.wipePos+AMEM.wipeDir >= CUBESIZE){
	  AMEM.wipeDir*=-1;
	}else if(AMEM.wipePos+AMEM.wipeDir < 0 ){
	  AMEM.wipeDir *= -1;
	}
	//shift image
	m_cube[AMEM.wipePos+AMEM.wipeDir][y][z] = m_cube[AMEM.wipePos][y][z];
	m_cube[AMEM.wipePos][y][z] = CUBEOFF;
      }
    }
    //increment next position
    AMEM.wipePos+=AMEM.wipeDir;
    break;
  case YAXIS: //y axis
    for(x = 0; x < CUBESIZE; x++){
      for(z = 0; z < CUBESIZE; z++){
	if(AMEM.wipePos+AMEM.wipeDir >= CUBESIZE){
	  AMEM.wipeDir*=-1;
	}else if(AMEM.wipePos+AMEM.wipeDir < 0 ){
	  AMEM.wipeDir *= -1;
	}
	m_cube[x][AMEM.wipePos+AMEM.wipeDir][z] = m_cube[x][AMEM.wipePos][z];
	m_cube[x][AMEM.wipePos][z] = CUBEOFF;
      }
    }
    AMEM.wipePos+=AMEM.wipeDir;
    break;
  case ZAXIS: //z axis
    for(x = 0; x < CUBESIZE; x++){
      for(y = 0; y < CUBESIZE; y++){
	if(AMEM.wipePos+AMEM.wipeDir >= CUBESIZE){
	  AMEM.wipeDir*=-1;
	}else if(AMEM.wipePos+AMEM.wipeDir < 0 ){
	  AMEM.wipeDir *= -1;
	}
	m_cube[x][y][AMEM.wipePos+AMEM.wipeDir] = m_cube[x][y][AMEM.wipePos];
	m_cube[x][y][AMEM.wipePos] = CUBEOFF;
      }
    }
    AMEM.wipePos+=AMEM.wipeDir;
    break;
  }
}

//disappear FUNCTION USED IN THE type FUNCTION
void AnimCube::disappear(void){
  int i, j, k;

  //shift cube contents along disappear axis
  for(i = 0; i < CUBESIZE-1; i++){
    for(j = 0; j < CUBESIZE; j++){
      for(k = 0; k < CUBESIZE; k++){
	// check to see if plane has moved the entire cube and set dissapearDone
	switch(AMEM.disappearAxis){
	case XAXIS:
	  if(m_cube[j][i][k] == CUBEON && i == 0){
	    AMEM.disappearDone = 1;
	  }
	  m_cube[j][i][k] = m_cube[j][i+1][k];
	  break;
	case YAXIS:
	  if(m_cube[i][j][k] == CUBEON && i == 0){
	    AMEM.disappearDone = 1;
	  }
	  m_cube[i][j][k] = m_cube[i+1][j][k];
	  break;
	case ZAXIS:
	  if(m_cube[j][k][i] == CUBEON && i == 0){
	    AMEM.disappearDone = 1;
	  }
	  m_cube[j][k][i] = m_cube[j][k][i+1];
	  break;
	}
      }
    }
  }
	
  //load new image from dummy to m_cube
  for(i = 0; i < CUBESIZE; i++){
    for(j = 0; j < CUBESIZE; j++){
      switch(AMEM.disappearAxis){
      case XAXIS:
	m_cube[i][7][j] = dummyCube[i][7][j] ? CUBEON : CUBEOFF;
	break;
      case YAXIS:
	m_cube[7][i][j] = dummyCube[i][7][j] ? CUBEON : CUBEOFF;
	break;
      case ZAXIS:
	m_cube[7-i][j][7] = dummyCube[i][7][j] ? CUBEON : CUBEOFF;
	break;
      }
			
    }
  }
  clearDummy();
}

//I was quite happy with these next two functions

//outerRotate and loop allow you to rotate an animated image about the outside fo the cube
//ORArray will wrap around the cube using outerRotate and be rotates using loop, any animations placed
//in ORArray will be animated and rotated.
//make sure you load the animation into the same position on the ORArray at each itteration, loop handles the rotation

//ROTATE data in ORArray AROUND OUTER PLANES
void AnimCube::outerRotate(void){
  int r, z;
	
  loop();

  //wrap the ORArray arrpound the outside of the cube
  for(r = 0; r < CUBESIZE*4-4; r++){
    for(z = 0; z < CUBESIZE; z++){
      if(r >= 0 && r < CUBESIZE){
	m_cube[CUBESIZE - 1 - r][7][z] = ORArray[r][z];
      }else if(r >= CUBESIZE && r < CUBESIZE*2-1){
	m_cube[0][CUBESIZE - (r-CUBESIZE) - 2][z] = ORArray[r][z];
      }else if(r >= CUBESIZE*2-1 && r < CUBESIZE*3-2){
	m_cube[r-(2*CUBESIZE-2)][0][z] = ORArray[r][z];
      }else if(r >= CUBESIZE*3-2 && r < CUBESIZE*4-3){
	m_cube[7][r-(3*CUBESIZE-3)][z] = ORArray[r][z];
      }
    }
  }
}

//Loop ORArray used for rotate function
void AnimCube::loop(void){
  int n, r, z;
  int temp[8];

  //shift and loop the ORArray until the correct position is reached
  for(n = 0; n < AMEM.rotatePos; n++){
    //shift first column into a temp array
    for(z = 0; z < CUBESIZE; z++){
      temp[z] = ORArray[0][z];
    }

    //shift all columns down
    for(r = 0; r < CUBESIZE*4-5; r++){
      for(z = 0; z < CUBESIZE; z++){
	ORArray[r][z] = ORArray[r+1][z];
      }
    }

    //move data from first column into last
    for(z = 0; z < CUBESIZE; z++){
      ORArray[CUBESIZE*4-5][z] = temp[z];
    }
  }

  //increment/reset rotatePos, this is what gives the rotation effect
  AMEM.rotatePos++;
  if(AMEM.rotatePos > 27){
    AMEM.rotatePos = 0;
  }
}

//SINE FUNCTION
void AnimCube::sine(void)
{
  int x, y;
  double Z;
  
  clear();
  
  //basic sine function
  for(x = 0; x < CUBESIZE; x++){
    for(y = 0; y < CUBESIZE; y++){
      Z = sin(AMEM.phase + sqrt(pow(map(x,0,CUBESIZE-1,-PI,PI),2) + pow(map(y,0,CUBESIZE-1,-PI,PI),2)));
      Z = map(Z,-1,0.95,0,CUBESIZE - 1); 	
      m_cube[x][y][(int)Z] = 1;
    }
  }
  
  //handle the speed and increment phase
  AMEM.phase += map(m_speed, 1, 10, PI/64, PI/18); 
}

//SINE PLANE
void AnimCube::sideSine(void){
  int x, y;
  double Z = 0;

  clear();
  for(x = 0; x < CUBESIZE; x++){
    for(y = 0; y < CUBESIZE; y++){
      Z = sin(AMEM.phase + sqrt(pow(map(x,-8,CUBESIZE-1,-PI,PI),2) + pow(map(y,-8,CUBESIZE-1,-PI,PI),2)));
      Z = map(Z,-0.9,0.9,0,7);
      m_cube[x][y][(int)Z] = 1;
    }
  }

  AMEM.phase += map(m_speed, 1, 10, PI/32, PI/12); 
}

//RANDOM RAIN EFFECT
void AnimCube::rain(void){
  int rX, rY, x, y, z;
  int count = 0;
  AMEM.disappearAxis = 2;

  if(AMEM.rainDrops > rand()%4+0){
    rX = rand()%8+0;
    rY = rand()%8+0;
    while(m_cube[rX][rY][7] != 0 && count < 100){
      rX = rand()%8+0;
      rY = rand()%8+0;
      count++;
    }
    m_cube[rX][rY][7] = 1;
    AMEM.rainDrops = 0;
  }

  if(AMEM.slowCount > 6){
    for(x = 0; x < CUBESIZE; x++){
      for(y = 0; y < CUBESIZE; y++){
	for(z = 0; z < CUBESIZE-1; z++){
	  m_cube[x][y][z] = m_cube[x][y][z+1];
	  if(m_cube[x][y][z] == 1){
	    m_cube[x][y][7] = 5;
	  }
	}
      }
    }
    AMEM.slowCount = 0;
  }

  AMEM.slowCount++;
  AMEM.rainDrops++;
}

//PULSING CUBE
void AnimCube::cubePulse(void){
  int x, y, z;

  if(AMEM.slowCount > (int)map(m_speed, 0, 10, 8, 2)){ 
    clear();

    //draw the 12 edges of a cube determained by cubeGrowSize
    for(x = 0; x < CUBESIZE; x++){
      for(y = 0; y < CUBESIZE; y++){
	for(z = 0; z < CUBESIZE; z++){
	  //corners
	  if(std::abs((int)(2*(x-centre))) == AMEM.cubeGrowSize &&
	     std::abs((int)(2*(y-centre))) == AMEM.cubeGrowSize &&
	     std::abs((int)(2*(z-centre))) == AMEM.cubeGrowSize){
	    m_cube[x][y][z] = 1;
	  }
	  //sides
	  if(std::abs((int)(2*(x-centre))) == AMEM.cubeGrowSize &&
	     std::abs((int)(2*(y-centre))) == AMEM.cubeGrowSize &&
	     std::abs((int)(2*(z-centre))) < AMEM.cubeGrowSize){
	    m_cube[x][y][z] = 1;
	  }
	  if(std::abs((int)(2*(x-centre))) == AMEM.cubeGrowSize &&
	     std::abs((int)(2*(z-centre))) == AMEM.cubeGrowSize &&
	     std::abs((int)(2*(y-centre))) < AMEM.cubeGrowSize){
	    m_cube[x][y][z] = 1;
	  }
	  if(std::abs((int)(2*(z-centre))) == AMEM.cubeGrowSize &&
	     std::abs((int)(2*(y-centre))) == AMEM.cubeGrowSize &&
	     std::abs((int)(2*(x-centre))) < AMEM.cubeGrowSize){
	    m_cube[x][y][z] = 1;
	  }
	}
      }
    }


    //increment and reset 
    AMEM.cubeGrowSize += AMEM.cubeGrowDir;

    if(AMEM.cubeGrowSize > 7){
      AMEM.cubeGrowSize = 5;
      AMEM.cubeGrowDir *= -1;
    }else if(AMEM.cubeGrowSize < 0){
      AMEM.cubeGrowSize = 3;
      AMEM.cubeGrowDir *= -1;
    }

    AMEM.slowCount = 0;
  }
}

//HELIX/SPIRAL
void AnimCube::spiral(void){
  int z, i;
  double X = 0;
  double Y = 0;
  clear();

  for(z = 0; z < CUBESIZE; z++){
    //the 3 itterations make the spiral thicker which looks better
    for(i = 0; i < 3; i++){
      Y = cos(AMEM.phase + i*PI/8 + map(z,0,CUBESIZE-1,0,2*PI));
      X = sin(AMEM.phase + i*PI/8 + map(z,0,CUBESIZE-1,0,2*PI));
      Y = map(Y,-1.1,0.9,0,CUBESIZE-1);
      X = map(X,-1.1,0.9,0,CUBESIZE-1);
      m_cube[(int)X][(int)Y][z] = 1;
    }
  }

  AMEM.phase += map(m_speed, 1, 10, PI/22, PI/8);
}

//FILL AND EMPTY CUBE RANDOM LEDS AT A TIME
void AnimCube::randomExpand(void){
  int count = 0;
  int rX, rY, rZ;

  srand(time(NULL));
  rX = rand()%8+0;
  rY = rand()%8+0;
  rZ = rand()%8+0;
	
  //find random off LED
  while(m_cube[rX][rY][rZ] == CUBEON && AMEM.randDir == 1){
    rX = rand()%8+0;
    rY = rand()%8+0;
    rZ = rand()%8+0;
    count++;
		
    //if its taking to long to find a off LED its probably all on, start turning off
    if(count > 10000){
      AMEM.randDir *= -1;
    }
  }
  count = 0;

  //find random on LED
  while(m_cube[rX][rY][rZ] == CUBEOFF && AMEM.randDir == -1){
    rX = rand()%8+0;
    rY = rand()%8+0;
    rZ = rand()%8+0;
    count++;

    if(count > 10000){
      AMEM.randDir *= -1;
    }
  }

  m_cube[rX][rY][rZ] = AMEM.randDir==1 ? CUBEON:CUBEOFF;
}

//////////////////////////
//INTERACTIVE ANIMATIONS//
//////////////////////////

//3D interactive snake game
//the snake puts an incrementally increaseing int in the array every move in the direction of travel, 
//this means the tail is always the lowest number in the cube and the head is always the largest number
void AnimCube::snake(void){
  int x, y, z;

  //first call or restart snake game
  if(AMEM.snakeStart){
    clear();
    clearDummy();
    AMEM.maxLength = 30;
    AMEM.headX = 0;
    AMEM.headY = 7;
    AMEM.headZ = 0;
    AMEM.tailX = AMEM.headX;
    AMEM.tailY = AMEM.headY;
    AMEM.tailZ = AMEM.headZ;
    AMEM.snakeLength = 1;
    AMEM.chopNum = 1;
    AMEM.snakeDir = 'u';
    AMEM.snakeAdd = 1;
    AMEM.changeDir = 0;
    AMEM.tailChop = 0;
    AMEM.snakeDie = 0;
    AMEM.snakeStart = 0;
  }

  if(AMEM.slowCount > 15 - m_speed || AMEM.buttonPress != 0){
    //if snake died on last restart snake game
    if(AMEM.snakeDie){
      AMEM.snakeStart = 1;
      AMEM.snakeAdd = 0;
    }
		
    //add head
    if(AMEM.snakeAdd){
      dummyCube[AMEM.headX][AMEM.headY][AMEM.headZ] = AMEM.snakeLength;
      AMEM.snakeAdd = 0;
    }
		
    //chop tail
    if(AMEM.snakeLength > AMEM.maxLength && AMEM.tailChop){
      dummyCube[AMEM.tailX][AMEM.tailY][AMEM.tailZ] = 0;

      if(dummyCube[AMEM.tailX-1][AMEM.tailY][AMEM.tailZ] == AMEM.chopNum && AMEM.tailX-1 != -1){
	AMEM.tailX--;
      }else if(dummyCube[AMEM.tailX+1][AMEM.tailY][AMEM.tailZ] == AMEM.chopNum && AMEM.tailX+1 != CUBESIZE){
	AMEM.tailX++;
      }else if(dummyCube[AMEM.tailX][AMEM.tailY-1][AMEM.tailZ] == AMEM.chopNum && AMEM.tailY-1 != -1){
	AMEM.tailY--;
      }else if(dummyCube[AMEM.tailX][AMEM.tailY+1][AMEM.tailZ] == AMEM.chopNum && AMEM.tailY+1 != CUBESIZE){
	AMEM.tailY++;
      }else if(dummyCube[AMEM.tailX][AMEM.tailY][AMEM.tailZ-1] == AMEM.chopNum && AMEM.tailZ-1 != -1){
	AMEM.tailZ--;
      }else if(dummyCube[AMEM.tailX][AMEM.tailY][AMEM.tailZ+1] == AMEM.chopNum && AMEM.tailZ+1 != CUBESIZE){
	AMEM.tailZ++;
      }
      AMEM.chopNum++;
      AMEM.tailChop = 0;
    }
		
    //change direction
    switch(AMEM.buttonPress){
    case 1: //up
      //ignore a 180deg turn button press
      AMEM.snakeDir = AMEM.snakeDir!='d'?'u':'d';
      AMEM.pressHandled = 1;
      break;
    case 2: //down
      AMEM.snakeDir = AMEM.snakeDir!='u'?'d':'u';
      AMEM.pressHandled = 1;
      break;
    case 3: //east
      AMEM.snakeDir = AMEM.snakeDir!='w'?'e':'w';
      AMEM.pressHandled = 1;
      break;
    case 4: //west
      AMEM.snakeDir = AMEM.snakeDir!='e'?'w':'e';
      AMEM.pressHandled = 1;
      break;
    case 5: //north
      AMEM.snakeDir = AMEM.snakeDir!='s'?'n':'s';
      AMEM.pressHandled = 1;
      break;
    case 6: //south
      AMEM.snakeDir = AMEM.snakeDir!='n'?'s':'n';
      AMEM.pressHandled = 1;
      break;
    }

    //move current direction
    switch(AMEM.snakeDir){
    case 'u':
      AMEM.headZ++;
      AMEM.snakeLength++;
      AMEM.snakeAdd = 1;
      AMEM.tailChop = 1;
      break;
    case 'd':
      AMEM.headZ--;
      AMEM.snakeLength++;
      AMEM.snakeAdd = 1;
      AMEM.tailChop = 1;
      break;
    case 'n':
      AMEM.headY--;
      AMEM.snakeLength++;
      AMEM.snakeAdd = 1;
      AMEM.tailChop = 1;
      break;
    case 's':
      AMEM.headY++;
      AMEM.snakeLength++;
      AMEM.snakeAdd = 1;
      AMEM.tailChop = 1;
      break;
    case 'e':
      AMEM.headX--;
      AMEM.snakeLength++;
      AMEM.snakeAdd = 1;
      AMEM.tailChop = 1;
      break;
    case 'w':
      AMEM.headX++;
      AMEM.snakeLength++;
      AMEM.snakeAdd = 1;
      AMEM.tailChop = 1;
      break;
    }
		
    if(AMEM.headX > CUBESIZE-1 || AMEM.headX < 0){
      AMEM.snakeDie = 1;
    }else if(AMEM.headY > CUBESIZE-1 || AMEM.headY < 0){
      AMEM.snakeDie = 1;
    }else if(AMEM.headZ > CUBESIZE-1 || AMEM.headZ < 0){
      AMEM.snakeDie = 1;
    }else if(dummyCube[AMEM.headX][AMEM.headY][AMEM.headZ] != 0){
      AMEM.snakeDie = 1;
    }
		
    AMEM.slowCount = 0;
  }
	
  if(AMEM.snakeAdd){
    clear();
    for(x = 0; x < CUBESIZE; x++){
      for(y = 0; y < CUBESIZE; y++){
	for(z = 0; z < CUBESIZE; z++){
	  if(dummyCube[x][y][z] != 0){
	    m_cube[x][y][z] = CUBEON;
	  }
	}
      }
    }
  }
}
#if 0
//VISUALISE MOUSE MOVEMENT
void AnimCube::mouse(void){
  POINT point;
  int I, J;
  int captureWidth = 1919;
  int captureHeight = 1079;
	
  GetCursorPos(&point);
	
  I = (int)point.x;
  J = (int)point.y;
	
  clearDummy();
	
  if(I >= 0 && I <= captureWidth  && J >= 0 && J <= captureHeight){
    I = map(I, 0, captureWidth , 0, CUBESIZE-1);
    J = map(J, 0, captureHeight , 0, CUBESIZE-1);
    switch(AMEM.disappearAxis){
    case 0:
      dummyCube[CUBESIZE - 1 - I][7][CUBESIZE - 1 - J] = 1;
      m_cube[CUBESIZE - 1 - I][7][CUBESIZE - 1 - J] = 1;
      break;
    case 1:
      dummyCube[7][CUBESIZE - 1 - I][CUBESIZE - 1 - J] = 1;
      m_cube[7][CUBESIZE - 1 - I][CUBESIZE - 1 - J] = 1;
      break;
    case 2:
      dummyCube[CUBESIZE - 1 - I][J][7] = 1;
      m_cube[CUBESIZE - 1 - I][J][7] = 1;
      break;
    }
  }
	
  if(AMEM.slowCount > 2){
    disappear();
    AMEM.slowCount = 0;
  }
}	
#endif

void AnimCube::displayString()
{
 
  usleep(100000); // sleep 0.1 second
  if (AMEM.disappearDone) {
    char in = toupper(m_str[strpos]);
    cout << "Current Char: " << in << endl;
    loadArray(IMG(in));
    AMEM.disappearDone = 0;
    AMEM.slowCount = 0;
    strpos ++;
    if (strpos >= m_str.size())
      strpos = 0;
  }
    
  if(AMEM.slowCount > map(m_speed, 0, 10, 5, 1)){
    disappear();
    AMEM.slowCount = 0;
  }
}

//CAPTURE AND DISPLAY TEXT FROM KEYBOARD
void AnimCube::type(char in)
{
  //convert to upper case
  if(isalpha(in)){
    //    in -= 0x20;
    in = toupper(in);
  }
  
  //  if(AMEM.disappearDone && AMEM.typeStringPos == 0) {
  if(AMEM.disappearDone) {
    loadArray(IMG(in));
    AMEM.slowCount = 0;
    AMEM.disappearDone = 0;
    AMEM.pressHandled = 1;
  }
  /*jhf  this buffers up the entire string one char at a time. not needed here...
  else {
    AMEM.typeString[AMEM.typeStringPos] = in;
    AMEM.typeStringPos++;
    AMEM.disapperDone = 0;
    AMEM.pressHandled = 1;
  }

  if(AMEM.disappearDone && AMEM.typeStringPos > 0){
    loadArray(IMG(pull()));
    AMEM.disappearDone = 0;
    AMEM.pressHandled = 1;
  }
  */
  if(AMEM.slowCount > map(m_speed, 0, 10, 5, 1)){
    disappear();
    AMEM.slowCount = 0;
  }
}

//ROTATING DIGITAL CLOCK
void AnimCube::clockA(void){
  int z, r, i, x, y;
  int offset = 0;
  int skip = 0;
  int *tempArP;
  int tempAr[8][8];
	
  //get the sytem time and date
  time_t rawtime = time(NULL);
  char *timeDate = ctime(&rawtime);
  char time[] = "xx:xx:xx";
  char hour[] = "xx";
  int hourD;
	
  //extract time and convert it to 12 hour time
  memcpy(time, timeDate+11, 8);
  memcpy(hour, time, 2);
  hourD = atoi(hour);

  if(hourD > 12){
    hourD -= 12;
    string s = std::to_string(hourD); //TODO jhf doublecheck this later
    hour[0] = s[0];
    hour[1] = s[1];
    if(hourD >= 10){
      memcpy(time, hour, 2);
    }else{
      memcpy(time+1, hour, 1);
      memset(time, '0', 1);
    }
  }

  //load time into ORArray
  if(AMEM.slowCount > 10 - m_speed){
    memset(ORArray, 0, sizeof(ORArray));

    for(i = 0; i < 5; i++){
      if(!(i == 0 && time[i] == '0')){
	tempArP = IMG(time[i]);
	for(x = 0; x < CUBESIZE; x++){
	  for(y = 0; y < CUBESIZE; y++){
	    tempAr[y][CUBESIZE - 1 - x] = *tempArP++;
	  }
	}

	if(time[i] == '1' || time[i] == ':'){
	  skip = 4;
	}else{
	  skip = 2;
	}

	for(z = 0; z < CUBESIZE; z++){
	  for(r = 0; r < CUBESIZE-skip; r++){
	    ORArray[r+offset][z] = tempAr[r+skip][z];
	  }
	}

	offset += CUBESIZE - skip - 2;
      }
    }

    //loop ORArray and load onto m_cube
    outerRotate();
    AMEM.slowCount = 0;
  }
}

//////////////////
//HELPER METHODS//
//////////////////

//pull a value from the start of typeString and shift all other values down
char AnimCube::pull(void){
  int i;
  char ret = AMEM.typeString[0];

  for(i = 0; i < sizeof(AMEM.typeString) - 1; i++){
    AMEM.typeString[i] = AMEM.typeString[i+1];
  }

  AMEM.typeString[sizeof(AMEM.typeString) - 1] = 0x00;

  AMEM.typeStringPos--;
  return ret;
}

//LETS WIN APP KNOW WHEN ANIMATION HAS FINISHED HANDELING A BUTTON PRESS
int AnimCube::buttonHandled(void){
  return AMEM.pressHandled;
}

//MAP RANGE OF VALUES
double AnimCube::map(double in, double inMin, double inMax,
		     double outMin, double outMax)
{
  double out;
  out = (in-inMin)/(inMax-inMin)*(outMax-outMin) + outMin;
  return out;
}

//CLEAR dummyCube
void AnimCube::clearDummy(void){
  memset(dummyCube, 0, sizeof(dummyCube));
}

AnimCube::AnimCube() : LedCube(CUBESIZE){
  m_speed = 1;
  m_anim = 0;
  m_str = "";
  defaultMem();
}

int AnimCube::main(int argc, char **argv)
{
  int i = 0;
  int j = 0;
  int ich = 0;
  char ch = m_str[ich];

  while (1) {    
    usleep(50000); // sleep 0.1 second
      /*
    if (i >20) {
      i = 0;

      if (m_anim == 9) {
	if (j > 2) {
	  if (m_str[ich] != '\0') {
	    ch = m_str[ich];
	    cout << "Current Char: " << ch << endl;
	    ich ++;
	  }
	  j = 0;
	}
	j++;
      }
    }
    i++;
*/
    //    drawCube();  
    animate(ch);
    cubeToReceivers();
  }
}
