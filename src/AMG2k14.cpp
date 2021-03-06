
//----------------------------------------------------------------------
//	Addapted from: https://www.cs.umd.edu/class/spring2013/cmsc425/oglsample.shtml
//
//----------------------------------------------------------------------

#include <cstdlib> // standard definitions
#include <cmath> // math definitions
#include <cstdio> // standard I/O
#include "Maze.h"

// include files are in a slightly different location for MacOS
#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

// escape key (for exit)
#define ESC 27

//----------------------------------------------------------------------
// Global variables
//
// The coordinate system is set up so that the (x,y)-coordinate plane
// is the ground, and the z-axis is directed upwards. The y-axis points
// to the north and the x-axis points to the east.
//
// The values (x,y) are the current camera position. The values (lx, ly)
// point in the direction the camera is looking. The variables angle and
// deltaAngle control the camera's angle. The variable deltaMove
// indicates the amount of incremental motion for the camera with each
// redraw cycle. The variables isDragging and xDragStart are used to
// monitor the mouse when it drags (with the left button down).
//----------------------------------------------------------------------

// Camera position
float x = 0.0, y = -5.0; // initially 5 units south of origin
float deltaMove = 0.0; // initially camera doesn't move

// Camera direction
float lx = 0.0, ly = 1.0; // camera points initially along y-axis
float angle = 0.0; // angle of rotation for the camera direction
float deltaAngle = 0.0; // additional angle change when dragging

// Mouse drag control
int isDragging = 0; // true when dragging
int xDragStart = 0; // records the x-coordinate when dragging starts

//Maze
int _x=20;
int _y=20;
Smart_Maze maze(_x,_y);

//---------------------------------------------------------------------
//	Initializes the maze
//
//	Adds walls, and will initialize lighting in the future
//---------------------------------------------------------------------
void init(){
	_x=maze.get_x();
	_y=maze.get_y();
	maze.generate();
	x=4;
	y=4;


  	 glClearColor (0.0, 0.0, 0.0, 0.0);

   //Disable global ambient lighting
   	GLfloat black[] = {0, 0, 0, 1};
   	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);


	GLfloat ambient[] = { 0.7, 0.7, 0.7, 1.0 };
	GLfloat diffuse[] = { 0.2,0.2,0.2, 1.0 };
	GLfloat specular[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat shininess = 65.0;

	GLfloat position[] = { 0.0, 0, 0.0, 0.5 };

	glEnable(GL_COLOR_MATERIAL);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
	glLightfv(GL_LIGHT0,GL_AMBIENT,specular);
	glLightfv(GL_LIGHT0,GL_POSITION,position);


   //glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   //glEnable(GL_LIGHT1);
   glEnable(GL_DEPTH_TEST);
   glShadeModel (GL_SMOOTH);
 //  glEnable( GL_TEXTURE_2D );
 //  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   //glBindTexture(GL_TEXTURE_2D,texture[ntex]);
}




//----------------------------------------------------------------------
// Reshape callback
//
// Window size has been set/changed to w by h pixels. Set the camera
// perspective to 45 degree vertical field of view, a window aspect
// ratio of w/h, a near clipping plane at depth 1, and a far clipping
// plane at depth 100. The viewport is the entire window.
//
//----------------------------------------------------------------------
void changeSize(int w, int h) 
{
	float ratio =  ((float) w) / ((float) h); // window aspect ratio
	glMatrixMode(GL_PROJECTION); // projection matrix is active
	glLoadIdentity(); // reset the projection
	gluPerspective(45.0, ratio, 0.1, 100.0); // perspective transformation
	glMatrixMode(GL_MODELVIEW); // return to modelview mode
	glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

//----------------------------------------------------------------------
// Draw one snowmen (at the origin)
//
// A snowman consists of a large body sphere and a smaller head sphere.
// The head sphere has two black eyes and an orange conical nose. To
// better create the impression they are sitting on the ground, we draw
// a fake shadow, consisting of a dark circle under each.
//
// We make extensive use of nested transformations. Everything is drawn
// relative to the origin. The snowman's eyes and nose are positioned
// relative to a head sphere centered at the origin. Then the head is
// translated into its final position. The body is drawn and translated
// into its final position.
//----------------------------------------------------------------------
void drawMaze()
{
	// Draw body (a 20x20 spherical mesh of radius 0.75 at height 0.75)
	float off_white=0.85;
	glColor3f(off_white, off_white, off_white); // set drawing color to white
	glPushMatrix();
		maze.draw_walls();
	glPopMatrix();

}

//----------------------------------------------------------------------
// Update with each idle event
//
// This incrementally moves the camera and requests that the scene be
// redrawn.
//----------------------------------------------------------------------
void update(void) 
{
	int try_x = (x + deltaMove * lx * 0.1)/3;
	int try_y = (y + deltaMove * ly * 0.1)/3;
	if(DEBUG) std::cout<<(int)try_x<<", "<<(int)try_y<<std::endl;
	if (deltaMove && maze.valid_move(try_x,try_y)) { // update camera position
		x += deltaMove * lx * 0.1;
		y += deltaMove * ly * 0.1;
	}
	if(y>_y*3){
		maze.generate();
		init();}
	glutPostRedisplay(); // redisplay everything
}

//----------------------------------------------------------------------
// Draw the entire scene
//
// We first update the camera location based on its distance from the
// origin and its direction.
//----------------------------------------------------------------------
void renderScene(void) 
{
	int i, j;

	// Clear color and depth buffers
	glClearColor(0.15, 0.33, 0.55, 1.0); // sky color is light blue
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	// Set the camera centered at (x,y,1) and looking along directional
	// vector (lx, ly, 0), with the z-axis pointing up
	gluLookAt(
			x,      y,      1.0,
			x + lx, y + ly, 1.0,
			0.0,    0.0,    1.0);

	// Draw ground - 200x200 square colored grey

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.3, 0.3, 0.3);
	glBegin(GL_QUADS);
		glVertex3f(-100.0, -100.0, 0.0);
		glVertex3f(-100.0,  _y*3, 0.0);
		glVertex3f( _x*3,  _y*3, 0.0);
		glVertex3f( _x*3, -100.0, 0.0);
	glEnd();
	glEnable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);

	// Draw maze
	drawMaze();
	glutSwapBuffers(); // Make it all visible
} 

//----------------------------------------------------------------------
// User-input callbacks
//
// processNormalKeys: ESC, q, and Q cause program to exit
// pressSpecialKey: Up arrow = forward motion, down arrow = backwards
// releaseSpecialKey: Set incremental motion to zero
//----------------------------------------------------------------------
void processNormalKeys(unsigned char key, int xx, int yy)
{
	if (key == ESC || key == 'q' || key == 'Q') exit(0);
	if (key == 'w' || key=='W') deltaMove = 3.0;
	if (key == 's' || key=='S') deltaMove = -3.0;
} 
void releaseNormalKeys(unsigned char key, int xx, int yy)
{
	if (key == ESC || key == 'q' || key == 'Q') exit(0);
	if (key == 'w' || key=='W') deltaMove = 0;
	if (key == 's' || key=='S') deltaMove = 0;
} 

void pressSpecialKey(int key, int xx, int yy)
{
	switch (key) {
		case GLUT_KEY_UP : deltaMove = 3.0; break;
		case GLUT_KEY_DOWN : deltaMove = -3.0; break;
	}
} 

void releaseSpecialKey(int key, int x, int y) 
{
	switch (key) {
		case GLUT_KEY_UP : deltaMove = 0.0; break;
		case GLUT_KEY_DOWN : deltaMove = 0.0; break;
	}
} 

//----------------------------------------------------------------------
// Process mouse drag events
// 
// This is called when dragging motion occurs. The variable
// angle stores the camera angle at the instance when dragging
// started, and deltaAngle is a additional angle based on the
// mouse movement since dragging started.
//----------------------------------------------------------------------
void mouseMove(int x, int y) 
{ 	
	if (isDragging) { // only when dragging
		// update the change in angle
		deltaAngle = (x - xDragStart) * 0.005;

		// camera's direction is set to angle + deltaAngle
		lx = -sin(angle + deltaAngle);
		ly = cos(angle + deltaAngle);
	}
}

void mouseButton(int button, int state, int x, int y) 
{
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) { // left mouse button pressed
			isDragging = 1; // start dragging
			xDragStart = x; // save x where button first pressed
		}
		else  { /* (state = GLUT_UP) */
			angle += deltaAngle; // update camera turning angle
			isDragging = 0; // no longer dragging
		}
	}
}

//----------------------------------------------------------------------
// Main program  - standard GLUT initializations and callbacks
//----------------------------------------------------------------------
int main(int argc, char **argv) 
{
	printf("\n\
-----------------------------------------------------------------------\n\
  The Amazing Maze Game 2k14:\n\
  - Drag mouse left-right to rotate camera\n\
  - Hold W/S to move forward/backward\n\
  - q or ESC to quit\n\
-----------------------------------------------------------------------\n");

	// general initializations
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 400);
	glutCreateWindow("OpenGL/GLUT Sampe Program");

	// register callbacks
	glutReshapeFunc(changeSize); // window reshape callback
	glutDisplayFunc(renderScene); // (re)display callback
	glutIdleFunc(update); // incremental update 
	glutIgnoreKeyRepeat(1); // ignore key repeat when holding key down
	glutMouseFunc(mouseButton); // process mouse button push/release
	glutMotionFunc(mouseMove); // process mouse dragging motion
	glutKeyboardFunc(processNormalKeys); // process standard key clicks
	glutKeyboardUpFunc(releaseNormalKeys); // process standard key clicks
	glutSpecialFunc(pressSpecialKey); // process special key pressed
						// Warning: Nonstandard function! Delete if desired.
	glutSpecialUpFunc(releaseSpecialKey); // process special key release

	// OpenGL init
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	init();
	// enter GLUT event processing cycle
	glutMainLoop();

	return 0; // this is just to keep the compiler happy
}
