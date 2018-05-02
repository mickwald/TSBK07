#include "MicroGlut.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"

int prevX = 0;
int prevY = 0;
float dX = 0;
float dY = 0;
int down = 0;
int mouseMoved = 0;
void mouse(int x, int y){
	dX = x - prevX;
	dY = y - prevY;

	prevX = x;
	prevY = y;
	//printf("X: %d PrevX: %d dX: %f Y: %d PrevY: %d dY: %f\n", x, prevX, dX, y, prevY, dY);
	//printf("dX: %d dY: %d\n", dX, dY);
}
void mouseDown(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		down = 1;
	}
	if(state == GLUT_UP){
		down = 0;
	}
}


void checkInput(int *t, float *sphereSpeed, mat4 *sphereTransform, mat4 *camMatrix){

	//Check mouse input
	glutMouseFunc(mouseDown);

	if(down == 1){

		if((*t%2) == 0){
			//printf("X: %d PrevX: %d dX: %d Y: %d PrevY: %d dY: %d\n", x, prevX, dX, y, prevY, dY);
			//printf("dX: %f dY: %f\n", dX, dY);
		}

		if(dX > 0){ // move camera right
			*camMatrix = Mult(Ry(dX*M_PI/360),*camMatrix);
		}
		else if(dX < 0){ //move camera left
			*camMatrix = Mult(Ry(dX*M_PI/360),*camMatrix);
		}

		dX = 0;
		dY = 0;
	}

	//Check keyboard input
	if(glutKeyIsDown('w')){
		*camMatrix = Mult(*camMatrix, T(-0.4f*camMatrix->m[2],0,0.4f*camMatrix->m[0]));
	}
	if(glutKeyIsDown('s')){
		*camMatrix = Mult(*camMatrix, T(0.4f*camMatrix->m[2],0,-0.4f*camMatrix->m[0]));
	}
	if(glutKeyIsDown('a')){
		*camMatrix = Mult(*camMatrix, T(0.4f*camMatrix->m[0],0,0.4f*camMatrix->m[2]));
	}
	if(glutKeyIsDown('d')){
		*camMatrix = Mult(*camMatrix, T(-0.4f*camMatrix->m[0],0,-0.4f*camMatrix->m[2]));
	}
	if(glutKeyIsDown('i')){
		*camMatrix = Mult(*camMatrix, T(0,-0.2f,0));
	}
	if(glutKeyIsDown('k')){
		*camMatrix = Mult(*camMatrix, T(0,0.2f,0));
	}
	if(glutKeyIsDown('j')){
		*camMatrix = Mult(Ry(-1*M_PI/180),*camMatrix );
	}
	if(glutKeyIsDown('l')){
		*camMatrix = Mult(Ry(1*M_PI/180),*camMatrix );
	}
	if(glutKeyIsDown(GLUT_KEY_DOWN)){
		*sphereTransform = Mult(*sphereTransform, T(-*sphereSpeed*camMatrix->m[2],0,*sphereSpeed*camMatrix->m[0]));
	}
	if(glutKeyIsDown(GLUT_KEY_UP)){
		*sphereTransform = Mult(*sphereTransform, T(*sphereSpeed*camMatrix->m[2],0,-*sphereSpeed*camMatrix->m[0]));
	}
	if(glutKeyIsDown(GLUT_KEY_RIGHT)){
		*sphereTransform = Mult(*sphereTransform, T(*sphereSpeed*camMatrix->m[0],0,*sphereSpeed*camMatrix->m[2]));
	}
	if(glutKeyIsDown(GLUT_KEY_LEFT)){
		*sphereTransform = Mult(*sphereTransform, T(-*sphereSpeed*camMatrix->m[0],0,-*sphereSpeed*camMatrix->m[2]));
	}
	if(glutKeyIsDown('+')){
		if(*t == 0){
		*sphereSpeed += 0.01f;
		printf("Speed: %f\n", *sphereSpeed);
		}
	}
	if(glutKeyIsDown('-')){
		if(*t== 0){
		*sphereSpeed -= 0.01f;
		printf("Speed: %f\n", *sphereSpeed);
		}
	}
	if(glutKeyIsDown('r')){
		*camMatrix = IdentityMatrix();
		*camMatrix = Mult(Ry(135*M_PI/180.0f),*camMatrix);
	}
	if(glutKeyIsDown('m')){
		printf("0: %f %f %f %f \n1: %f %f %f %f \n2: %f %f %f %f \n3: %f %f %f %f \n\n",camMatrix->m[0],camMatrix->m[1],camMatrix->m[2],camMatrix->m[3],camMatrix->m[4],camMatrix->m[5],camMatrix->m[6],camMatrix->m[7],camMatrix->m[8],camMatrix->m[9],camMatrix->m[10],camMatrix->m[11],camMatrix->m[12],camMatrix->m[13],camMatrix->m[14],camMatrix->m[15]);
	}
	if(glutKeyIsDown('n')){
		printf("0: %f %f %f %f \n1: %f %f %f %f \n2: %f %f %f %f \n3: %f %f %f %f \n\n",sphereTransform->m[0],sphereTransform->m[1],sphereTransform->m[2],sphereTransform->m[3],sphereTransform->m[4],sphereTransform->m[5],sphereTransform->m[6],sphereTransform->m[7],sphereTransform->m[8],sphereTransform->m[9],sphereTransform->m[10],sphereTransform->m[11],sphereTransform->m[12],sphereTransform->m[13],sphereTransform->m[14],sphereTransform->m[15]);
	}
	if(sphereTransform->m[11] < 0){
		sphereTransform->m[11] = 0;
	}
	if(sphereTransform->m[3] < 0){
		sphereTransform->m[3] = 0;
	}
	if((*t)++ <= 10){
		if((*t) == 10){
			//printf("X: %f, Y: %f, Z: %f\n", camMatrix->m[3], camMatrix->m[7], camMatrix->m[11]);
			(*t) = 0;
		}
	}
	//camMatrix->m[7] = -cam_height + calcHeight(camMatrix->m[3], camMatrix->m[11], ttex.width, tm->vertexArray);
}
