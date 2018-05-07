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
} drawObject;

void init(Model **sphereModel, Model **skyBox, Model **tm, mat4 *skyBoxTransform, mat4 *camMatrix, mat4 *projectionMatrix, mat4 *sphereTransform, GLuint *texGrass, GLuint *texSphere, GLuint *texTerrain, GLuint *texLake, GLuint *texMountain, GLuint *skyboxTex, GLuint *skyboxprogram, GLuint *program, TextureData *ttex, float *sphereSpeed, drawObject **drawObjects, int *drawObjectsArrayElements, int *drawObjectsArraySize);

