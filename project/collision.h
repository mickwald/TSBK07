#include "MicroGlut.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include <stdbool.h>

typedef struct Collider Collider;

typedef struct ColliderList ColliderList;

bool checkCollision(Collider c, Collider c2);
