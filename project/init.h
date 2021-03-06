#ifndef _INIT_
#define _INIT_

#include "loadobj.h"
#include "collision.h"

typedef struct drawObject {
	Model *m;
	char *texName;
	GLuint texNum;
	mat4 trans;
	mat4 rot;
	mat4 scale;
	GLuint shaderprogram;
	mat4 objectTransform;
	Collider col;
	bool alive;
} drawObject;

typedef struct bullet {
	Model *m;
	char *texName;
	GLuint texNum;
	mat4 trans;
	mat4 rot;
	mat4 scale;
	GLuint shaderprogram;
	mat4 bulletTransform;
	GLfloat bulletSpeed;
	int TTL;
	vec3 moveVec;
	Collider col;
	bool alive;
} bullet;


void init(Model **sphereModel, Model **skyBox, Model **tm, mat4 *skyBoxTransform, mat4 *camMatrix, mat4 *projectionMatrix, mat4 *sphereTransform, GLuint *texGrass, GLuint *texSphere, GLuint *texTerrain, GLuint *texLake, GLuint *texMountain, GLuint *skyboxTex, GLuint *skyboxprogram, GLuint *program, TextureData *ttex, float *sphereSpeed, drawObject **drawObjects, int *drawObjectsArrayElements, int *drawObjectsArraySize, Collider *playerCol, vec3 *farTopLeft, vec3 *farTopRight, vec3 *farBotLeft, vec3 *farBotRight, bullet **bullets, int *bulletsArrayElements, int *bulletsArraySize);

#endif
