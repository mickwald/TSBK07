#ifndef _PROJECT_IO_
#define _PROJECT_IO_

#include "MicroGlut.h"



void mouse(int x, int y);
void mouseDown(int button, int state, int x, int y);

void checkInput(int *t, float *sphereSpeed, mat4 *sphereTransform, mat4 *camMatrix, int *lockCamera, float *rotTest,  Collider *playerCol, drawObject *drawObjects, int *drawObjectsArrayElements, mat4 *tmpPlayerMat, bool *shoot, bool *playerAlive);

#endif
