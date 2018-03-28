#include "MicroGlut.h"

void init(void)
{
	// GL inits
	glClearColor(0.8,0.8,0.8,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	m = LoadModelPlus("webtrcc.obj");
	skyBox = LoadModelPlus("skybox.obj");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 100.0);

	// Load and compile shader
	program = loadShaders("world.vert", "world.frag");
	skyboxprogram = loadShaders("skybox.vert", "skybox.frag");
	glUseProgram(program);
	printError("init shader");

	sphereTransform = IdentityMatrix();
	sphereSpeed = 0.1f;

	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	glUseProgram(skyboxprogram);
	glUniformMatrix4fv(glGetUniformLocation(skyboxprogram, "uniProjection"), 1, GL_TRUE, projectionMatrix.m);

	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "texGrass"), 0); // Texture unit 0
	LoadTGATextureSimple("Grass_tile_B_diffuse.tga", &texGrass);
	LoadTGATextureSimple("SkyBox512.tga", &skyboxTex);

	vec3 cam = {0, 0, 0};
	vec3 lookAtPoint = {1, 0, 0};
	camMatrix = lookAt(cam.x, cam.y, cam.z,
				lookAtPoint.x, lookAtPoint.y, lookAtPoint.z,
				0.0, 1.0, 0.0);
// Load terrain data


	LoadTGATextureData("fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	//If done above Texture data load 0 generate terrain, causes graphical errors in edges ???
	LoadTGATextureSimple("rock_02_dif.tga", &texMountain);
	LoadTGATextureSimple("rock_02_dif.tga", &texLake);
	LoadTGATextureSimple("fft-terrain.tga", &texTerrain);
	LoadTGATextureSimple("rock_01_dif.tga", &texSphere);

	printError("init terrain");

	uploadLights();

}