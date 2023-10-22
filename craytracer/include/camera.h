#ifndef CAMERA_H
#define CAMERA_H

#include<stdbool.h>

#include "hypatiaINC.h"
#include "types.h"
#include "ray.h"

typedef struct camera {
    vec3 origin;
    vec3 lowerLeftCorner;
    vec3 horizontal;
    vec3 vertical;

    vec3 u;
    vec3 v;
    vec3 w;
    

    CFLOAT aspectRatio;
    CFLOAT focalLength;
    CFLOAT lensRadius;
    CFLOAT viewportHeight;
    CFLOAT viewportWidth;
    CFLOAT verticalFOV;
} Camera;

extern void cam_setCamera(Camera * restrict c, vec3 origin, CFLOAT aspectRatio, 
                          CFLOAT focalLength, CFLOAT vfov);

extern void cam_setLookAtCamera(Camera * restrict c, vec3 lookFrom, 
                                vec3 lookAt, vec3 up, CFLOAT vfov, 
                                CFLOAT aspectRatio, CFLOAT aperture,
                                CFLOAT focusDist);

extern Ray cam_getRay(const Camera * restrict cam, CFLOAT u, CFLOAT v);


#endif

