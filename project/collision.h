#ifndef _COLLISION_
#define _COLLISION_

#include "MicroGlut.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include <stdbool.h>


typedef struct Collider {
   bool AABB; // true - collider is AABB, false - collider is sphere
   GLfloat maxX, maxY, maxZ; // (for AABB) coords for the max in one axis, if sphere set to 0
   GLfloat minX, minY, minZ; // (for AABB) coords for the min in one axis, if sphere set to 0
   vec3 midPoint; // XYZ coord of middle of collider
   GLfloat radius; //value of sphere raduis, if AABB set to 0
} Collider;

typedef struct ColliderList ColliderList;

bool checkCollision(Collider c, Collider c2);
Collider makeSphereCollider(vec3 midP, GLfloat rad);

#endif
