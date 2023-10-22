#ifndef RAY_H
#define RAY_H

#include "hypatiaINC.h"

typedef struct ray {
    // origin of the ray
    vec3 origin;

    // direction of the ray
    vec3 direction;
} Ray;

extern Ray ray_create(vec3 origin, vec3 direction);

#endif
