// Simple demo showing how "splatting" is performed, that is
// multitexturing controlled by a map.

// Extended version, allowing for five different texture
// controlled by an RGBA image.

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// Linking hint for Lightweight IDE:
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

Model *m;
// Reference to shader program
GLuint program;
GLuint grass, conc, dirt, redsquares, water, map;

void init(void)
{
	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("tex.vert", "tex.frag");
	glUseProgram(program);
	printError("init shader");
	
	// Upload geometry to the GPU:
	m = LoadModelPlus("grid2.obj");
	
	// End of upload of geometry

	mat4 look = lookAt(4,3,0, 0,0,0, 0,1,0);
	mat4 projectionMatrix = frustum(-0.5, 0.5, -0.5, 0.5, 1.0, 30.0);

	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, look.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	
	glUniform1i(glGetUniformLocation(program, "grass"), 0); // Texture unit 0
	glUniform1i(glGetUniformLocation(program, "conc"), 1); // Texture unit 1
	glUniform1i(glGetUniformLocation(program, "dirt"), 2); // Texture unit 2
	glUniform1i(glGetUniformLocation(program, "redsquares"), 3); // Texture unit 3
	glUniform1i(glGetUniformLocation(program, "water"), 4); // Texture unit 4
	glUniform1i(glGetUniformLocation(program, "map"), 5); // Texture unit 5

	LoadTGATextureSimple("grass.tga", &grass);
	LoadTGATextureSimple("conc.tga", &conc);
	LoadTGATextureSimple("dirt.tga", &dirt);
	LoadTGATextureSimple("redsquares.tga", &redsquares);
	LoadTGATextureSimple("water.tga", &water);
	LoadTGATextureSimple("map.tga", &map);
	
	// Bind to texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grass);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, conc);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dirt);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, redsquares);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, water);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, map);
	
	glUniform1i(glGetUniformLocation(program, "opt"), 1);
}

GLfloat a = 0.0;
int opt = 0;
int rot = 1;

void key(unsigned char k, int x, int y)
{
	if (k == 'm')
	{
		opt = !opt;
		glUniform1i(glGetUniformLocation(program, "opt"), opt);
	}
	if (k == ' ')
	{
		rot = !rot;
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (rot)
		a += 0.05;
	mat4 rot = Ry(a/2);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, rot.m);

	DrawModel(m, program, "inPosition", "inNormal", "inTexCoord");

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutCreateWindow ("GL3 texture splatting example");
	glutDisplayFunc(display);
	init ();
	glutKeyboardFunc(key);
	glutRepeatingTimerFunc(20);
	glutMainLoop();
	exit(0);
}

