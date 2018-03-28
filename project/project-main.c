#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

#include "init.h"
#include "projectio.h"

Point3D lightSourcesColorsArr[] = { {1.0f, 0.0f, 0.0f}, // Red light

                                 {0.0f, 1.0f, 0.0f}, // Green light

                                 {0.0f, 0.0f, 1.0f}, // Blue light

                                 {1.0f, 1.0f, 1.0f} }; // White light

Point3D lightSourcesDirectionsPositionsStiff[] = { {10.0f, 5.0f, 0.0f}, // Red light, positional

                                       {0.0f, 5.0f, 10.0f}, // Green light, positional

                                       {-1.0f, 0.0f, 0.0f}, // Blue light along X

                                       {0.0f, 0.0f, -1.0f} }; // White light along Z

GLfloat specularExponent[] = {100.0, 200.0, 60.0, 550.0};

GLint isDirectional[] = {0,0,1,1};


Point3D lightSourcesDirectionsPositions[] = { {10.0f, 5.0f, 0.0f}, // Red light, positional

                                       {0.0f, 5.0f, 10.0f}, // Green light, positional

                                       {-1.0f, 0.0f, 0.0f}, // Blue light along X

                                       {0.0f, 0.0f, -1.0f} }; // White light along Z



float sphereSpeed;

// vertex array object
Model *m, *m2, *tm;

// Reference to shader program
GLuint program, texGrass, texMountain, texLake, texTerrain, texSphere;
mat4 sphereTransform, projectionMatrix;
TextureData ttex; // terrain
int t = 0;

mat4 camMatrix;
mat4 transform, rot, trans, total, complete, scale;
//SKYBOX STUFF!!!
Model *skyBox;
GLuint skyboxprogram;
mat4 skyBoxTransform;

unsigned int skyboxTex;

void drawSkybox(){
	glUseProgram(skyboxprogram);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	
	trans = T(0.0f, -0.1f, 0.0f);
	rot = Ry(0);
	total = Mult(rot, trans);
	scale = S(0.3f,2.0f,0.3f);
	total = Mult(total,scale);
	complete = Mult(skyBoxTransform, total);

	glUseProgram(skyboxprogram);
	glActiveTexture(GL_TEXTURE5);
	glUniform1i(glGetUniformLocation(skyboxprogram,"texUnit"),5);
	glUniform1i(glGetUniformLocation(skyboxprogram,"uniWrap"),1);
	glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "uniTotal"), 1, GL_TRUE, complete.m);

	DrawModel(skyBox, skyboxprogram, "in_Position", NULL, "inTexCoord");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void uploadLights(){
	glUseProgram(program);
	int i;
	for(i = 0; i <=3; i++){
	if(!isDirectional[i]){
	lightSourcesDirectionsPositions[i] = MultVec3(camMatrix, lightSourcesDirectionsPositionsStiff[i]);
		}
	}
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);

	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);
}

vec3 calculateNormal(int x, int z, int zLen, GLfloat *vertex){
	vec3 returnVector;
	vec3 a, b, c;

	if(x != 0){
		if(z != 0){
			a = SetVector(vertex[((x-1) + z * zLen)*3 + 0], vertex[((x-1) + z * zLen)*3 + 1], vertex[((x-1) + z * zLen)*3 + 2]);
			b = SetVector(vertex[(x + (z-1) * zLen)*3 + 0], vertex[(x + (z-1) * zLen)*3 + 1], vertex[(x + (z-1) * zLen)*3 + 2]);
			c = SetVector(vertex[((x-1) + (z-1) * zLen)*3 + 0], vertex[((x-1) + (z-1) * zLen)*3 + 1], vertex[((x-1) + (z-1) * zLen)*3 + 2]);
		} else {
			a = SetVector(vertex[((x-1) + z * zLen)*3 + 0], vertex[((x-1) + z * zLen)*3 + 1], vertex[((x-1) + z * zLen)*3 + 2]);
			b = SetVector(vertex[(x + (z+1) * zLen)*3 + 0], vertex[(x + (z+1) * zLen)*3 + 1], vertex[(x + (z+1) * zLen)*3 + 2]);
			c = SetVector(vertex[((x-1) + (z+1) * zLen)*3 + 0], vertex[((x-1) + (z+1) * zLen)*3 + 1], vertex[((x-1) + (z+1) * zLen)*3 + 2]);
		}
	} else {
		if(z != 0){
			a = SetVector(vertex[((x+1) + z * zLen)*3 + 0], vertex[((x+1) + z * zLen)*3 + 1], vertex[((x+1) + z * zLen)*3 + 2]);
			b = SetVector(vertex[(x + (z-1) * zLen)*3 + 0], vertex[(x + (z-1) * zLen)*3 + 1], vertex[(x + (z-1) * zLen)*3 + 2]);
			c = SetVector(vertex[((x+1) + (z-1) * zLen)*3 + 0], vertex[((x+1) + (z-1) * zLen)*3 + 1], vertex[((x+1) + (z-1) * zLen)*3 + 2]);
		} else {
			a = SetVector(vertex[((x+1) + z * zLen)*3 + 0], vertex[((x+1) + z * zLen)*3 + 1], vertex[((x+1) + z * zLen)*3 + 2]);
			b = SetVector(vertex[(x + (z+1) * zLen)*3 + 0], vertex[(x + (z+1) * zLen)*3 + 1], vertex[(x + (z+1) * zLen)*3 + 2]);
			c = SetVector(vertex[((x+1) + (z+1) * zLen)*3 + 0], vertex[((x+1) + (z+1) * zLen)*3 + 1], vertex[((x+1) + (z+1) * zLen)*3 + 2]);
		}
	}
	returnVector = CalcNormalVector(a,b,c);
	return returnVector;
}

Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	int x, z;

	GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount*3);

	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width)*3 + 0] = x / 1.0;
			vertexArray[(x + z * tex->width)*3 + 1] = 4*tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 100.0;
			vertexArray[(x + z * tex->width)*3 + 2] = z / 1.0;
// Normal vectors. You need to calculate these.
			if(x != 0 && z != 0){
				vec3 temp = calculateNormal(x,z, tex->width, vertexArray);

				normalArray[(x + z * tex->width)*3 + 0] = temp.x;	//N_x
				normalArray[(x + z * tex->width)*3 + 1] = temp.y;	//N_y
				normalArray[(x + z * tex->width)*3 + 2] = temp.z;	//N_z
			}
			if (x == tex->width-1 && z == tex->height-1){
				vec3 temp = calculateNormal(0,0, tex->width, vertexArray);

				normalArray[(0 + 0 * tex->width)*3 + 0] = temp.x;	//N_x
				normalArray[(0 + 0 * tex->width)*3 + 1] = temp.y;	//N_y
				normalArray[(0 + 0 * tex->width)*3 + 2] = temp.z;	//N_z
			}
// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)*2 + 0] = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z; // (float)z / tex->height;
		}
	for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
		// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
		// Triangle 2
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
		}

	// End of terrain generation

	// Create Model and upload to GPU:

	Model* model = LoadDataToModel(
			vertexArray,
			normalArray,
			texCoordArray,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);

	return model;
}

GLfloat calcHeight(GLfloat in_x, GLfloat in_z, int zLen, GLfloat *vertex){
	int x, z;
	vec3 n;
	x = (int) floor(in_x);
	z = (int) floor(in_z);
	float quad_x = in_x - x;
	float quad_z = in_z - z;
	GLfloat y_o, y_x, y_z, y_xz, res, d;

	if(quad_x + quad_z <= 1){	//Near triangle in quad
		y_o = vertex[(x + z * zLen)*3 + 1];
		y_x = vertex[((x+1) + z * zLen)*3 + 1];
		y_z = vertex[(x + (z+1) * zLen)*3 + 1];
		vec3 ox, oz;
		ox = SetVector(1,y_x-y_o,0);
		oz = SetVector(0,y_z-y_o,1);
		n = CrossProduct(ox,oz);
		d = -(n.x*x + n.y*y_o + n.z*z);
		res = (-d -n.x*in_x-n.z*in_z)/n.y;
		if(t == -10){
			printf("X_in: %f, Z_in: %f\n", in_x, in_z);
			printf("X: %f, Z: %f, res: %f\n", quad_x, quad_z, res);
		}
		return res + 1.0f;
	} else {			//Far triangle in quad
		y_xz = vertex[((x+1) + (z+1) * zLen)*3 + 1];
		y_x = vertex[((x+1) + z * zLen)*3 + 1];
		y_z = vertex[(x + (z+1) * zLen)*3 + 1];
		vec3 xz_x, xz_z;
		xz_x = SetVector(1-1,y_x-y_xz,0-1);
		xz_z = SetVector(0-1,y_z-y_xz,1-1);
		n = CrossProduct(xz_x,xz_z);
		d = -(n.x*(x+1) + n.y*y_xz + n.z*(z+1));
		res = (-d -n.x*in_x-n.z*in_z)/n.y;
		if(t == -10){
			printf("X_in: %f, Z_in: %f\n", in_x, in_z);
			printf("X: %f, Z: %f, res: %f\n", quad_x, quad_z, res);
		}
		return res + 1.0f;
	}
}




void display(void)
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Skybox
	drawSkybox();

	mat4 total, modelView;

	printError("pre display");

	glUseProgram(program);

	// Build matrix

	modelView = IdentityMatrix();
	total = Mult(camMatrix, modelView);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);

	glActiveTexture(GL_TEXTURE0);				//Texture unit 0
	glBindTexture(GL_TEXTURE_2D, texGrass);		// Bind Our Texture texGrass
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texMountain);
	glUniform1i(glGetUniformLocation(program, "texMountain"), 1); // Texture unit 1
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texLake);
	glUniform1i(glGetUniformLocation(program, "texLake"), 2); // Texture unit 2
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texTerrain);
	glUniform1i(glGetUniformLocation(program, "texTerrain"), 3); // Texture unit 3
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, texSphere);
	glUniform1i(glGetUniformLocation(program, "texSphere"), 4); // Texture unit 4
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, skyboxTex);
	glUniform1i(glGetUniformLocation(skyboxprogram, "skyboxTex"), 5); // Texture unit 5
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program, "color"), false);
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

	printError("display 2");

	//Draw sphere
	mat4 trans = T(0.0f, 0.0f, 0.0f);
	mat4 rot = Ry(0);
	mat4 scale = S(1.0f,1.0f,1.0f);
	total = Mult(trans,scale);
	total = Mult(rot, total);
	total.m[7] = calcHeight(sphereTransform.m[3], sphereTransform.m[11], ttex.width, tm->vertexArray);
	mat4 complete = Mult(camMatrix,Mult(total,sphereTransform));
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, complete.m);
	glUniform1i(glGetUniformLocation(program, "color"), true);
	DrawModel(m, program, "inPosition", "inNormal", "inTexCoord");

	glutSwapBuffers();
}

void timer(int i)
{
	glutTimerFunc(20, &timer, i);
	checkInput();
	glutPostRedisplay();
}

void mouse(int x, int y)
{
	//printf("%d %d\n", x, y);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (600, 600);
	glutCreateWindow ("TSBK07 Lab 4");
	glutDisplayFunc(display);
	init ();
	glutTimerFunc(20, &timer, 0);

	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	exit(0);
}
