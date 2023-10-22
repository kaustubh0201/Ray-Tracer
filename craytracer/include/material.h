#ifndef MATERIAL_H
#define MATERIAL_H

#include "types.h"
#include "ray.h"
#include "hitRecord.h"
#include "hypatiaINC.h"
#include "color.h"
#include "texture.h"

#include <stdbool.h>

typedef struct metalMat {
    RGBColorF albedo;
    CFLOAT fuzz;
} MetalMat;

typedef struct lambertianMat {
    //RGBColorF albedo;
    Texture lambTexture;
} LambertianMat;

typedef struct dielectricMat {
    CFLOAT ir;
} DielectricMat;

typedef enum materialType {
    LAMBERTIAN,
    METAL,
    DIELECTRIC
} MaterialType;

typedef struct material {
    const void * mat;
    MaterialType matType;
} Material;


bool mat_scatter (
        const Ray * restrict rayIn, 
        const HitRecord * restrict rec, 
        RGBColorF * restrict attenuation, 
        Ray * restrict out
    );


// IP = in place
#define MAT_CREATE_LAMB_IP(lambMatptr) { .mat = (lambMatptr), .matType = LAMBERTIAN}
#define MAT_CREATE_METAL_IP(metalMatptr) { .mat = (metalMatptr), .matType = METAL}
#define MAT_CREATE_DIELECTRIC_IP(dielecMatptr) { .mat = (dielecMatptr), .matType = DIELECTRIC}

#define MAT_CREATE_LAMB(lambMatptr) (Material){ .mat = (lambMatptr), .matType = LAMBERTIAN}
#define MAT_CREATE_METAL(metalMatptr) (Material){ .mat = (metalMatptr), .matType = METAL}
#define MAT_CREATE_DIELECTRIC(dielecMatptr) (Material){ .mat = (dielecMatptr), .matType = DIELECTRIC}



#endif
