/*
 * Author Joshua Flank
 * September 2017
 * OpenGL view of 8x8x8 LED cube.
 */

#include "GLcube.h"

#include <GL/gl.h>
#include <GL/glut.h>
#include "GL/freeglut.h"
#include <unistd.h>

#define WINDOWX 640
#define WINDOWY 640

int a[3]={10,10,10}, b[3]={10,-10,10},
  c[3]={-10,-10,10}, d[3]={-10,10,10},
  e[3]={10,10,-10}, f[3]={10,-10,-10},
  g[3]={-10,-10,-10}, h[3]={-10,10,-10};

double transfat = 0.25;
GLCube * myGLCubeP = NULL;

float rot_z_vel = 0.0;
float rot_y_vel = 0.0;
//float rot_y_vel = 0.1;
float rot_x_vel = 0.0;

void GLCube::drawSpheres(void)
{
  int x, y, z;
  
  if (m_speed != GLMAXSPEED ) {
      usleep (100*(GLMAXSPEED - m_speed));
  }

  pthread_mutex_lock( &m_mutex);


  glMatrixMode(GL_PROJECTION);
  //glLoadIdentity();
  glPushMatrix();
  glTranslated(-1.0-1.0/CUBESIZE,-1.0-1.0/CUBESIZE, -CUBESIZE/2.0);
  //  glTranslated(0.0,- CUBESIZE, -CUBESIZE);
  for (z = 0; z < CUBESIZE; z++) {
    glTranslated(0,0, transfat);
    for (y = 0; y < CUBESIZE; y++) {
      glTranslated(0,transfat,0);
      for (x = 0; x < CUBESIZE; x++) {
	glTranslated(transfat, 0, 0);
	if (m_cube[x][y][z] == CUBEON) {
	  glColor3d(1,0,0);
	  glutSolidSphere(.04,15,15);
	} else if (m_cube[x][y][z] != CUBEOFF) {
	  rgb_t rgb;
	  rgb.word = m_cube[x][y][z];
	  glColor3b(rgb.color.red,rgb.color.green,rgb.color.blue);
	  glutSolidSphere(.04,15,15);	  
	} else {
	  glColor3d(.1,0,.3);
	  //	  glColor3d(0,0,1);
	  glutWireSphere(.03,7,7);
	}
      }
      glTranslated(-CUBESIZE * transfat ,0,0);
    }
    glTranslated(0, -CUBESIZE * transfat, 0);
  }
  glPopMatrix(); 
  pthread_mutex_unlock( &m_mutex);

}

int GLCube::setSpeed(int speed)
{
  if (speed > GLMAXSPEED || speed < GLMINSPEED) {
    return GLDEFSPEED;
  }
  m_speed = speed;
  return speed;
}

void RotateCube(void) {
    glMatrixMode(GL_PROJECTION);

    glTranslated(0,0, -CUBESIZE/3.0);
    glRotatef(rot_x_vel, 1.0, 0.0, 0.0);
    glRotatef(rot_y_vel, 0.0, 1.0, 0.0);
    glTranslated(0,0, CUBESIZE/3.0);
    glRotatef(rot_z_vel, 0.0, 0.0, 1.0);

} 

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);
  RotateCube();
  myGLCubeP->drawSpheres();
  glutSwapBuffers();

  
}

//from solidsphere.c
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

void keyboard(unsigned char key, int x, int y)
{
  switch (key)
    {
    case 'u':
      rot_z_vel += 0.1;
      break;
    case 'i':
      rot_z_vel -= 0.1;
      break;
    case 'j':
      rot_y_vel += 0.1;
      break;
    case 'k':
      rot_y_vel -= 0.1;
      break;
    case 'm':
      rot_x_vel += 0.1;
      break;  
    case ',':
      rot_x_vel -= 0.1;
      break;  
    case '-':
      myGLCubeP->setSpeed(myGLCubeP->getSpeed() + 1);
      break;  
    case '+':
      myGLCubeP->setSpeed(myGLCubeP->getSpeed() - 1);
      break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
        myGLCubeP->set(x*CUBESIZE/WINDOWX,7-y*CUBESIZE/WINDOWY,key-'1', CUBEON);
      break;
    case ' ':
      myGLCubeP->clear();
      break;
    case 'p':
      myGLCubeP->cubeToFile("GLCUBE.bin");
      break;
    case 's':
      myGLCubeP->cubeToSerial(); //serial not implemented yet
      break;
    case 0x1B:
    case 'q':
    case 'Q':
      exit(0);
      break;
    }
}

void mouse(int btn, int state, int x, int y)
{
  if (state == GLUT_DOWN)
    {
      if (btn == GLUT_LEFT_BUTTON)
	rot_y_vel -= 0.1;
      else if (btn == GLUT_RIGHT_BUTTON)
	rot_y_vel += 0.1;
    }
}


GLCube::GLCube() : LedCube(){
  m_speed = GLMAXSPEED;
  return;
}


const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f }; 

const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f }; 


void * mainGL(void * ptr)
{
  int argc = 1;
  char *argv[1] = {(char*)"Something"};

  myGLCubeP = new GLCube();

  myGLCubeP->set(0,0,0, CUBEON);
  myGLCubeP->set(1,0,0, CUBEON);
  myGLCubeP->set(1,7,0, CUBEON);
  myGLCubeP->set(0,0,7, CUBEON);

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

  /*
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  */
  //  glRotatef(30.0, 1.0, 0.0, 0.0);

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
