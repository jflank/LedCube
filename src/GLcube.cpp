/*
 * Author Joshua Flank
 * September 2017
 * OpenGL view of nxnxn LED cube.
 */

#include "GLcube.h"

#include <GL/gl.h>
#include <GL/glut.h>
#include "GL/freeglut.h"
#include <unistd.h>
#include "easylogging++.h"
#include <iomanip>
#include "math.h"

#define WINDOWX 640
#define WINDOWY 640

#define DEBUGLOG 0

void GLCube::drawSpheres(void)
{
  int x, y, z;
  
  unique_lock<mutex> lock(m_mutex);

  glPushMatrix();

  glTranslated(-1.0 - m_trans * 0.5, -1.0 - m_trans * 0.5, - m_trans * 2 - 4);
  
  for (z = 0; z < m_size; z++) {
    glTranslated(0, 0, m_trans);
    for (y = 0; y < m_size; y++) {
      glTranslated(0, m_trans, 0);
      for (x = 0; x < m_size; x++) {
	glTranslated(m_trans, 0, 0);

	if (m_cube[x][y][z] == CUBEON) {
	  glColor3d(1,0,0);
	  glutSolidSphere(m_sphereSize,15,15);
	} else if (m_cube[x][y][z] != CUBEOFF) {
	  rgb_t rgb;
	  rgb.word = m_cube[x][y][z];
	  glColor3b(rgb.color.red,rgb.color.green,rgb.color.blue);
	  glutSolidSphere(m_sphereSize,15,15);	  
	} else {
	  glColor3d(.1,0,.3);
	  //	  glColor3d(0,0,1);
	  glutWireSphere(.03,7,7);
	}
      }
      glTranslated( -1.0 * m_pixelSize * m_trans, 0, 0);
    }
    glTranslated(0, -1.0 * m_pixelSize * m_trans, 0);
  }
  glPopMatrix(); 

}

void GLCube::rotateCube(void) {

  glMatrixMode(GL_PROJECTION);

  glTranslated(0,0, - 3.5 + m_trans/2);
  glRotatef(m_rot_x, 1.0, 0.0, 0.0);
  glRotatef(m_rot_y, 0.0, 1.0, 0.0);
  glTranslated(0,0, 3.5 - m_trans/2);
  glRotatef(m_rot_z, 0.0, 0.0, 1.0);
  
} 

void GLCube::keyboard(unsigned char key, int x, int y)
{
  switch (key)
    {
    case 'u':
      m_rot_z += 0.1;
      break;
    case 'i':
      m_rot_z -= 0.1;
      break;
    case 'j':
      m_rot_y += 0.1;
      break;
    case 'k':
      m_rot_y -= 0.1;
      break;
    case 'm':
      m_rot_x += 0.1;
      break;  
    case ',':
      m_rot_x -= 0.1;
      break;  
    case '-':
      m_sphereSize -= 0.01;
      break;  
    case '=':
      m_sphereSize += 0.01;
      break;
    case ' ':
      myGLCubeP->clear();
      break;
    case 'p':
      myGLCubeP->cubeToFile("GLCUBE.bin");
      break;
    case 0x1B:
    case 'q':
    case 'Q':
      exit(0);
      break;
    }
}


/*
 * Turn on LED's by clicking on them. Ideally this should work in 3d, even 
 * when rotating the GLcube. The 3d part of this is a bit complicated, but 
 * at least this is a good start.
 */
void GLCube::clickLED(int mouseX, int mouseY, int bOn)
{
  double matModelView[16], matProjection[16]; 
  int viewport[4];
  double wx, wy, wz;
  double wx1,wy1,wz1;
  double wx2,wy2,wz2;

  LOG_IF(DEBUGLOG, DEBUG) << "Clicking on nearest LED to " << mouseX << "," << mouseY;

  // get matrix and viewport:
  glGetDoublev( GL_MODELVIEW_MATRIX, matModelView ); 
  glGetDoublev( GL_PROJECTION_MATRIX, matProjection ); 
  glGetIntegerv( GL_VIEWPORT, viewport ); 

  // window pos of mouse, Y is inverted on Windows
  double winX = (double)mouseX; 
  double winY = viewport[3] - (double)mouseY; 
  
  // get point on the 'near' plane (third param is set to 0.0)
  gluUnProject(winX, winY, 0.0, matModelView, matProjection, 
	       viewport, &wx1, &wy1, &wz1); 

  // get point on the 'far' plane (third param is set to 1.0)
  gluUnProject(winX, winY, 36.0, matModelView, matProjection, 
	       viewport, &wx2, &wy2, &wz2); 

  // now you can create a ray from m_start to m_end
  LOG_IF(DEBUGLOG, DEBUG) << std::fixed << std::setprecision(4) << "Ray from " << wx1 << "," << wy1 << "," <<wz1;


  int bestz = 0;
  int besty = 0;
  int bestx = 0;
  float bestdist = 10000; //pick a large number.
  float curdist;
  float dx =(0.40+0.40)/m_pixelDist; //(-4 to 4), -7 to 7
  float dy = dx; //dy and dx are the same right now.
  float dz = (.06+2)/m_pixelDist; // (from -2 to .06)

    
  //right now this is only calculated for default 640x640 window.
  wz = -2.0; // to .06

  for (int z = 0; z < m_size; z++) {
    wy = -0.4031 - z*0.30/7;
    for (int y = 0; y < m_size; y++) {
      wx = -0.4031 - z*0.30/7;
      for (int x = 0; x < m_size; x++) {
	float p = (wx-wx1)*(wx-wx1)+(wy-wy1)*(wy-wy1);
	  //	long double p = (dis12+dis2p+dis1p)/2;	
	if (p < bestdist) {
	  LOG_IF(DEBUGLOG, DEBUG) << std::fixed << std::setprecision(4) <<
	    "Ray from " << wx << "," << wy << ":" << bestdist << ":" <<x <<","<<y<<","<<z;
	  bestdist = p;
	  bestx = x;
	  besty = y;
	  bestz = z;
	}
	wx += dx;
      }
      wy += dy;
    }
    dx = dx + 0.6/(m_pixelDist * m_pixelDist);
    dy = dy + 0.6/(m_pixelDist * m_pixelDist);
  }
  myGLCubeP->set(bestx,besty,bestz, bOn);
}

/*
 * initialize GLCube - TODO: Right now there are two inits - Base init(size) 
 * this init(void) - I may want to combine them later to avoid confusion.
 */
void GLCube::init()
{
  m_pixelDist  = (double) m_size - 1;
  m_pixelSize  = (double) m_size; 
  m_trans      = 2/m_pixelSize;
  m_sphereSize = 0.04;  
  m_rot_x      = 0.0;
  m_rot_y      = 0.0;
  m_rot_z      = 0.0;
}

GLCube::GLCube() : LedCube()
{
  this->init();
}

GLCube::GLCube(uint32_t size) : LedCube(size)
{
  this->init();
}

/*
 * TODO: All of these functions/variables should be inside of GLCube. 
 */

const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f }; 

const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f }; 

static void resize(int width, int height)
{
  const float ar = (float) width / (float) height; 

  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0); 

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity() ;
} 

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);
  myGLCubeP->rotateCube();
  myGLCubeP->drawSpheres();

  glutSwapBuffers();
  
}

void keyboard(unsigned char key, int x, int y)
{
  myGLCubeP->keyboard(key, x, y);
}

void mouse(int btn, int state, int x, int y)
{
  if (state == GLUT_DOWN)
    {
      if (btn == GLUT_LEFT_BUTTON) {
	myGLCubeP->clickLED(x, y, CUBEON);
      }
      else if (btn == GLUT_RIGHT_BUTTON)
	myGLCubeP->clickLED(x, y, CUBEOFF);
    }
  myGLCubeP->cubeToReceivers();
}

void * mainGL(void * ptr)
{
  int argc = 1;
  char *argv[1] = {(char*)"Something"};

  glutInit(&argc, argv);
  glutInitWindowSize(WINDOWX, WINDOWY);
  glutInitWindowPosition(10,10);
  
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("GL Cube");

  glutMouseFunc(mouse);
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutReshapeFunc(resize);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, WINDOWX, WINDOWY, 0.0f, 0.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK); 

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); 

  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING); 

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position); 

  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess); 

  glutMainLoop();
  while (1) {
    
  }
  
  return(0);
}
