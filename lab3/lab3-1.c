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
#include "../common/loadobj.h"
#include "../common/LoadTGA.h"
#include "../common/VectorUtils3.h"


#define near 1.0

#define far 30.0

#define right 0.5

#define left -0.5

#define top 0.5

#define bottom -0.5

GLfloat color[] = 
{
	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	0.0f,0.0f,1.0f

	
};


GLfloat projectionMatrix[] = {
	2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f, 
	0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
	0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
	0.0f, 0.0f, -1.0f, 0.0f 
};


mat4 cameraMatrix;


mat4 transform, rot, trans, total, complete, wmWallTransform, wmRoofTransform, wmBalconyTransform;
mat4 wmBladeTransform, transBlade, rotBlade;
//Maek teh madelz!
Model *wmWalls;
Model *wmRoof;
Model *wmBalcony;
Model *wmBlade;
vec3 p, l, v;


// vertex array object
GLfloat t;
GLuint program;



void init(void)
{
	// Load windmill
	unsigned int myTex;

	LoadTGATextureSimple("skybox.tga", &myTex);
	
	transform = IdentityMatrix();
	//Load windmill Models
	wmWalls = LoadModelPlus("windmill/windmill-walls.obj");
	wmRoof = LoadModelPlus("windmill/windmill-roof.obj");
	wmBalcony = LoadModelPlus("windmill/windmill-balcony.obj");
	wmBlade = LoadModelPlus("windmill/blade.obj");
	dumpInfo();

	cameraMatrix = IdentityMatrix();
	wmRoofTransform = IdentityMatrix();
	wmWallTransform = IdentityMatrix();
	wmBalconyTransform = IdentityMatrix();
	wmBladeTransform = IdentityMatrix();
	

	// GL inits
	glClearColor(0.3,0.3,0.3,0);
	//glDisable(GL_DEPTH_TEST);
	printError("GL inits");
	
	//Projection
	p = SetVector(0.0f,10.0f,20.0f);
	l = SetVector(0,10.0f,0);
	v = SetVector(0,1,0);
	cameraMatrix = lookAtv(p,l,v);
	
	// Load and compile shader
	program = loadShaders("lab3-1.vert", "lab3-1.frag");
	printError("init shader");
	
	glUniformMatrix4fv(glGetUniformLocation(program, "uniProjection"), 1, GL_TRUE, projectionMatrix);
	
	glBindTexture(GL_TEXTURE_2D, myTex);
	printError("init arrays");

	

}


void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	GLfloat mod1Color[] = {0.0f, 0.4f, 0.8f, 0.0f};
	GLfloat mod2Color[] = {0.4f, 0.4f, 0.4f, 0.0f};
	
	
	


	//Windmill transforms	
	trans = T(0.0f, 0, 0.0f);
	rot = Ry(0);
	total = Mult(rot, trans);

	//Walls
	//Move object to camera
	complete = Mult(cameraMatrix,Mult(total,wmWallTransform));
	glUniformMatrix4fv(glGetUniformLocation(program, "uniTotal"), 1, GL_TRUE, complete.m);
	DrawModel(wmWalls, program, "in_Position", "in_Normal", "inTexCoord");

	//Roof
	//Move object to camera
	complete = Mult(cameraMatrix,Mult(total,wmRoofTransform));
	glUniformMatrix4fv(glGetUniformLocation(program, "uniTotal"), 1, GL_TRUE, complete.m);
	DrawModel(wmRoof, program, "in_Position", "in_Normal", "inTexCoord");

	//Balcony
	//Move object to camera
	complete = Mult(cameraMatrix,Mult(total,wmBalconyTransform));
	glUniformMatrix4fv(glGetUniformLocation(program, "uniTotal"), 1, GL_TRUE, complete.m);
	DrawModel(wmBalcony, program, "in_Position", "in_Normal", "inTexCoord");

	//Blades
	int i;
	mat4 rotBlade = Rx(t/2.0f);
	transBlade = Mult(T(4.5f, 9.2f, 0.1f) , trans);
	for(i = 4; i >= 1 ; i--){
		//Move object to camera
		rotBlade = Mult(rotBlade, Rx((M_PI/2)*i));
		total = Mult( rot, Mult(transBlade,rotBlade));
		complete = Mult(cameraMatrix,Mult(total,wmBladeTransform));
		glUniformMatrix4fv(glGetUniformLocation(program, "uniTotal"), 1, GL_TRUE, complete.m);
		DrawModel(wmBlade, program, "in_Position", "in_Normal", "inTexCoord");
	}


	printError("display");
	
	glutSwapBuffers();
}
//uniProjection * uniCamera * uniTotal * uniTransform

void onTimer(int value)
{
	
	t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) * M_PI / (2*1000);
	//Camera Controls
	if(glutKeyIsDown('w')){
		//p = SetVector(p.x, p.y+0.2f, p.z);
		cameraMatrix = Mult(cameraMatrix, T(0,0,0.2f));
	}	
	if(glutKeyIsDown('s')){
		//p = SetVector(p.x, p.y-0.2f, p.z);
		cameraMatrix = Mult(cameraMatrix, T(0,0,-0.2f));
	}	
	if(glutKeyIsDown('a')){
		//p = SetVector(p.x-0.2f, p.y, p.z);
		cameraMatrix = Mult(cameraMatrix, T(0.2f,0,0));
	}	
	if(glutKeyIsDown('d')){
		//p = SetVector(p.x+0.2f, p.y, p.z);
		cameraMatrix = Mult(cameraMatrix, T(-0.2f,0,0));
	}	
	if(glutKeyIsDown('i')){
		//p = SetVector(p.x, p.y, p.z-0.2f);
		cameraMatrix = Mult(cameraMatrix, T(0,-0.2f,0));
	}	
	if(glutKeyIsDown('k')){
		//p = SetVector(p.x, p.y, p.z+0.2f);
		cameraMatrix = Mult(cameraMatrix, T(0,0.2f,0));
	}
	if(glutKeyIsDown('j')){
		//p = SetVector(p.x, p.y, p.z+0.2f);
		cameraMatrix = Mult(Ry(-1*M_PI/180),cameraMatrix );
	}
	if(glutKeyIsDown('l')){
		//p = SetVector(p.x, p.y, p.z+0.2f);
		cameraMatrix = Mult(Ry(1*M_PI/180),cameraMatrix );
	}	
	


	glUniform1f(glGetUniformLocation(program, "uniTime"), t*10);
	glutPostRedisplay();	
	glutTimerFunc(20, &onTimer, value); //50 FPS
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("Spinning windmill");
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(display);
	glutTimerFunc(20, &onTimer, 0); 
	init ();
	glutMainLoop();
	return 0;
}
