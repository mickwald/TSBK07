//
//Author: Gustav Jannerng & Michael Karlsson
//Course: TSBK07 Computer Graphics
//

#include "MicroGlut.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include <stdbool.h>

#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

typedef struct Collider {
   bool AABB; // true - collider is AABB, false - collider is sphere
   GLfloat maxX, maxY, maxZ; // (for AABB) coords for the max in one axis, if sphere set to 0
   GLfloat minX, minY, minZ; // (for AABB) coords for the min in one axis, if sphere set to 0
   vec3 midPoint; // XYZ coord of middle of collider
   GLfloat radius; //value of sphere raduis, if AABB set to 0
} Collider;

typedef struct ColliderList {
  struct Collider col;
  struct ColliderList* next;
} ColliderList;

bool checkCollision(Collider c, Collider c2){

  GLfloat collisionRadius = 10; // check for collisions if inside this
  GLfloat radiusSqrd = pow(collisionRadius,2);

  GLfloat distanceX = c2.midPoint.x - c.midPoint.x;
  GLfloat distanceY = c2.midPoint.y - c.midPoint.y;
  GLfloat distanceZ = c2.midPoint.z - c.midPoint.z;

  vec3 distance = SetVector(distanceX, distanceY, distanceZ);
  GLfloat distanceSqrd = pow(distance.x,2)+pow(distance.y,2)+pow(distance.z,2);

  if(radiusSqrd >= distanceSqrd){ //close enough to check collision

    if(c.AABB && c2.AABB){ // both colliders are AABB

      if((c.minX <= c2.maxX && c.maxX >= c2.minX)
      &&(c.minY <= c2.maxY && c.maxY >= c2.minY)
      &&(c.minZ <= c2.maxZ && c.maxZ >= c2.minZ)){
         return true;
       }
    }
    else if((c.AABB && !(c2.AABB)) || (!(c.AABB) && c2.AABB)){ // one AABB and one sphere

      if(c.AABB){
      GLfloat x = max(c.minX, min(c2.midPoint.x, c.maxX));
      GLfloat y = max(c.minY, min(c2.midPoint.y, c.maxY));
      GLfloat z = max(c.minZ, min(c2.midPoint.z, c.maxZ));

      distanceSqrd = pow(x,2) + pow(y,2) + pow(z,2);

      if(distanceSqrd < c2.radius){
        return true;
      }
      }
      else{

        GLfloat x = max(c2.minX, min(c.midPoint.x, c2.maxX));
        GLfloat y = max(c2.minY, min(c.midPoint.y, c2.maxY));
        GLfloat z = max(c2.minZ, min(c.midPoint.z, c2.maxZ));

        distanceSqrd = pow(x,2) + pow(y,2) + pow(z,2);

        if(distanceSqrd < c.radius){
          return true;
        }
      }

    }
    else{ // both colliders are spheres

      if(distanceSqrd < pow((c.radius + c2.radius),2)){
        return true;
      }
    }

  }
  return false;
  }
