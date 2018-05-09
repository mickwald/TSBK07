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
#define __debug__ 0



drawObject *drawObjects;
int drawArrayElements;
int drawArraySize;

float sphereSpeed;

int lockCamera = 0;
int loops = 0;

// vertex array object
Model *sphereModel, *tm;

// Reference to shader program
GLuint program, texGrass, texMountain, texLake, texTerrain, texSphere;
mat4 sphereTransform, projectionMatrix;
TextureData ttex; // terrain
int t = 0;

mat4 camMatrix;
mat4 transform, rot, trans, total, complete, scale, sphereModelMat;
//SKYBOX STUFF!!!
Model *skyBox;
GLuint skyboxprogram;
mat4 skyBoxTransform;
Collider playerCol;
mat4 tmpPlayerMat;

float rotTest = 0;

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
		if(__debug__ && !t){
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
		if(__debug__ && !t){
			printf("X_in: %f, Z_in: %f\n", in_x, in_z);
			printf("X: %f, Z: %f, res: %f\n", quad_x, quad_z, res);
		}
		float sphereOffset = 1.0f;
		return res + sphereOffset;
	}
}

vec3 calcSlope(GLfloat in_x, GLfloat in_z, int zLen, GLfloat *vertex){
	//vec3 spherePos = SetVector(sphereTransform[3],sphereTransform[7],sphereTransform[11]);
	int x, z;
	if(__debug__ && !t){
		printf("X_in: %f, Z_in: %f\n", in_x, in_z);
	}
	vec3 n;
	x = (int) floor(in_x);
	z = (int) floor(in_z);
	float quad_x = in_x - x;
	float quad_z = in_z - z;
	GLfloat y_o, y_x, y_z, y_xz;

	if(quad_x + quad_z <= 1){	//Near triangle in quad
		y_o = vertex[(x + z * zLen)*3 + 1];
		y_x = vertex[((x+1) + z * zLen)*3 + 1];
		y_z = vertex[(x + (z+1) * zLen)*3 + 1];
		vec3 ox, oz;
		ox = SetVector(1,y_x-y_o,0);
		oz = SetVector(0,y_z-y_o,1);
		n = CrossProduct(ox,oz);

		return  n;
	} else {			//Far triangle in quad
		y_xz = vertex[((x+1) + (z+1) * zLen)*3 + 1];
		y_x = vertex[((x+1) + z * zLen)*3 + 1];
		y_z = vertex[(x + (z+1) * zLen)*3 + 1];
		vec3 xz_x, xz_z;
		xz_x = SetVector(1-1,y_x-y_xz,0-1);
		xz_z = SetVector(0-1,y_z-y_xz,1-1);
		n = CrossProduct(xz_z, xz_x);

		return n;
	}
}

mat4 calcSlopeRotMat(){
	vec3 tSlope = calcSlope(sphereTransform.m[3], sphereTransform.m[11], ttex.width, tm->vertexArray);
	tSlope = ScalarMult(Normalize(tSlope),-1);
	vec3 x_Hat = SetVector(1.0f,0.0f, 0.0f);
	vec3 zVec = Normalize(CrossProduct(x_Hat, tSlope));
	vec3 xVec = Normalize(CrossProduct(tSlope,zVec));

	mat4 slopeRotMat = IdentityMatrix();

	//radvis
	slopeRotMat.m[0] = xVec.x;
	slopeRotMat.m[1] = tSlope.x;
	slopeRotMat.m[2] = zVec.x;
	slopeRotMat.m[4] = xVec.y;
	slopeRotMat.m[5] = tSlope.y;
	slopeRotMat.m[6] = zVec.y;
	slopeRotMat.m[8] = xVec.z;
	slopeRotMat.m[9] = tSlope.z;
	slopeRotMat.m[10] = zVec.z;

	return slopeRotMat;

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


	//Draw World
	//mat4 worldMatrix = IdentityMatrix();
	//glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, worldMatrix.m);
	glUniform1i(glGetUniformLocation(program, "color"), false);
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

	printError("display 2");

	//Draw drawObjects
	int i = 0;
	while(i < drawArrayElements){
		GLfloat objectHeight = calcHeight(drawObjects[i].objectTransform.m[3], drawObjects[i].objectTransform.m[11], ttex.width, tm->vertexArray);
		drawObjects[i].objectTransform.m[7] = objectHeight;
		//drawObjects[i].objectTransform.m[7] = objectHeight;
		drawObjects[i].col.midPoint.y = objectHeight;
		mat4 model = Mult(drawObjects[i].trans,drawObjects[i].scale);
		model = Mult(drawObjects[i].rot, model);
		mat4 modelToWorld = Mult(model, drawObjects[i].objectTransform);
		mat4 modelToView = Mult(camMatrix, modelToWorld);
		glUniformMatrix4fv(glGetUniformLocation(drawObjects[i].shaderprogram, "mdlMatrix"), 1, GL_TRUE, modelToView.m);
		glUniform1i(glGetUniformLocation(drawObjects[i].shaderprogram, "color"), true);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, drawObjects[i].texNum);
		glUniform1i(glGetUniformLocation(program, "texSphere"), 4); // Texture unit 4
		//printf("Drawing item %d.\n", i);
		DrawModel(drawObjects[i].m, drawObjects[i].shaderprogram, "inPosition", "inNormal", "inTexCoord");
		if(i==1){
			//printf("x: %F y: %f z: %f\n", drawObjects[0].objectTransform.m[3], drawObjects[0].objectTransform.m[7], drawObjects[0].objectTransform.m[11]);
		}
		i++;
	}
	i = 0;

	//Draw sphere
	//Get correct texture
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, texSphere);
	glUniform1i(glGetUniformLocation(program, "texSphere"), 4); // Texture unit 4
	//Transformations
	mat4 slopeRotMat = calcSlopeRotMat();
	mat4 trans = T(0.0f, 0.0f, 0.0f);
	mat4 rot = Ry(rotTest);
	mat4 scale = S(1.0f,1.0f,1.0f);
	sphereModelMat = Mult(trans,scale);
	sphereModelMat = Mult(rot, sphereModelMat);
	//Set Height
	sphereTransform.m[7] = calcHeight(sphereTransform.m[3], sphereTransform.m[11], ttex.width, tm->vertexArray);

	mat4 modelToWorld = Mult(sphereTransform,Mult(slopeRotMat,sphereModelMat));
	mat4 complete = Mult(camMatrix, modelToWorld);
	//Upload
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, complete.m);
	glUniform1i(glGetUniformLocation(program, "color"), true);
	//Draw
	DrawModel(sphereModel, program, "inPosition", "inNormal", "inTexCoord");

	glutSwapBuffers();
}


void createSphere(){

	if( drawArrayElements < drawArraySize){
		drawObject tmp;
		tmp.m = LoadModelPlus("webtrcc.obj");
		tmp.texName = "rock_01_dif.tga";
		tmp.trans = IdentityMatrix();
		tmp.rot = IdentityMatrix();
		tmp.scale = IdentityMatrix();
		tmp.shaderprogram = program;
		tmp.objectTransform = IdentityMatrix();
		tmp.objectTransform = Mult(tmp.objectTransform, T((float)loops, 0.0f,10.0f));
		//calcSlope(tmp.objectTransform.m[3], tmp.objectTransform.m[11],
		LoadTGATextureSimple(tmp.texName, &tmp.texNum);
		vec3 midP = SetVector(tmp.objectTransform.m[3],tmp.objectTransform.m[7],tmp.objectTransform.m[11]);
		Collider tmpCol = makeSphereCollider(midP, 1.0f);
		tmp.col = tmpCol;
		if(__debug__ && !t){
			printf("Elements: %d, ArraySize: %d\n", drawArrayElements, drawArraySize);
		}
		if(drawArrayElements < drawArraySize){
			drawObjects[drawArrayElements++] = tmp;
			if(__debug__ && !t){
				printf("Added object\n");
			}
		} else {
			if(__debug__ && !t){
				printf("Size of drawArrayElements: %d\n", drawArrayElements);
				printf("Can't insert more objects!\n");
				printf("Size of drawObjects: %d, and drawObjects[0]: %d\n", (int) sizeof(drawObjects), (int) sizeof(drawObjects[0]));
			}
		}
	}
}

void positionCamera(){
	if(lockCamera == 1){
		camMatrix.m[3] = sphereTransform.m[3];
		camMatrix.m[7] = -sphereTransform.m[7] -2.0f;
		camMatrix.m[11] = sphereTransform.m[11] - 5.0f;
	}

}
void revertPlayerMovement(){
	int j;
	for(j=0; j<16; j++){
		sphereTransform.m[j] = tmpPlayerMat.m[j];
	}
}

void updateColliders(){
	vec3 newMidP = SetVector(sphereTransform.m[3],sphereTransform.m[7],sphereTransform.m[11]);
	playerCol.midPoint = newMidP;
	int i;
	while(i < drawArrayElements){
		drawObjects[i].col.midPoint.y = calcHeight(drawObjects[i].objectTransform.m[3], drawObjects[i].objectTransform.m[11], ttex.width, tm->vertexArray);
		i++;
	}
	i=0;
}

void checkPlayerCollision(){
	int i = 0;
	while(i < drawArrayElements){
		bool hit = checkCollision(playerCol, drawObjects[i].col);
		if(hit){
			printf("HIT!\n");
			revertPlayerMovement();
		}
		i++;
	}
	i= 0;
}

void timer(int i)
{
	glutTimerFunc(20, &timer, i);
	checkInput(&t, &sphereSpeed, &sphereTransform, &camMatrix, &lockCamera, &rotTest, &playerCol, drawObjects, &drawArrayElements, &tmpPlayerMat);
	positionCamera();
	updateColliders();
	checkPlayerCollision();
	if(t==0) loops++;
	if(loops % 10 == 0 && t==0){
		createSphere();
	}
	glutPostRedisplay();
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (1024, 768);
	glutCreateWindow ("TSBK07 Project");
	glutDisplayFunc(display);
	printf("Loading...\n");
	init (&sphereModel, &skyBox, &tm, &skyBoxTransform, &camMatrix, &projectionMatrix, &sphereTransform, &texGrass, &texSphere, &texTerrain, &texLake, &texMountain, &skyboxTex, &skyboxprogram, &program, &ttex, &sphereSpeed, &drawObjects, &drawArrayElements, &drawArraySize, &playerCol);
	printf("Load complete\n");
	glutTimerFunc(20, &timer, 0);
	glutPassiveMotionFunc(mouse);
	glutMainLoop();
	exit(0);
}
