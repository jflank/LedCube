#ifndef _H_ANIMCUBE
#define _H_ANIMCUBE

#include "Ledcube.h"
extern int *IMG(char in);

//set this to make sure that an availiable animation is selected through the window 
#define numOfAnimations 9

//structure to handle all animation inputs and memory
//Im not sure if this is how structures are ment to be used, but I
//just learned about them so wanted to use one
typedef struct{
	//used everywhere
	int startX, startY, startZ;
	int animation;
	int slowCount;
	int buttonPress;
	int pressHandled;
	int error;
	
	//wipe animations
	int firstPass;
	int wipePos, wipeDir, wipeAxis;
	int planePos;
	
	//sine animations
	double phase;
	
	//snake animation
	int maxLength;
	int tailX, tailY, tailZ;
	int headX, headY, headZ;
	int snakeLength;
	int chopNum;
	char snakeDir;
	int snakeStart, snakeAdd, tailChop, changeDir, snakeDie;

	//mouse animation
	int disappearAxis;

	//type animation
	char letter;
	char typeString[512];
	char typeStringPos;
	int disappearDone;

	//rotate animation
	int rotatePos;

	//pulsing cube animation
	int cubeGrowSize;
	int cubeGrowDir;

	//rain animation
	int rainDrops;

	//random
	int randDir;
}animationData;

typedef enum {
  XAXIS,
  YAXIS,
  ZAXIS
} axes_t;

class AnimCube : public LedCube {
 public:
  AnimCube();
  ~AnimCube();
  
  void defaultMem       (void);
  void animate          (char );
  void wipeFullPlane    (axes_t);
  void loadPlane        (axes_t, int);
  void loadArray        (int *array);
  void dummyToCube      (void);
  void loadBMP          (void);
  void wipeImage        (axes_t);
  void disappear        (void);
  void outerRotate      (void);
  void loop             (void);
  void sine             (void);
  void sideSine         (void);
  void rain             (void);
  void cubePulse        (void);
  void spiral           (void);
  void randomExpand     (void);
  void snake            (void);
  void mouse            (void);
  void type             (char in);
  void clockA           (void);
  char pull             (void);
  int buttonHandled     (void);
  double map            (double in, double inMin, double inMax,
			 double outMin, double outMax);
  void clearDummy       (void);

  void setStr           (string);
  void setAnim          (int);
  int  main             (int argc, char **argv);
  void displayString    ();
  
 private:
  int    m_anim;
  string m_str;
  int    strpos;
  
 protected:

  int dummyCube[8][8][8];
  int ORArray[28][8];
  char cubeChars[64];
  animationData AMEM;

  
};

#endif //_H_ANIMCUBE
