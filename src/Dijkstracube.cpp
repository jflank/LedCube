/* 
 * Copyright Joshua Flank (c) 2017 
 * 
 * Author Joshua Flank
 * October 2017
 * 
 * Dijkstra's algorithm on a cube.
 * see https://en.wikipedia.org/wiki/Dijkstra's_algorithm
 * 
 * 0 - give a random distance for each edge
 * 1 - set initial node as current
 * 2 - create a set of all unvisited nodes called unvisited.
 * 3 - calculate distance from current node to neighbors. Set neighbor distance to lowest between current and 
 */
#include "Dijkstracube.h"

#include <unistd.h>
#include "easylogging++.h"
#include <iomanip>
#include "math.h"
#include <ctime>

using namespace std;

DJCube::DJCube(uint32_t size) : LedCube(size)
{
  this->init();
}

/*
 * obtains a key that will represent the edge from point1 to point 2
 */
uint64_t DJCube::getkey(uint32_t x1, uint32_t y1, uint32_t z1,
			uint32_t x2, uint32_t y2, uint32_t z2)
{
  uint64_t multiple = m_size;
  uint64_t key = 0;

  key += x1 * multiple;
  multiple *= multiple;
  key +=  y1 * multiple;
  multiple *= multiple;
  key +=  z1 * multiple;
  multiple *= multiple;

  key +=  x2 * multiple;
  multiple *= multiple;
  key +=  y2 * multiple;
  multiple *= multiple;
  key +=  z2 * multiple;

  return key;
}

#define DJINFINITY ((uint32_t) -1)
#define DJMAXEDGE  50
void DJCube::printEdges ()
{
  cout << "Edges:\n";
  for (auto it: m_edges) {
    cout << it.first << " => " << it.second << "\t";
  }
  cout << endl;
}

void DJCube::printDists ()
{
  cout << "Distances from base:\n";

  for (int x = 0; x < m_size; x ++) {     
    for (int y = 0; y < m_size; y ++) {   
      for (int z = 0; z < m_size; z ++) { 
	if ((m_option == 2 || m_option == 20) && z != m_size-1) // 2d, only show the top of the cube
	  continue;
	cout << m_dist[x][y][z] << ":";
      }
      cout << "\t";
    }
    cout << endl;
  }
}


/*
 * TODO: create a real iterator for the 6 neighbors
 */

uint64_t DJCube::getNeighborEdge(const DJNode *inP, int i)
{
  uint32_t ret = 0;
  int x = inP->x;
  int y = inP->y;
  int z = inP->z;
  
  switch (i)
    {
    case 0:
      if (x+1 < m_size)
	ret = m_edges[getkey(x, y, z, x+1, y, z)];
      break;
    case 1:
      if (x-1 >= 0)
	ret = m_edges[getkey(x, y, z, x-1, y, z)];
      break;
    case 2:
      if (y+1 < m_size)
	ret = m_edges[getkey(x, y, z, x, y+1, z)];
      break;
    case 3:
      if (y-1 >= 0)
	ret = m_edges[getkey(x, y, z, x, y-1, z)];
      break;
    case 4:
      if ((m_option == 3 || m_option == 30) && z+1 < m_size)
	ret = m_edges[getkey(x, y, z, x, y, z+1)];
      break;
    case 5:
      if ((m_option == 3 || m_option == 30) && z-1 >= 0)
	ret = m_edges[getkey(x, y, z, x, y, z-1)];
      break;
    default:
      break;
    }
  return ret;
}

/*
 * TODO: Copy constructor doesn't accept a pointer
 */
DJNode * DJCube::getNeighborCube(const DJNode *inP, int i)
{
  DJNode * retP = NULL;
  switch (i)
    {
    case 0:
      if (inP->x+1 < m_size) {
	retP = new DJNode(); retP->x = inP->x; retP->y = inP->y; retP->z = inP->z;
	retP->x++;
      }
      break;
    case 1:
      if (inP->x-1 >= 0) {
	retP = new DJNode(); retP->x = inP->x; retP->y = inP->y; retP->z = inP->z;
	retP->x--;
      }
      break;
    case 2:
      if (inP->y+1 < m_size) {
	retP = new DJNode(); retP->x = inP->x; retP->y = inP->y; retP->z = inP->z;
	retP->y++;
      }
      break;
    case 3:
      if (inP->y-1 >= 0) {
	retP = new DJNode(); retP->x = inP->x; retP->y = inP->y; retP->z = inP->z;
	retP->y--;
      }
      break;
    case 4:
      if ((m_option == 3 || m_option == 30) && inP->z+1 < m_size) {
	retP = new DJNode(); retP->x = inP->x; retP->y = inP->y; retP->z = inP->z;
	retP->z++;
      }
      break;
    case 5:
      if ((m_option == 3 || m_option == 30) && inP->z-1 >= 0) {
	retP = new DJNode(); retP->x = inP->x; retP->y = inP->y; retP->z = inP->z;
	retP->z --;
      }
      break;
    default:
      break;
    }
  return retP;
}

void DJCube::init()
{
  m_wsize = m_size - 1;
  m_speed = 1000000;
  m_maxdist = DJMAXEDGE;
  m_option  = 2;
  m_visit.resize(m_size);
  m_dist.resize(m_size);

  // create the matrices. (Edges will be added in main)
  for (int x = 0; x < m_size; x ++) {
    m_visit[x].resize(m_size);
    m_dist[x].resize(m_size);
    for (int y = 0; y < m_size; y ++) {   
      m_visit[x][y].resize(m_size);
      m_dist[x][y].resize(m_size);
      for (int z = 0; z < m_size; z ++) { 
	m_visit[x][y][z] = 0; // set visited to false;
	m_dist[x][y][z] = DJINFINITY; 
      }
    }
  }
  m_dist[0][0][0] = 0;
}

/*
 * Run a BFS for the graph
 */

int DJCube::solve()
{
#define CURNODE m_dist[cur->x][cur->y][cur->z]
#define NEINODE m_dist[neighbor->x][neighbor->y][neighbor->z]
#define X cur->x
#define Y cur->y
#define Z cur->z
  
  int numIter = 0;
  bool done = false;
  DJNode * cur = new DJNode(); // c is current node

  list <DJNode *> priorityQ;

  if (m_option == 2 || m_option == 20)
    Z = m_size-1;
  
  while (!done) {
    numIter++;
    distToCube();
    cubeToReceivers();
    //    cout << "Cur  node: " << X << ", " << Y << ", " << Z << endl;
    if (numIter % 100 == 0)
      printDists();
    usleep(100000);
  
    for (int i = 0; i < 6; ++i) {
      DJNode* neighbor = getNeighborCube(cur, i);
      if (!neighbor)
	continue;
      uint32_t edge = getNeighborEdge(cur,i);
      uint32_t tentative = CURNODE + edge;
      if (tentative < NEINODE) {
	NEINODE = tentative;
      }
      list<DJNode*>::iterator it;
      for (it = priorityQ.begin(); it != priorityQ.end(); ++it) {
        DJNode * n = *it;
	if (m_dist[n->x][n->y][n->z] > NEINODE) {
	  break;
	}
      }
      priorityQ.insert(it, neighbor);
    }
    m_visit[X][Y][Z] = 1;
    if (CURNODE > m_maxdist)
      m_maxdist = CURNODE; // for viewing purposes
    
    if (X == m_size-1 && Y == m_size -1 && Z == m_size -1) {
      cout << "Done. Distance = " << CURNODE << endl;
      break;
    }
    
    do {
      delete cur;
      cur = priorityQ.front();
      priorityQ.pop_front();
      //      cout << "Next node: " << X << ", " << Y << ", " << Z << endl;
    } while (priorityQ.size() > 0 && m_visit[X][Y][Z] == 1);

    if (CURNODE == DJINFINITY || priorityQ.size() == 0) {
      cout << "Can't do it" << endl;
      break;
    }
  }
  // There still should be one more node. Either at infinity or the last node.
  delete cur;
  
  // clean up the rest of the priorityQ
  list<DJNode*>::iterator it;
  for (it = priorityQ.begin(); it != priorityQ.end(); ++it) {
    delete *it;
  }
  return 0;
}

/* generate colors from red to green
R = (255 * n) / 100
G = (255 * (100 - n)) / 100 
rgb_t rgb;
rgb.word = m_cube[x][y][z];
*/

void DJCube::distToCube()
{
  rgb_t rgb;
  unique_lock<mutex> lock(m_mutex); 
  
  for (int x = 0; x < m_size; x ++) {     
    for (int y = 0; y < m_size; y ++) {   
      for (int z = 0; z < m_size; z ++) { 
	if (m_dist[x][y][z] == DJINFINITY) {
	  continue;
	}
	rgb_t rgb = {0};
	rgb.color.red   = (225 * (m_dist[x][y][z])/ m_maxdist)+25;
	rgb.color.green = (225 * (m_maxdist-m_dist[x][y][z]+1)/ m_maxdist)+25;
	rgb.color.blue  = 0;
	m_cube[x][y][z] = rgb.word;
	cout << x << "," << y << "," << z << "," << hex << rgb.word << dec << endl;
	/*
	 * TODO: This ought to work, but it doesn't. Something is fishy with GL.
	 
	if (m_dist[x][y][z] < m_maxdist/2) {
	  rgb.color.red   = 255;
	  rgb.color.green = (255 * (m_dist[x][y][z]*2)/m_maxdist);
	  rgb.color.blue  = 0;
	} else {
	  rgb.color.green   = 255;
	  rgb.color.red    =  510 - (510 * m_dist[x][y][z]/m_maxdist);
	  rgb.color.blue  = 0;
	}
	*/
      }
    }
  }
}

void DJCube::setOption(int numOption)
{
  m_option = numOption;
  cout << "option = " << m_option << endl;
  if (m_option == 2 || m_option == 20) {
    m_dist[0][0][m_size-1] = 0;
  }
}

int DJCube::main(int argc, char **argv)
{
  srand(time(0));
  
  for (int x = 0; x < m_size; x ++) {
    for (int y = 0; y < m_size; y ++) {   
      for (int z = 0; z < m_size; z ++) { 
	if ((m_option == 2 || m_option == 20) && z != m_size-1) // 2d, only show the top of the cube 
	  continue;
	if (x+1 < m_size)
	  m_edges[getkey(x, y, z, x+1, y, z)] = rand() % DJMAXEDGE + 1;
	if (y+1 < m_size)
	  m_edges[getkey(x, y, z, x, y+1, z)] = rand() % DJMAXEDGE + 1;
	if (z+1 < m_size)
	  m_edges[getkey(x, y, z, x, y, z+1)] = rand() % DJMAXEDGE + 1;
	
	if (m_option == 30 || m_option == 20) { //edges are bidirectional
	  if (x-1 >= 0)
	    m_edges[getkey(x, y, z, x-1, y, z)] = rand() % DJMAXEDGE + 1;
	  if (y-1 >= 0)
	    m_edges[getkey(x, y, z, x, y-1, z)] = rand() % DJMAXEDGE + 1;
	  if (z-1 >= 0)
	    m_edges[getkey(x, y, z, x, y, z-1)] = rand() % DJMAXEDGE + 1;
	}
	
	// just for fun, on a 2d plane create m_size surmountable barrriers
	if (m_option == 2 && rand() % (m_size) == 0) {
	  if (x > 0){
	    m_edges.erase(getkey(x-1, y, z, x, y, z));
	    m_edges[getkey(x-1, y, z, x, y, z)] = DJMAXEDGE*100;
	  }
	  if (y>0) {
	    m_edges.erase(getkey(x, y-1, z, x, y, z));
	    m_edges[getkey(x, y-1, z, x, y, z)] = DJMAXEDGE*100;
	  }
	}
      }
    }
  }
  
  printEdges();
  solve();
  printDists();

  //  cout << "maxdist: " << m_maxdist << endl;
  //  printEdges();
}
