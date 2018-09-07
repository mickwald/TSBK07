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

#define far 100000.0

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

Point3D lightSourcesColorsArr[] = { {1.0f, 0.0f, 0.0f}, // Red light

                                 {0.0f, 1.0f, 0.0f}, // Green light

                                 {0.0f, 0.0f, 1.0f}, // Blue light

                                 {1.0f, 1.0f, 1.0f} }; // White light

Point3D lightSourcesDirectionsPositionsStiff[] = { {10.0f, 5.0f, 0.0f}, // Red light, positional

                                       {0.0f, 5.0f, 10.0f}, // Green light, positional

                                       {-1.0f, 0.0f, 0.0f}, // Blue light along X

                                       {0.0f, 0.0f, -1.0f} }; // White light along Z

GLfloat specularExponent[] = {100.0, 200.0, 60.0, 50.0};

GLint isDirectional[] = {0,0,1,1};


Point3D lightSourcesDirectionsPositions[] = { {10.0f, 5.0f, 0.0f}, // Red light, positional

                                       {0.0f, 5.0f, 10.0f}, // Green light, positional

                                       {-1.0f, 0.0f, 0.0f}, // Blue light along X

                                       {0.0f, 0.0f, -1.0f} }; // White light along Z

mat4 transform, rot, trans, total, complete, wmWallTransform, wmRoofTransform, wmBalconyTransform;
mat4 wmBladeTransform, transBlade, rotBlade, groundBoxTransform, scale, cameraMatrix, world, bunnyTransform;
//Maek teh madelz!
Model *wmWalls;
Model *wmRoof;
Model *wmBalcony;
Model *wmBlade;
Model *groundBox;
Model *skyBox;
Model *bunny;
vec3 p, l, v;


unsigned int vertexArrayObjID;


// vertex array object
GLfloat t;
GLuint program;
GLuint noshadeprogram;
GLuint bunnyprogram;


unsigned int skyboxTex;
unsigned int groundTex;

void drawSkybox(){
	glUseProgram(noshadeprogram);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	mat4 skyBoxTransform = cameraMatrix;
	skyBoxTransform.m[3] = 0;
	skyBoxTransform.m[7] = 0;
	skyBoxTransform.m[11] = 0;
	trans = T(0.0f, -0.3f, 0.0f);
	rot = Ry(0);
	total = Mult(rot, trans);
	scale = S(1.0f,2.0f,1.0f);
	total = Mult(total,scale);
	complete = Mult(skyBoxTransform, total);
	
	
	glUniform1i(glGetUniformLocation(noshadeprogram,"texUnit"),1);
	glUniform1i(glGetUniformLocation(noshadeprogram,"uniWrap"),1);	
	glUniformMatrix4fv(glGetUniformLocation(noshadeprogram, "uniTotal"), 1, GL_TRUE, complete.m);
	
	DrawModel(skyBox, noshadeprogram, "in_Position", NULL, "inTexCoord");
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void drawBunny(){
	glUseProgram(bunnyprogram);
	trans = T(0.0f, 0.5f, 6.0f);
	rot = Ry(-0.5f*M_PI);
	total = Mult(rot, trans);

	//Move object to camera
	complete = Mult(cameraMatrix,Mult(total,bunnyTransform));
	world = Mult(total,bunnyTransform);
	glUniform1i(glGetUniformLocation(bunnyprogram,"texUnit0"),0);
	glUniform1i(glGetUniformLocation(bunnyprogram,"texUnit1"),1);
	glUniformMatrix4fv(glGetUniformLocation(bunnyprogram, "uniTotal"), 1, GL_TRUE, complete.m);
	glUniformMatrix4fv(glGetUniformLocation(bunnyprogram, "uniWorld"), 1, GL_TRUE, world.m);
	DrawModel(bunny, bunnyprogram, "in_Position", "in_Normal", "inTexCoord");
}

void drawWindmill(){
	glUseProgram(program);
	
	//Windmill transforms	
	trans = T(0.0f, 6.0f, 6.0f);
	rot = Rz(3*t);
	total = Mult(rot, trans);

	//Walls
	//Move object to camera
	complete = Mult(cameraMatrix,Mult(total,wmWallTransform));
	world = Mult(total,wmWallTransform);
	glUniformMatrix4fv(glGetUniformLocation(program, "uniTotal"), 1, GL_TRUE, complete.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "uniWorld"), 1, GL_TRUE, world.m);
	DrawModel(wmWalls, program, "in_Position", "in_Normal", "inTexCoord");

	//Roof
	//Move object to camera
	complete = Mult(cameraMatrix,Mult(total,wmRoofTransform));
	world = Mult(total,wmRoofTransform);
	glUniformMatrix4fv(glGetUniformLocation(program, "uniTotal"), 1, GL_TRUE, complete.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "uniWorld"), 1, GL_TRUE, world.m);
	DrawModel(wmRoof, program, "in_Position", "in_Normal", "inTexCoord");

	//Balcony
	//Move object to camera
	complete = Mult(cameraMatrix,Mult(total,wmBalconyTransform));
	world = Mult(total,wmBalconyTransform);
	glUniformMatrix4fv(glGetUniformLocation(program, "uniTotal"), 1, GL_TRUE, complete.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "uniWorld"), 1, GL_TRUE, world.m);
	DrawModel(wmBalcony, program, "in_Position", "in_Normal", "inTexCoord");

	//Blades
	int i;
	mat4 rotBlade = Rx(10.0f*t/2.0f);
	transBlade = Mult(T(4.5f, 9.2f, 0.1f) , trans);
	for(i = 4; i >= 1 ; i--){
		//Move object to camera
		rotBlade = Mult(rotBlade, Rx((M_PI/2)*i));
		total = Mult( rot, Mult(transBlade,rotBlade));
		complete = Mult(cameraMatrix,Mult(total,wmBladeTransform));
		world = Mult(total,wmBladeTransform);
		glUniformMatrix4fv(glGetUniformLocation(program, "uniTotal"), 1, GL_TRUE, complete.m);
		glUniformMatrix4fv(glGetUniformLocation(program, "uniWorld"), 1, GL_TRUE, world.m);
		DrawModel(wmBlade, program, "in_Position", "in_Normal", "inTexCoord");
	}
}

void drawGround(){
	glUseProgram(noshadeprogram);
	trans = T(0.0f, -1.0f, 0.0f);
	rot = Ry(0);
	total = Mult(rot, trans);
	scale = S(100.0f,0.5f,100.0f);
	total = Mult(total,scale);
	complete = Mult(cameraMatrix,Mult(total,groundBoxTransform));

	glUniform1i(glGetUniformLocation(noshadeprogram,"uniWrap"),100);
	glUniform1i(glGetUniformLocation(noshadeprogram,"texUnit"),0);
	glUniformMatrix4fv(glGetUniformLocation(noshadeprogram, "uniTotal"), 1, GL_TRUE, complete.m);
	DrawModel(groundBox, noshadeprogram, "in_Position", NULL, "inTexCoord");
}

void CheckInput(){
	if(glutKeyIsDown('w')){
		cameraMatrix = Mult(cameraMatrix, T(-0.4f*cameraMatrix.m[2],0,0.4f*cameraMatrix.m[0]));
	}	
	if(glutKeyIsDown('s')){
		cameraMatrix = Mult(cameraMatrix, T(0.4f*cameraMatrix.m[2],0,-0.4f*cameraMatrix.m[0]));
	}	
	if(glutKeyIsDown('a')){
		cameraMatrix = Mult(cameraMatrix, T(0.4f*cameraMatrix.m[0],0,0.4f*cameraMatrix.m[2]));
	}	
	if(glutKeyIsDown('d')){
		cameraMatrix = Mult(cameraMatrix, T(-0.4f*cameraMatrix.m[0],0,-0.4f*cameraMatrix.m[2]));
	}	
	if(glutKeyIsDown('i')){
		cameraMatrix = Mult(cameraMatrix, T(0,-0.2f,0));
	}	
	if(glutKeyIsDown('k')){
		cameraMatrix = Mult(cameraMatrix, T(0,0.2f,0));
	}
	if(glutKeyIsDown('j')){
		cameraMatrix = Mult(Ry(-1*M_PI/180),cameraMatrix );
	}
	if(glutKeyIsDown('l')){
		cameraMatrix = Mult(Ry(1*M_PI/180),cameraMatrix );
	}	
}

void loadModels(){
	wmWalls = LoadModelPlus("windmill/windmill-walls.obj");
	wmRoof = LoadModelPlus("windmill/windmill-roof.obj");
	wmBalcony = LoadModelPlus("windmill/windmill-balcony.obj");
	wmBlade = LoadModelPlus("windmill/blade.obj");
	groundBox = LoadModelPlus("groundbox.obj");
	bunny = LoadModelPlus("../lab2/bunnyplus.obj");
	skyBox = LoadModelPlus("skybox.obj");
}

void initTransforms(){
	cameraMatrix = wmRoofTransform = wmWallTransform = wmBalconyTransform = wmBladeTransform = groundBoxTransform = transform = bunnyTransform = IdentityMatrix();
}


void uploadProjectionMatrices(){
	glUseProgram(noshadeprogram);
	glUniformMatrix4fv(glGetUniformLocation(noshadeprogram, "uniProjection"), 1, GL_TRUE, projectionMatrix);
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "uniProjection"), 1, GL_TRUE, projectionMatrix);
	glUseProgram(bunnyprogram);
	glUniformMatrix4fv(glGetUniformLocation(bunnyprogram, "uniProjection"), 1, GL_TRUE, projectionMatrix);
}


void uploadTextures(){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, groundTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, skyboxTex);
}

void uploadLights(){
	glUseProgram(program);
	int i;
	for(i = 0; i <=3; i++){
	if(!isDirectional[i]){
	lightSourcesDirectionsPositions[i] = MultVec3(cameraMatrix, lightSourcesDirectionsPositionsStiff[i]);
		}
	}
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);

	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);

	glUseProgram(bunnyprogram);
	for(i = 0; i <=3; i++){
	if(!isDirectional[i]){
	lightSourcesDirectionsPositions[i] = MultVec3(cameraMatrix, lightSourcesDirectionsPositionsStiff[i]);
		}
	}
	glUniform3fv(glGetUniformLocation(bunnyprogram, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);

	glUniform3fv(glGetUniformLocation(bunnyprogram, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1fv(glGetUniformLocation(bunnyprogram, "specularExponent"), 4, specularExponent);
	glUniform1iv(glGetUniformLocation(bunnyprogram, "isDirectional"), 4, isDirectional);


}

void initShaders(){
	program = loadShaders("lab3-1.vert", "lab3-1.frag");
	noshadeprogram = loadShaders("noshade.vert", "noshade.frag");
	bunnyprogram = loadShaders("bunny.vert", "bunny.frag");
}


//Init here
void init(void)
{
	// Load windmill

	LoadTGATextureSimple("SkyBox512.tga", &skyboxTex);
	LoadTGATextureSimple("Grass_tile_B_diffuse.tga", &groundTex);
	
	//Load Models
	loadModels();
	dumpInfo();
	
	initTransforms();

	// GL inits
	glClearColor(0.3,0.3,0.3,0);
	printError("GL inits");
	
	//Projection
	p = SetVector(0.0f,10.0f,20.0f);
	l = SetVector(0,10.0f,0);
	v = SetVector(0,1,0);
	cameraMatrix = lookAtv(p,l,v);
	
	// Load and compile shader
	initShaders();
	printError("init shader");
	
	uploadProjectionMatrices();
	
	uploadTextures();

	printError("init arrays");

	uploadLights();


	

}


//Draw call
void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Skybox
	drawSkybox();

	//Windmill
	drawWindmill();

	//Bunneh
	drawBunny();
	
	//Ground
	drawGround();

	//Lights
	uploadLights();

	printError("display");
	
	glutSwapBuffers();
}


void onTimer(int value)
{
	
	t = (GLfloat) glutGet(GLUT_ELAPSED_TIME) * M_PI / (2*1000);
	//Camera Controls
	CheckInput();

	glUniform1f(glGetUniformLocation(program, "uniTime"), t*10);
	glutPostRedisplay();	
	glutTimerFunc(20, &onTimer, value); //50 FPS
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("Camo bunny!");
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(display);
	glutTimerFunc(20, &onTimer, 0); 
	init ();
	glutMainLoop();
	return 0;
}



