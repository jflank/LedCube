/*
 * Author Joshua Flank
 * September 2017
 * 
 * Solves 8 queens problem. Code taken from stack-overflow.
 */

#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <vector>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <X11/Xlib.h>
#include <errno.h>
#include "Solve8.h"

void Solve8Cube::printSol()
{
  for(int i=0; i < size; ++i)
   {
      for(int j=0; j < size; ++j)
      {
         if(state[i] == j)
            cout << 'Q' << " ";
         else
            cout << '_' << " ";
      }

      cout << endl;
   }
  cout << endl;
}   

int Solve8Cube::shareBox()
{
  static int count = 0;
  int z;
  z = count % size;
  pthread_mutex_lock( &m_mutex);

  for(int x=0; x < size; ++x) {
      for(int y=0; y < size; ++y) {
	if(state[x] == y) {
	  rgb_t rgb;
	  rgb.word = 0;
	  rgb.color.red = 100;
	  rgb.color.green = 10 * z;
	  rgb.color.blue =   50 * z;
	  
	  m_cube[x][y][z] = rgb.word;
	} else {
	   m_cube[x][y][z] = 0;
	}
      }
   }

  pthread_mutex_unlock( &m_mutex);
  count ++;
  return 0;
}

int Solve8Cube::init()
{
  size = CUBESIZE;
  spaces = new bool*[CUBESIZE];
  for (int i = 0; i < CUBESIZE; ++i) {
    spaces[i] = new bool[CUBESIZE];
  }
  state.resize(CUBESIZE);
  return 0;
}

Solve8Cube::Solve8Cube() : LedCube()
{
  return;
}

bool Solve8Cube::isSafe(int row, int column)
{
  //check row conflict
  //no need to check for column conflicts
  //because the board is beimg filled column
  //by column
  for(int i = 0; i < column; ++i) {
    if(state[i] == row)
      return false;
  }
  
  //check for diagonal conflicts
  int columnDiff = 0;
  int rowDiff = 0;
  for(int i = 0; i < column; ++i) {
    columnDiff = abs(column - i);
    rowDiff = abs(row - state[i]);
    if(columnDiff == rowDiff) {
      return false;
    }
  }

  return true;
}

/*
int Solve8Q::getSize()
{
  return size;
}

bool Solve8Q::getSpace(int x, int y)
{
  return spaces[y][x];
}

void Solve8Q::setSpace(int x, int y, bool value)
{
  spaces[y][x] = value;
}
*/
/* do i need a copy constructor?
 
    Board(Board& other)
    {
        this->size = other.getSize();
        spaces = new bool*[size];
        for (int i = 0; i < size; ++i)
        {
            spaces[i] = new bool[size];
            for (int j = 0; j < size; ++j)
            {
                spaces[i][j] = other.getSpace(j, i);
            }
        }
    }
*/
bool Solve8Cube::backtrack(int& column)
{
  int row = 0;
  bool backtrack = column == size;
  
  while(column < size || backtrack) {
    if(backtrack) {
      if (column == 0) {
	return false;
      }
      column--;
      row = state[column] + 1;
      if(row == size) {
	backtrack = true;
	continue;
      }
      backtrack = false;
    }
    
    if(isSafe(row, column)) {
	state[column] = row;
	row = 0;
	column++; //advance
	backtrack = false;
	continue;
    } else {
      if(row == (size - 1)) {
	backtrack = true;
      } else {
	row++;
      }
    }
  }
  
  return true;  
}


void * mainsolve8(void * ptr)
{
    int board_size = CUBESIZE;
    int solution_count = 1000;

    cout << "Starting Solve 8 Queens 1000 times\n";
    my8CubeP->init();
    
    my8CubeP->printSol();  

    int column = 0;
    while (solution_count-- > 0 && my8CubeP->backtrack(column)) {
      //      if (m_speed > 0 && m_speed < 1000000) {
      //      usleep(1000000 - m_speed);
      //      }
      usleep(1000000);
      my8CubeP->printSol();
      my8CubeP->shareBox();
      my8CubeP->cubeToReceivers();
    }
    return 0;
}


