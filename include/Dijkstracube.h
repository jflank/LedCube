/*
 * Author Joshua Flank
 * October 2017
 * header file for Dijkstra's algorithm, viewed on a cube
 */
#ifndef _H_DJCUBE
#define _H_DJCUBE


#include "Ledcube.h"
#include <map>

class DJNode {
 public:
  int x;
  int y;
  int z;
  DJNode () {x = 0; y = 0; z = 0;}
  DJNode (const DJNode &obj) { this->x = obj.x; this->y = obj.y; this->z = obj.z;}
};



class DJCube : public LedCube {

 public:
  DJCube (uint32_t size = CUBESIZE);
  ~DJCube ();

  void     printEdges ();
  void     printDists ();
  uint64_t getNeighborEdge(const DJNode *inP, int i);
  DJNode * getNeighborCube(const DJNode *inP, int i);
  void     setOption  (int);
  
  int  main        ( int argc = 0, char **argv = NULL );

  
 private:
  void init(); // Not virtual from LedCube.
  int  solve();
  void distToCube();  
  
  uint32_t m_wsize;
  uint64_t getkey(uint32_t x1, uint32_t y1, uint32_t z1,
		  uint32_t x2, uint32_t y2, uint32_t z2);
  map<uint64_t, uint32_t> m_edges;                 // 3d edges between points
  vector< vector < vector <uint8_t>>>  m_visit;    // visitation
  vector< vector < vector <uint32_t>>> m_dist;     // distances

  uint32_t m_maxdist;
  uint32_t m_option;
  
protected:


};


#endif // _H_DJCUBE
