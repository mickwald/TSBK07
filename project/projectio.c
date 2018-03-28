#include "MicroGlut.h"
#include "VectorUtils3.h"

void checkInput(){
	if(glutKeyIsDown('w')){
		camMatrix = Mult(camMatrix, T(-0.4f*camMatrix.m[2],0,0.4f*camMatrix.m[0]));
	}
	if(glutKeyIsDown('s')){
		camMatrix = Mult(camMatrix, T(0.4f*camMatrix.m[2],0,-0.4f*camMatrix.m[0]));
	}
	if(glutKeyIsDown('a')){
		camMatrix = Mult(camMatrix, T(0.4f*camMatrix.m[0],0,0.4f*camMatrix.m[2]));
	}
	if(glutKeyIsDown('d')){
		camMatrix = Mult(camMatrix, T(-0.4f*camMatrix.m[0],0,-0.4f*camMatrix.m[2]));
	}
	if(glutKeyIsDown('i')){
		camMatrix = Mult(camMatrix, T(0,-0.2f,0));
	}
	if(glutKeyIsDown('k')){
		camMatrix = Mult(camMatrix, T(0,0.2f,0));
	}
	if(glutKeyIsDown('j')){
		camMatrix = Mult(Ry(-1*M_PI/180),camMatrix );
	}
	if(glutKeyIsDown('l')){
		camMatrix = Mult(Ry(1*M_PI/180),camMatrix );
	}
	if(glutKeyIsDown(GLUT_KEY_DOWN)){
		sphereTransform = Mult(sphereTransform, T(-sphereSpeed*camMatrix.m[2],0,sphereSpeed*camMatrix.m[0]));
	}
	if(glutKeyIsDown(GLUT_KEY_UP)){
		sphereTransform = Mult(sphereTransform, T(sphereSpeed*camMatrix.m[2],0,-sphereSpeed*camMatrix.m[0]));
	}
	if(glutKeyIsDown(GLUT_KEY_RIGHT)){
		sphereTransform = Mult(sphereTransform, T(sphereSpeed*camMatrix.m[0],0,sphereSpeed*camMatrix.m[2]));
	}
	if(glutKeyIsDown(GLUT_KEY_LEFT)){
		sphereTransform = Mult(sphereTransform, T(-sphereSpeed*camMatrix.m[0],0,-sphereSpeed*camMatrix.m[2]));
	}
	if(glutKeyIsDown('+')){
		if(t == 0){
		sphereSpeed += 0.01f;
		printf("Speed: %f\n", sphereSpeed);
		}
	}
	if(glutKeyIsDown('-')){
		if(t== 0){
		sphereSpeed -= 0.01f;
		printf("Speed: %f\n", sphereSpeed);
		}
	}
	if(glutKeyIsDown('r')){
		camMatrix = IdentityMatrix();
		camMatrix = Mult(Ry(135*M_PI/180.0f),camMatrix);
	}
	if(glutKeyIsDown('m')){
		printf("0: %f %f %f %f \n1: %f %f %f %f \n2: %f %f %f %f \n3: %f %f %f %f \n\n",camMatrix.m[0],camMatrix.m[1],camMatrix.m[2],camMatrix.m[3],camMatrix.m[4],camMatrix.m[5],camMatrix.m[6],camMatrix.m[7],camMatrix.m[8],camMatrix.m[9],camMatrix.m[10],camMatrix.m[11],camMatrix.m[12],camMatrix.m[13],camMatrix.m[14],camMatrix.m[15]);
	}
	if(sphereTransform.m[11] < 0){
		sphereTransform.m[11] = 0;
	}
	if(sphereTransform.m[3] < 0){
		sphereTransform.m[3] = 0;
	}
	if(t++ <= 10){
		if(t == 10){
			//printf("X: %f, Y: %f, Z: %f\n", camMatrix.m[3], camMatrix.m[7], camMatrix.m[11]);
			t = 0;
		}
	}
	//camMatrix.m[7] = -cam_height + calcHeight(camMatrix.m[3], camMatrix.m[11], ttex.width, tm->vertexArray);
}