#include "LedcubeFactory.h"
#include "Ledcube.h"
#include "Serialcube.h"
#include "GLcube.h"
#include "Solve5x5.h"
#include "Solve8.h"
#include "AnimCube.h"

using namespace std;

LedCubeFactory & LedCubeFactory::Get()
{
  static LedCubeFactory instance;
  return instance;
}

void* LedCubeFactory::Thread(void * ptr)
{
  LedCube * cubeP = (LedCube *) ptr;
  cubeP->main(0, NULL);
}

LedCube* LedCubeFactory::Create(char type, int size)
{
  LedCube * cubeP;
  pthread_t thread;
  
  switch (type)
    {
    case 'p':
      cubeP = new SerialCube();
      break;
    case 'g':
      if (size == 0)
	size = 8;
      cubeP = new GLCube(size);
      break;
    case '8':
      cubeP = new Solve8Cube();
      break;
    case '5':
      cubeP = new Solve5();
      break;
    case 'z':
      cubeP = new AnimCube();
      break;
    }
  
  pthread_create( &thread, NULL, this->Thread, (void *)cubeP);
  
  m_threads.push_back(thread);
  m_cubes.push_back(cubeP);
  return cubeP;
}

/*
 * Clean up the cubes in the list. The list will be freed automagically.
 */

LedCubeFactory::~LedCubeFactory()
{
  for (auto it:m_threads) {
    pthread_join(it, NULL);
  }
  for (auto it:m_cubes) {
    free (it);
  }
}
