#include "camera.h"
#include <stdlib.h>
#include <tgmath.h>

#include "util.h"

static CFLOAT degreesToRadians(CFLOAT deg){
    return (M_PI / 180.0) * deg; 
}

void cam_setCamera(Camera * restrict c, vec3 origin, CFLOAT aspectRatio, 
                          CFLOAT focalLength, CFLOAT vfov){      
    
    c->origin = origin;
    c->aspectRatio = aspectRatio;
    c->focalLength = focalLength;

    c->verticalFOV = degreesToRadians(vfov); 

    c->viewportHeight = 2.0 * tan(c->verticalFOV / 2.0);
    c->viewportWidth = c->aspectRatio * c->viewportHeight;

    c->vertical.y = c->viewportHeight;
    c->horizontal.x = c->viewportWidth;

    c->lowerLeftCorner.x = c->origin.x - c->horizontal.x/2 - c->vertical.x/2 - 0.0;
    c->lowerLeftCorner.y = c->origin.y - c->horizontal.y/2 - c->vertical.y/2 - 0.0;
    c->lowerLeftCorner.z = c->origin.z - c->horizontal.z/2 - c->vertical.z/2 - c->focalLength; 
}

void cam_setLookAtCamera(Camera * restrict c, vec3 lookFrom, 
                         vec3 lookAt, vec3 up, CFLOAT vfov, 
                         CFLOAT aspectRatio, CFLOAT aperture,
                         CFLOAT focusDist){

    c->aspectRatio = aspectRatio;

    c->verticalFOV = degreesToRadians(vfov);

    c->viewportHeight = 2.0 * tan(c->verticalFOV / 2.0); 
    c->viewportWidth = c->aspectRatio * c->viewportHeight;
    
    // w = dir(lookFrom - lookAt)
    c->w = lookFrom;
    vector3_subtract(&c->w, &lookAt);
    vector3_normalize(&c->w);
    
    // u = dir(up - w)
    vector3_cross_product(&c->u, &up, &c->w);
    vector3_normalize(&c->u);
    
    // v = cross (w, u)
    vector3_cross_product(&c->v, &c->w, &c->u);
    
    //origin = lookFrom 
    c->origin = lookFrom;

    // horizontal = focusDist * viewport_width * u
    c->horizontal = c->u;
    vector3_multiplyf(&c->horizontal, c->viewportWidth * focusDist);
    
    // vertical = focusDist * viewport_height * v
    c->vertical = c->v;
    vector3_multiplyf(&c->vertical, c->viewportHeight * focusDist);
    
    // lower_left_corner = origin - (horizontal + vertical)/2 - w * focusDist
    c->lowerLeftCorner = c->horizontal;
    vector3_add(&c->lowerLeftCorner, &c->vertical);
    vector3_multiplyf(&c->lowerLeftCorner, -0.5);
    vec3 temp = c->w;
    vector3_multiplyf(&temp, focusDist);
    vector3_subtract(&c->lowerLeftCorner, &temp);
    vector3_add(&c->lowerLeftCorner, &c->origin);


    c->lensRadius = aperture / 2.0;
}

Ray cam_getRay(const Camera * restrict cam, CFLOAT u, CFLOAT v){

    // randOnDist = lensRadius * util_randomUnitDisk()
    vec3 randOnDist = util_randomUnitDisk();
    vector3_multiplyf(&randOnDist, cam->lensRadius);
    
    CFLOAT x = randOnDist.x;
    CFLOAT y = randOnDist.y;

    // offset = randOnDist.x * c->u + randOnDist.y * c->v
    vec3 offset = {
        .x = x * cam->u.x + y * cam->v.x,
        .y = x * cam->u.y + y * cam->v.y,
        .z = x * cam->u.z + y * cam->v.z,
    };
    
    // outOrigin = cam->origin + offset
    vec3 outOrigin = cam->origin;
    vector3_add(&outOrigin, &offset);
    
    // direction = lowerLeftCorner + u*horizontal + v*vertical - origin - offset
    vec3 outDirection = cam->lowerLeftCorner;
    vec3 uHori = cam->horizontal;
    vector3_multiplyf(&uHori, u);
    vec3 vVeri = cam->vertical;
    vector3_multiplyf(&vVeri, v);
    vector3_add(&outDirection, &uHori);
    vector3_add(&outDirection, &vVeri);
    vector3_subtract(&outDirection, &cam->origin);
    vector3_subtract(&outDirection, &offset);

    return ray_create(outOrigin, outDirection);
}
