#ifndef _H_LEDCUBEFACTORY
#define _H_LEDCUBEFACTORY

#include <pthread.h>
#include "Ledcube.h"

class LedCubeFactory;
 /* 
  * Singleton factory to manage creation & deletion of cubes.
  * In addition to creating Ledcubes, this acts as a threadpool
  * for each other class.
  */

class LedCubeFactory
{
 private:
  LedCubeFactory() {};
  LedCubeFactory(const LedCubeFactory &) {}
  LedCubeFactory &operator=(const LedCubeFactory &) { return *this; }

  static void* Thread(void * ptr);

  
  list<LedCube *>   m_cubes;
  list<pthread_t> m_threads;
  
 public:
  ~LedCubeFactory();

  static LedCubeFactory &Get();
  
  LedCube* Create(char type, int size = 0);
  
};



#endif //_H_LEDCUBEFACTORY
