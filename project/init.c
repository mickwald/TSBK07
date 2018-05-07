#include "MicroGlut.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include "collision.h"


int initialArraySize = 3;

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

//Light definitions
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


//Assistant function for uploading lights
void uploadLights(GLuint *program, mat4 *camMatrix){
	glUseProgram(*program);
	int i;
	for(i = 0; i <=3; i++){
	if(!isDirectional[i]){
	lightSourcesDirectionsPositions[i] = MultVec3(*camMatrix, lightSourcesDirectionsPositionsStiff[i]);
		}
	}
	glUniform3fv(glGetUniformLocation(*program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);

	glUniform3fv(glGetUniformLocation(*program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1fv(glGetUniformLocation(*program, "specularExponent"), 4, specularExponent);
	glUniform1iv(glGetUniformLocation(*program, "isDirectional"), 4, isDirectional);
}


//Calculate Normal helper func
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



//GenerateTerrain Helper function
Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	int x, z;
	printf("X: %d, Y: %d\n", vertexCount, triangleCount);

	GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount*3);

	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
			// Vertex array.
			vertexArray[(x + z * tex->width)*3 + 0] = x / 1.0;
			vertexArray[(x + z * tex->width)*3 + 1] = 4*tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 100.0;
			vertexArray[(x + z * tex->width)*3 + 2] = z / 1.0;
			// Normal vectors.
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
			// Texture coordinates.
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




//INIT Starts here
void init(Model **sphereModel, Model **skyBox, Model **tm, mat4 *skyBoxTransform, mat4 *camMatrix, mat4 *projectionMatrix, mat4 *sphereTransform, GLuint *texGrass, GLuint *texSphere, GLuint *texTerrain, GLuint *texLake, GLuint *texMountain, GLuint *skyboxTex, GLuint *skyboxprogram, GLuint *program, TextureData *ttex, float *sphereSpeed, drawObject **drawObjects, int *drawObjectsArrayElements, int *drawObjectsArraySize, Collider *playerCol)
{

	printError("init start");
	// GL inits
	glClearColor(0.8,0.8,0.8,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");
	*sphereModel = LoadModelPlus("webtrcc.obj");
	*skyBox = LoadModelPlus("skybox.obj");

	vec3 playerMidP = SetVector(sphereTransform->m[3],sphereTransform->m[7],sphereTransform->m[11]);
	*playerCol = makeSphereCollider(playerMidP, 1.0f);


	*projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 100.0);

	//drawObjectsArray init
	*drawObjects = (struct drawObject*) malloc(sizeof(drawObject)*initialArraySize);
	*drawObjectsArrayElements = 0;
	*drawObjectsArraySize = initialArraySize;


	// Load and compile shader
	*program = loadShaders("world.vert", "world.frag");
	*skyboxprogram = loadShaders("skybox.vert", "skybox.frag");
	glUseProgram(*program);
	printError("init shader");

	*sphereTransform = IdentityMatrix();
	*sphereSpeed = 0.5f;

	glUniformMatrix4fv(glGetUniformLocation(*program, "projMatrix"), 1, GL_TRUE, projectionMatrix->m);

	glUseProgram(*skyboxprogram);
	glUniformMatrix4fv(glGetUniformLocation(*skyboxprogram, "uniProjection"), 1, GL_TRUE, projectionMatrix->m);

	glUseProgram(*program);
	glUniform1i(glGetUniformLocation(*program, "texGrass"), 0); // Texture unit 0
	LoadTGATextureSimple("Grass_tile_B_diffuse.tga", texGrass);
	LoadTGATextureSimple("SkyBox512.tga", skyboxTex);

	vec3 cam = {0, 0, 0};
	vec3 lookAtPoint = {1, 0, 0};
	*camMatrix = lookAt(cam.x, cam.y, cam.z,
				lookAtPoint.x, lookAtPoint.y, lookAtPoint.z,
				0.0, 1.0, 0.0);
// Load terrain data

	//set skyboxMatrix
	*skyBoxTransform = *camMatrix;
	skyBoxTransform->m[3] = 0;
	skyBoxTransform->m[7] = 0;
	skyBoxTransform->m[11] = 0;

	LoadTGATextureData("fft-terrain.tga", ttex);
	*tm = GenerateTerrain(ttex);
	//If done above Texture data load 0 generate terrain, causes graphical errors in edges ???
	LoadTGATextureSimple("rock_02_dif.tga", texMountain);
	LoadTGATextureSimple("rock_02_dif.tga", texLake);
	LoadTGATextureSimple("fft-terrain.tga", texTerrain);
	LoadTGATextureSimple("rock_01_dif.tga", texSphere);

	printError("init terrain");

	uploadLights(program, camMatrix);

}
