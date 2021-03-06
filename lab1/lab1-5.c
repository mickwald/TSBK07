// Lab 1-1.
// This is the same as the first simple example in the course book,
// but with a few error checks.
// Remember to copy your file to a new on appropriate places during the lab so you keep old results.
// Note that the files "lab1-1.frag", "lab1-1.vert" are required.

// Should work as is on Linux and Mac. MS Windows needs GLEW or glee.
// See separate Visual Studio version of my demos.
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include <math.h>
#include <stdio.h>
//#include "../common/VectorUtils3.h"

// Globals
// Data would normally be read from files
GLfloat vertices[] =
{
	//Front	
	-0.5f,-0.5f,0.5f,
	-0.5f,0.5f,0.5f,
	0.5f,-0.5f,0.5f,
	
	-0.5f,0.5f,0.5f,
	0.5f,0.5f,0.5f,
	0.5f,-0.5f,0.5f,
	
	//Back
	-0.5f,-0.5f,-0.5f,
	-0.5f,0.5f,-0.5f,
	0.5f,-0.5f,-0.5f,
	
	-0.5f,0.5f,-0.5f,
	0.5f,0.5f,-0.5f,
	0.5f,-0.5f,-0.5f,
	
	//Left
	-0.5f,0.5f,0.5f,
	-0.5f,-0.5f,0.5f,
	-0.5f,-0.5f,-0.5f,

	-0.5f,0.5f,0.5f,
	-0.5f,0.5f,-0.5f,
	-0.5f,-0.5f,-0.5f,
	
	//Right
	0.5f,0.5f,0.5f,
	0.5f,-0.5f,0.5f,
	0.5f,-0.5f,-0.5f,

	0.5f,0.5f,0.5f,
	0.5f,0.5f,-0.5f,
	0.5f,-0.5f,-0.5f,
	
	//Top
	-0.5f,0.5f,0.5f,
	-0.5f,0.5f,-0.5f,
	0.5f,0.5f,0.5f,

	0.5f,0.5f,0.5f,
	0.5f,0.5f,-0.5f,
	-0.5f,0.5f,-0.5f,

	//Bottom
	-0.5f,-0.5f,0.5f,
	-0.5f,-0.5f,-0.5f,
	0.5f,-0.5f,0.5f,

	0.5f,-0.5f,0.5f,
	0.5f,-0.5f,-0.5f,
	-0.5f,-0.5f,-0.5f
};

GLfloat color[] = 
{
	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f,

	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f,

	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f,

	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f,

	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f,

	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f,

	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f,

	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f,

	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f,

	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f,

	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f,

	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f
};

GLfloat myMatrix[] = {  1.0f, 0.0f, 0.0f, 0.0f,

                        0.0f, 1.0f, 0.0f, 0.0f,

                        0.0f, 0.0f, 1.0f, 0.0f,

                        0.0f, 0.0f, 0.0f, 1.0f };

GLfloat Rotx[] = {  1.0f, 0.0f, 0.0f, 0.0f,

                        0.0f, 1.0f, 0.0f, 0.0f,

                        0.0f, 0.0f, 1.0f, 0.0f,

                        0.0f, 0.0f, 0.0f, 1.0f };

GLfloat Roty[] = {  1.0f, 0.0f, 0.0f, 0.0f,

                        0.0f, 1.0f, 0.0f, 0.0f,

                        0.0f, 0.0f, 1.0f, 0.0f,

                        0.0f, 0.0f, 0.0f, 1.0f };

GLfloat Rotz[] = {  1.0f, 0.0f, 0.0f, 0.0f,

                        0.0f, 1.0f, 0.0f, 0.0f,

                        0.0f, 0.0f, 1.0f, 0.0f,

                        0.0f, 0.0f, 0.0f, 1.0f };



// vertex array object
unsigned int vertexArrayObjID;
GLfloat t;
GLfloat oldT;
GLuint program;

void init(void)
{
	// vertex buffer object, used for uploading the geometry
	unsigned int vertexBufferObjID;
	unsigned int colorBufferObjID;
	

	dumpInfo();

	// GL inits
	glClearColor(0.3,0.3,0.3,0);
	//glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	
	// Load and compile shader
	program = loadShaders("lab1-5.vert", "lab1-5.frag");
	printError("init shader");
	
	// Upload geometry to the GPU:
	
	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);
	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);
	
	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 12*9*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));
	
	// End of upload of geometry
	// Upload color to the GPU:

	glGenBuffers(1, &colorBufferObjID);

	//VBO for color data
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 12*9*sizeof(GLfloat), color, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_vertcolor"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_vertcolor"));
	// Upload transform to shader
	glUniformMatrix4fv(glGetUniformLocation(program, "myMatrix"), 1, GL_TRUE, myMatrix);
	// End of transform upload
	
	printError("init arrays");

}


void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vertexArrayObjID);	// Select VAO
	glDrawArrays(GL_TRIANGLES, 0, 12*3);	// draw object
	// Upload transform to shader
	glUniformMatrix4fv(glGetUniformLocation(program, "myMatrix"), 1, GL_TRUE, myMatrix);
	// End of transform upload 
	
	printError("display");
	
	glutSwapBuffers();
}


void doTransformTick(GLfloat time)
{
	
	//myMatrix[0] = cos(time);
	//myMatrix[2] = sin(time);
	//myMatrix[8] = -sin(time);
	//myMatrix[10] = cos(time);
	Rotx[5] = cos(time);
	Rotx[6] = -sin(time);
	Rotx[9] = sin(time);
	Rotx[10] = cos(time);
	Roty[0] = cos(time);
	Roty[3] = sin(time);
	Roty[8] = -sin(time);
	Roty[10] = cos(time);
	Rotz[0] = cos(time);
	Rotz[4] = sin(time);
	Rotz[1] = -sin(time);
	Rotz[5] = cos(time);
	/*myMatrix = Mult(Rx(time), Ry(time));
	myMatrix = (GLfloat) Mult((mat4)myMatrix, Rz(time));*/
}

void onTimer(int value)
{
	
	t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) * M_PI / (2*1000);
	doTransformTick(t);
	glUniformMatrix4fv(glGetUniformLocation(program, "Rotx"), 1, GL_TRUE, Rotx);
	glUniformMatrix4fv(glGetUniformLocation(program, "Roty"), 1, GL_TRUE, Roty);
	glUniformMatrix4fv(glGetUniformLocation(program, "Rotz"), 1, GL_TRUE, Rotz);
	glutPostRedisplay();	
	glutTimerFunc(20, &onTimer, value); //50 FPS
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("GL3 white triangle example");
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(display);
	glutTimerFunc(20, &onTimer, 0); 
	init ();
	glutMainLoop();
	return 0;
}
