#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include <stdlib.h>


#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

#include "init.h"
#include "projectio.h"
#define __debug__ 0
#define __frustum_print__ 0

#define near 0.2
#define far 100.0
#define left -0.1
#define right 0.1
#define top 0.1
#define bot -0.1

drawObject *drawObjects;
int drawArrayElements;
int drawArraySize;

float sphereSpeed;

int lockCamera = 0;
int loops = 0;

// vertex array object
Model *sphereModel, *tm, *bulletModel;

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

//GAME LOGIC STUFF
Collider playerCol;
mat4 tmpPlayerMat;
bool shoot, shotAlive, cooldown = false;
int cooldownTicks = 50;
int enemyRespawnCount = 0;
bool playerAlive = true;
int winCon = 5;
int kills = 0;
bool win = false;


//BULLET STUFF
bullet *bullets;
int bulletsArrayElements;
int bulletsArraySize;
int bulletRespawnCount = 0;


//Frustum points
vec3 farTopLeft, farTopRight, farBotLeft, farBotRight;
typedef struct plane {
	float a,b,c,d;
} plane;
plane frustumPlane[4];


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

int inFrustum(mat4 objectTransform){
	//printf("Checking frustum..\n");
	vec3 point = SetVector(objectTransform.m[3],objectTransform.m[7],objectTransform.m[11]);
	int i = 0;
	for(i = 0; i<4;i++){
		float distance = point.x*frustumPlane[i].a + point.y*frustumPlane[i].b + point.z*frustumPlane[i].c;
		//printf("Plane %i: a=%f b=%f c=%f d=%f\n",i,frustumPlane[i].a,frustumPlane[i].b,frustumPlane[i].c,frustumPlane[i].d);
		//printf("Distance: %f\n", distance);
		if(frustumPlane[i].d > distance){
			return 0;
		}
	}
	return 1;
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
	glUniform1i(glGetUniformLocation(program, "bullet"), false);

	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

	printError("display 2");

	//Draw drawObjects
	int i = 0;
	while(i < drawArrayElements){
		if(inFrustum(drawObjects[i].objectTransform)){
			if(__frustum_print__){
				printf("Drawing item %d.\n", i);
			}
			GLfloat objectHeight = calcHeight(drawObjects[i].objectTransform.m[3], drawObjects[i].objectTransform.m[11], ttex.width, tm->vertexArray);
			drawObjects[i].objectTransform.m[7] = objectHeight;
			drawObjects[i].col.midPoint.y = objectHeight;
			mat4 model = Mult(drawObjects[i].trans,drawObjects[i].scale);
			model = Mult(drawObjects[i].rot, model);
			mat4 modelToWorld = Mult(model, drawObjects[i].objectTransform);
			mat4 modelToView = Mult(camMatrix, modelToWorld);
			glUniformMatrix4fv(glGetUniformLocation(drawObjects[i].shaderprogram, "mdlMatrix"), 1, GL_TRUE, modelToView.m);
			glUniform1i(glGetUniformLocation(drawObjects[i].shaderprogram, "color"), true);
			glUniform1i(glGetUniformLocation(drawObjects[i].shaderprogram, "bullet"), false);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, drawObjects[i].texNum);
			glUniform1i(glGetUniformLocation(program, "texSphere"), 4); // Texture unit 4
			DrawModel(drawObjects[i].m, drawObjects[i].shaderprogram, "inPosition", "inNormal", "inTexCoord");
			if(i==1){
				//printf("x: %F y: %f z: %f\n", drawObjects[0].objectTransform.m[3], drawObjects[0].objectTransform.m[7], drawObjects[0].objectTransform.m[11]);
			}
			
		}
		i++;
	}
	i = 0;
	/*
	while(i < 5){
		//printf("Drawing item %d.\n", i);
		mat4 model = Mult(drawObjects[i].trans,drawObjects[i].scale);
		model = Mult(drawObjects[i].rot, model);
		mat4 modelToWorld = Mult(model, drawObjects[i].objectTransform);
		mat4 modelToView = Mult(camMatrix, modelToWorld);
		glUniformMatrix4fv(glGetUniformLocation(drawObjects[i].shaderprogram, "mdlMatrix"), 1, GL_TRUE, modelToView.m);
		glUniform1i(glGetUniformLocation(drawObjects[i].shaderprogram, "color"), true);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, drawObjects[i].texNum);
		glUniform1i(glGetUniformLocation(program, "texSphere"), 4); // Texture unit 4
		DrawModel(drawObjects[i].m, drawObjects[i].shaderprogram, "inPosition", "inNormal", "inTexCoord");
		i++;	
	}
	i = 0;
	*/


	//Draw bullets
	int j = 0;
	while(j < bulletsArrayElements){
		GLfloat bulletHeight = calcHeight(bullets[j].bulletTransform.m[3], bullets[j].bulletTransform.m[11], ttex.width, tm->vertexArray);
		bullets[j].bulletTransform.m[7] = bulletHeight + 0.2f;
		//bullets[j].bulletTransform.m[7] = bulletHeight;
		bullets[j].col.midPoint.y = bulletHeight;
		mat4 model = Mult(bullets[j].trans,bullets[j].scale);
		model = Mult(bullets[j].rot, model);
		mat4 modelToWorld = Mult(bullets[j].bulletTransform, model);
		mat4 modelToView = Mult(camMatrix, modelToWorld);
		glUniformMatrix4fv(glGetUniformLocation(bullets[j].shaderprogram, "mdlMatrix"), 1, GL_TRUE, modelToView.m);
		glUniform1i(glGetUniformLocation(bullets[j].shaderprogram, "color"), true);
		glUniform1i(glGetUniformLocation(bullets[j].shaderprogram, "bullet"), true);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, bullets[j].texNum);
		glUniform1i(glGetUniformLocation(program, "texSphere"), 4); // Texture unit 4
		//printf("Drawing item %d.\n", i);
		DrawModel(bullets[j].m, bullets[j].shaderprogram, "inPosition", "inNormal", "inTexCoord");
		if(j==1){
			//printf("x: %F y: %f z: %f\n", bullets[0].bulletTransform.m[3], bullets[0].bulletTransform.m[7], bullets[0].bulletTransform.m[11]);
		}
		j++;
	}
	j = 0;

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
	glUniform1i(glGetUniformLocation(program, "bullet"), false);
	//Draw
	DrawModel(sphereModel, program, "inPosition", "inNormal", "inTexCoord");

	glutSwapBuffers();
}


void createSphere(){
	int randomX = rand() % ((int)sphereTransform.m[3]+20)*100 + (abs((int)sphereTransform.m[3]-20)*100);
	int randomZ = rand() % ((int)sphereTransform.m[11]+20)*100 + (abs((int)sphereTransform.m[11]-20)*100);
	GLfloat randX = (GLfloat) randomX/100.0f;
	GLfloat randZ = (GLfloat) randomZ/100.0f;
	printf("X: %f Z: %f\n",randX,randZ);
	if( drawArrayElements < drawArraySize){
		drawObject tmp;
		tmp.m = LoadModelPlus("webtrcc.obj");
		tmp.texName = "rock_01_dif.tga";
		tmp.trans = IdentityMatrix();
		tmp.rot = IdentityMatrix();
		tmp.scale = IdentityMatrix();
		tmp.shaderprogram = program;
		tmp.objectTransform = IdentityMatrix();
		tmp.objectTransform = Mult(tmp.objectTransform, T(randX, 0.0f,randZ));
		//calcSlope(tmp.objectTransform.m[3], tmp.objectTransform.m[11],
		LoadTGATextureSimple(tmp.texName, &tmp.texNum);
		vec3 midP = SetVector(tmp.objectTransform.m[3],tmp.objectTransform.m[7],tmp.objectTransform.m[11]);
		Collider tmpCol = makeSphereCollider(midP, 1.0f);
		tmp.col = tmpCol;
		tmp.alive = true;
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
	else{
		int i = 0;
		while(i < drawArrayElements){
			if(!drawObjects[i].alive){
				drawObjects[i].alive = true;
				drawObjects[i].objectTransform = Mult(IdentityMatrix(), T(randX, 0.0f,randZ));
				//updateDrawObjCollider(drawObjects[i]);
				break;
			}
			i++;
		}
		i=0;
	}
}

void createBullet(){
	GLfloat bulletSpawnOffset = 1.0f;
	GLfloat xRot= sphereModelMat.m[2];
	GLfloat zRot = sphereModelMat.m[0];
	if( bulletsArrayElements < bulletsArraySize){
		bullet tmp;
		tmp.m = LoadModelPlus("webtrcc.obj");
		tmp.texName = "rock_01_dif.tga";
		tmp.trans = IdentityMatrix();
		tmp.scale = S(0.3f, 0.3f, 0.3f);//IdentityMatrix();
		tmp.rot = IdentityMatrix();
		tmp.shaderprogram = program;
		tmp.bulletTransform = IdentityMatrix();
		tmp.bulletTransform = Mult(tmp.bulletTransform, T((sphereTransform.m[3] + xRot*bulletSpawnOffset), sphereTransform.m[7] ,(sphereTransform.m[11] + zRot*bulletSpawnOffset)));
		tmp.bulletSpeed = 0.3f;
		tmp.moveVec = Normalize(SetVector(tmp.bulletTransform.m[3]-sphereTransform.m[3],tmp.bulletTransform.m[7]-sphereTransform.m[7],tmp.bulletTransform.m[11]-sphereTransform.m[11]));
		tmp.TTL = 75; //5 sek?
		tmp.alive = true;
		//calcSlope(tmp.bulletTransform.m[3], tmp.bulletTransform.m[11],
		LoadTGATextureSimple(tmp.texName, &tmp.texNum);
		vec3 midP = SetVector(tmp.bulletTransform.m[3],tmp.bulletTransform.m[7],tmp.bulletTransform.m[11]);
		Collider tmpCol = makeSphereCollider(midP, 0.5f);
		tmp.col = tmpCol;

		if(__debug__){
			printf("sphereModelMat\n 0: %f 2: %f\n",sphereModelMat.m[0],sphereModelMat.m[2] );
			printf("X: %f Z: %f\n",xRot,zRot);
			printf("X: %f, Y: %f, Z: %f\n", tmp.moveVec.x, tmp.moveVec.y, tmp.moveVec.z);
			printf("made a bullet!!!\n");
			printf("0: %f %f %f %f \n1: %f %f %f %f \n2: %f %f %f %f \n3: %f %f %f %f \n\n",tmp.bulletTransform.m[0],tmp.bulletTransform.m[1],tmp.bulletTransform.m[2],tmp.bulletTransform.m[3],tmp.bulletTransform.m[4],tmp.bulletTransform.m[5],tmp.bulletTransform.m[6],tmp.bulletTransform.m[7],tmp.bulletTransform.m[8],tmp.bulletTransform.m[9],tmp.bulletTransform.m[10],tmp.bulletTransform.m[11],tmp.bulletTransform.m[12],tmp.bulletTransform.m[13],tmp.bulletTransform.m[14],tmp.bulletTransform.m[15]);
		}

		if(__debug__ && !t){
			printf("Elements: %d, ArraySize: %d\n", drawArrayElements, drawArraySize);
		}

		if(bulletsArrayElements < bulletsArraySize){
			bullets[bulletsArrayElements++] = tmp;
			if(__debug__ && !t){
				printf("Added bullet\n");
			}
		} else {
			if(__debug__ && !t){
				printf("Size of bulletArrayElements: %d\n", drawArrayElements);
				printf("Can't insert more bullets!\n");
				printf("Size of bullets: %d, and bullets[0]: %d\n", (int) sizeof(bullets), (int) sizeof(bullets[0]));
			}
		}
	}
	else{
		int i = 0;
		while(i < bulletsArrayElements){
			if(!bullets[i].alive){
				bullets[i].alive = true;
				bullets[i].bulletTransform = Mult(IdentityMatrix(), T((sphereTransform.m[3] + xRot*bulletSpawnOffset), sphereTransform.m[7] ,(sphereTransform.m[11] + zRot*bulletSpawnOffset)));
				bullets[i].moveVec = Normalize(SetVector(bullets[i].bulletTransform.m[3]-sphereTransform.m[3],bullets[i].bulletTransform.m[7]-sphereTransform.m[7],bullets[i].bulletTransform.m[11]-sphereTransform.m[11]));
				bullets[i].TTL = 75; //5 sek?
				//updateDrawObjCollider(drawObjects[i]);
				break;
			}
			i++;
		}
		i=0;
	}
}

void positionCamera(){
	/*if(lockCamera == 1){
		camMatrix.m[3] = sphereTransform.m[3];
		camMatrix.m[7] = -sphereTransform.m[7] -2.0f;
		camMatrix.m[11] = sphereTransform.m[11] - 5.0f;
	}*/

}

void updatePlayerCollider(){
	vec3 newMidP = SetVector(sphereTransform.m[3],sphereTransform.m[7],sphereTransform.m[11]);
	playerCol.midPoint = newMidP;
}

void revertPlayerMovement(){
	int j;
	for(j=0; j<16; j++){
		sphereTransform.m[j] = tmpPlayerMat.m[j];
	}
	updatePlayerCollider();
}

void revertEnemyMovement(){

}

void moveBullet(){
	int i = 0;
	while(i < bulletsArrayElements){
		if(bullets[i]. alive){
			bullets[i].bulletTransform = Mult(bullets[i].bulletTransform, T(bullets[i].moveVec.x*bullets[i].bulletSpeed, 0, bullets[i].moveVec.z*bullets[i].bulletSpeed));
			bullets[i].TTL --;
			if(__debug__){
				printf("TTL: %d\n", bullets[i].TTL);
			}
		}
		if(bullets[i].TTL <= 0){
			bullets[i].alive = false;
			bullets[i].bulletTransform.m[3] = -100.0f;
			bullets[i].bulletTransform.m[11] = -100.0f;
		}
		i++;
	}
	i=0;
}

void updateDrawObjCollider(drawObject o){
	vec3 newMidP = SetVector(o.objectTransform.m[3],o.objectTransform.m[7],o.objectTransform.m[11]);
	o.col.midPoint = newMidP;
	o.col.midPoint.y = calcHeight(o.objectTransform.m[3], o.objectTransform.m[11], ttex.width, tm->vertexArray);
}

void moveEnemy(){ //moves enemy and checks enemy collision w other enemies and player
	GLfloat enemySpeed = 0.1f;
	int i = 0;
	while(i < drawArrayElements){
		if(drawObjects[i].alive){
			//save previous enemy coords
			vec3 prevTransform = SetVector(drawObjects[i].objectTransform.m[3],drawObjects[i].objectTransform.m[7],drawObjects[i].objectTransform.m[11]);

			//try moving the enemy towards the player
			vec3 enemyVec = Normalize(SetVector(sphereTransform.m[3] - drawObjects[i].objectTransform.m[3],0,sphereTransform.m[11] - drawObjects[i].objectTransform.m[11]));
			drawObjects[i].objectTransform = Mult(drawObjects[i].objectTransform, T(enemyVec.x*enemySpeed, 0 , enemyVec.z*enemySpeed));

			//update enemy collider
			drawObjects[i].col.midPoint = SetVector(drawObjects[i].objectTransform.m[3],drawObjects[i].objectTransform.m[7],drawObjects[i].objectTransform.m[11]);

			//check collisions with player
			bool hit = checkCollision(playerCol, drawObjects[i].col);
			if(hit){
				//revert enemy pos to prev coords
				if(__debug__)
					printf("HIT player!!!\n");
				drawObjects[i].objectTransform.m[3] = prevTransform.x;
				drawObjects[i].objectTransform.m[7] = prevTransform.y;
				drawObjects[i].objectTransform.m[11] = prevTransform.z;
				updateDrawObjCollider(drawObjects[i]);
			}
			//check collision with other enemies
			int j = 0;
			while(j < drawArrayElements){
				if(i != j){//dont check itself
					if(drawObjects[j].alive){
						hit = checkCollision(drawObjects[i].col, drawObjects[j].col);
						if(hit){
							//revert enemy pos to prev coords
							if(__debug__)
								printf("HIT enemy!!!\n");

							drawObjects[i].objectTransform.m[3] = prevTransform.x;
							drawObjects[i].objectTransform.m[7] = prevTransform.y;
							drawObjects[i].objectTransform.m[11] = prevTransform.z;
							updateDrawObjCollider(drawObjects[i]);
						}
					}
				}
					j++;
			}
			j=0;

		if(__debug__){
			printf("enemyVec: %f, %f, %f\n",enemyVec.x,enemyVec.y, enemyVec.z );
		}
	}
	i++;
}
i= 0;
}

void updateEnemyColliders(){ //NOT IN USE DELETE LATER
	int i = 0;
	while(i < drawArrayElements){
		if(drawObjects[i].alive){
			drawObjects[i].col.midPoint = SetVector(drawObjects[i].objectTransform.m[3],drawObjects[i].objectTransform.m[7],drawObjects[i].objectTransform.m[11]);
			drawObjects[i].col.midPoint.y = calcHeight(drawObjects[i].objectTransform.m[3], drawObjects[i].objectTransform.m[11], ttex.width, tm->vertexArray);
			if(__debug__){
				printf("Emeny midP\n X: %f Y: %f Z: %f\n",drawObjects[i].col.midPoint.x,drawObjects[i].col.midPoint.y,drawObjects[i].col.midPoint.z );
			}
		}
		i++;
	}
	i=0;

}
void updateBulletColliders(){
	int i = 0;
	while(i < bulletsArrayElements){
		if(bullets[i].alive){
			bullets[i].col.midPoint = SetVector(bullets[i].bulletTransform.m[3],bullets[i].bulletTransform.m[7],bullets[i].bulletTransform.m[11]);
			bullets[i].col.midPoint.y = calcHeight(bullets[i].bulletTransform.m[3], bullets[i].bulletTransform.m[11], ttex.width, tm->vertexArray);
			if(__debug__){
				printf("Bullet midP\n X: %f Y: %f Z: %f\n",bullets[i].col.midPoint.x,bullets[i].col.midPoint.y,bullets[i].col.midPoint.z );
			}
		}
		i++;
	}
	i=0;

}

void checkPlayerCollision(){
	int i = 0;
	while(i < drawArrayElements){
		if(drawObjects[i].alive){
			bool hit = checkCollision(playerCol, drawObjects[i].col);
			if(hit){
				if(__debug__)
					printf("HIT!\n");
				revertPlayerMovement();
				playerAlive = false;
				sphereTransform.m[3] = 0.0f;
				sphereTransform.m[11] = 0.0f;
			}
		}
		i++;
	}
	i= 0;
}

void checkBulletCollision(){
	int i = 0;
	while(i < bulletsArrayElements){
		if(bullets[i]. alive){
			int j = 0;
			while(j < drawArrayElements){
				if(drawObjects[i].alive){
					bool hit = checkCollision(bullets[i].col, drawObjects[j].col);
					if(hit){
						if(__debug__)
							printf("HIT!\n");
						bullets[i].alive = false;
						bullets[i].TTL = 0;
						bullets[i].bulletTransform.m[3] = -100.0f;
						bullets[i].bulletTransform.m[11] = -100.0f;

						drawObjects[j].alive = false;
						drawObjects[j].objectTransform.m[3] = -100.0f;
						drawObjects[j].objectTransform.m[11] = -100.0f;

						kills++;
						//createSphere();
						//createSphere();
					}
				}
				j++;
			}
			j= 0;
		}
		i++;
	}
	i=0;
}

void checkShoot(){
	//cooldown = false;
	if(shoot && !cooldown){
		shoot = false;
		if(__debug__)
			printf("SHOOT!\n");
		createBullet();

		cooldown = true;
		cooldownTicks = 50;
	}
	else{
		if(cooldownTicks == 0){
			shoot = false;
			cooldown = false;
		}
		cooldownTicks--;
	}
}

int equal(mat4 a, mat4 b){
	int i;
	for(i = 0; i < 16; i++){
		if(a.m[i] != b.m[i]){
			return 0;
		}
	}
	return 1;
}

void timer(int i)
{
	/*if(bulletRespawnCount == 0){
		createSphere();
		bulletRespawnCount++;
	}*/
	glutTimerFunc(20, &timer, i);
	if(!win){
		mat4 tmpCamera = camMatrix;
		checkInput(&t, &sphereSpeed, &sphereTransform, &camMatrix, &lockCamera, &rotTest, &playerCol, drawObjects, &drawArrayElements, &tmpPlayerMat, &shoot, &playerAlive);
		if (!equal(tmpCamera,camMatrix)){
		//Recalc frustum
		farTopLeft = SetVector((far/near)*left,(far/near)*top,-far);
		farTopRight = SetVector((far/near)*right,(far/near)*top,-far);
		farBotLeft = SetVector((far/near)*left,(far/near)*bot,-far);
		farBotRight = SetVector((far/near)*right,(far/near)*bot,-far);
		vec3 camPos = SetVector(0,0,0);

		/*
		printf("\nTop Left;\nx: %f, y: %f, z:%f\n",farTopLeft.x, farTopLeft.y,farTopLeft.z);
		printf("Top Right;\nx: %f, y: %f, z:%f\n",farTopRight.x, farTopRight.y,farTopRight.z);
		printf("Bot Left;\nx: %f, y: %f, z:%f\n",farBotLeft.x, farBotLeft.y,farBotLeft.z);
		printf("Bot Right;\nx: %f, y: %f, z:%f\n",farBotRight.x, farBotRight.y,farBotRight.z);
		*/

		//Invert camMatrix
		mat4 invCamMatrix = InvertMat4(camMatrix);
		
		farTopLeft = MultVec3(invCamMatrix,farTopLeft);
		farTopRight = MultVec3(invCamMatrix,farTopRight);
		farBotLeft = MultVec3(invCamMatrix,farBotLeft);
		farBotRight = MultVec3(invCamMatrix,farBotRight);
		camPos = MultVec3(invCamMatrix, camPos);
		
		
		if(lockCamera){
		
		drawObjects[0].objectTransform.m[3] = farTopLeft.x;
		drawObjects[0].objectTransform.m[7] = farTopLeft.y;
		drawObjects[0].objectTransform.m[11] = farTopLeft.z;
		drawObjects[1].objectTransform.m[3] = farTopRight.x;
		drawObjects[1].objectTransform.m[7] = farTopRight.y;
		drawObjects[1].objectTransform.m[11] = farTopRight.z;
		drawObjects[2].objectTransform.m[3] = farBotLeft.x;
		drawObjects[2].objectTransform.m[7] = farBotLeft.y;
		drawObjects[2].objectTransform.m[11] = farBotLeft.z;
		drawObjects[3].objectTransform.m[3] = farBotRight.x;
		drawObjects[3].objectTransform.m[7] = farBotRight.y;
		drawObjects[3].objectTransform.m[11] = farBotRight.z;
		drawObjects[4].objectTransform.m[3] = camPos.x;
		drawObjects[4].objectTransform.m[7] = camPos.y;
		drawObjects[4].objectTransform.m[11] = camPos.z;
		
		}
		/*
		printf("\n\nTop Left;\nx: %f, y: %f, z:%f\n",drawObjects[0].objectTransform.m[3], drawObjects[0].objectTransform.m[7],drawObjects[0].objectTransform.m[11]);
		printf("Top Right;\nx: %f, y: %f, z:%f\n",drawObjects[1].objectTransform.m[3], drawObjects[1].objectTransform.m[7],drawObjects[1].objectTransform.m[11]);
		printf("Bot Left;\nx: %f, y: %f, z:%f\n",drawObjects[2].objectTransform.m[3], drawObjects[2].objectTransform.m[7],drawObjects[2].objectTransform.m[11]);
		printf("Bot Right;\nx: %f, y: %f, z:%f\n",drawObjects[3].objectTransform.m[3], drawObjects[3].objectTransform.m[7],drawObjects[3].objectTransform.m[11]);
		printf("Cam;\nx: %f, y: %f, z:%f\n",drawObjects[4].objectTransform.m[3], drawObjects[4].objectTransform.m[7],drawObjects[4].objectTransform.m[11]);
		*/
		/*
		printf("\n\nTop Left;\nx: %f, y: %f, z:%f\n",farTopLeft.x, farTopLeft.y,farTopLeft.z);
		printf("Top Right;\nx: %f, y: %f, z:%f\n",farTopRight.x, farTopRight.y,farTopRight.z);
		printf("Bot Left;\nx: %f, y: %f, z:%f\n",farBotLeft.x, farBotLeft.y,farBotLeft.z);
		printf("Bot Right;\nx: %f, y: %f, z:%f\n",farBotRight.x, farBotRight.y,farBotRight.z);
		*/

		//Frustum Planes
		vec3 tmp = Normalize(CrossProduct(VectorSub(farBotLeft,camPos),VectorSub(farTopLeft,camPos))); //Left
		frustumPlane[0].a = tmp.x;
		frustumPlane[0].b = tmp.y;
		frustumPlane[0].c = tmp.z;
		frustumPlane[0].d = camPos.x*tmp.x + camPos.y*tmp.y + camPos.z*tmp.z;

		tmp = Normalize(CrossProduct(VectorSub(farTopRight,camPos), VectorSub(farBotRight, camPos))); //Right
		frustumPlane[1].a = tmp.x;
		frustumPlane[1].b = tmp.y;
		frustumPlane[1].c = tmp.z;
		frustumPlane[1].d = camPos.x*tmp.x + camPos.y*tmp.y + camPos.z*tmp.z;

		tmp = Normalize(CrossProduct(VectorSub(farTopLeft,camPos), VectorSub(farTopRight,camPos))); //Top
		frustumPlane[2].a = tmp.x;
		frustumPlane[2].b = tmp.y;
		frustumPlane[2].c = tmp.z;
		frustumPlane[2].d = camPos.x*tmp.x + camPos.y*tmp.y + camPos.z*tmp.z;

		tmp = Normalize(CrossProduct(VectorSub(farBotRight,camPos), VectorSub(farBotLeft,camPos))); //Bottom
		frustumPlane[3].a = tmp.x;
		frustumPlane[3].b = tmp.y;
		frustumPlane[3].c = tmp.z;
		frustumPlane[3].d = camPos.x*tmp.x + camPos.y*tmp.y + camPos.z*tmp.z;
	}
		if(playerAlive){
			updatePlayerCollider();
			positionCamera();
			moveBullet();
			checkShoot();
			updateBulletColliders();
			moveEnemy();
			checkPlayerCollision();
			checkBulletCollision();


		if(t==0) loops++;
		if(loops % 10 == 0 && t==0){
			createSphere();
			}
		}
		if(kills == winCon){
			win = true;
		}
	}
	glutPostRedisplay();
}



int main(int argc, char **argv)
{
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (1024, 768);
	glutCreateWindow ("TSBK07 Project");
	glutDisplayFunc(display);
	printf("Loading...\n");

	init (&sphereModel, &skyBox, &tm, &skyBoxTransform, &camMatrix, &projectionMatrix, &sphereTransform, &texGrass, &texSphere, &texTerrain, &texLake, &texMountain, &skyboxTex, &skyboxprogram, &program, &ttex, &sphereSpeed, &drawObjects, &drawArrayElements, &drawArraySize, &playerCol, &farTopLeft, &farTopRight, &farBotLeft, &farBotRight,&bullets, &bulletsArrayElements, &bulletsArraySize);
	printf("Load complete\n");
	/*
	createSphere();
	createSphere();
	createSphere();
	createSphere();
	createSphere();
	LoadTGATextureSimple("dried_grass.tga", &drawObjects[0].texNum);
	LoadTGATextureSimple("Sea_Caustic.tga", &drawObjects[1].texNum);
	LoadTGATextureSimple("rock_02_nm_inv.tga", &drawObjects[2].texNum);
	LoadTGATextureSimple("rock_01_rough.tga", &drawObjects[3].texNum);
	LoadTGATextureSimple("SkyBox512.tga", &drawObjects[4].texNum);
	*/
	glutTimerFunc(20, &timer, 0);
	glutPassiveMotionFunc(mouse);
	glutMainLoop();
	exit(0);
}
