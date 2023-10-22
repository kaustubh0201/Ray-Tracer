#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>


#include "hypatiaINC.h"
#include "color.h"
#include "types.h"

typedef enum {
    SOLID_COLOR,
    CHECKER,
    IMAGE
} TexType;

typedef struct {
    void * restrict tex;
    TexType texType;
} Texture;

typedef struct {
    RGBColorF color;
} SolidColor;


typedef struct {
    Texture odd;
    Texture even;
} Checker;


typedef struct {
    uint8_t * data;
    int32_t width;
    int32_t height;
    uint32_t bytesPerScanLine;
    int32_t compsPerPixel;
} Image;

extern RGBColorF tex_value(const Texture * restrict t, 
        CFLOAT u, CFLOAT v, vec3 p);

extern RGBColorF tex_solidColorValue(const SolidColor * restrict t);
extern RGBColorF tex_checkerValue(const Checker * restrict c,
                CFLOAT u, CFLOAT v, vec3 p);

extern void tex_loadImage(Image * restrict img, const char* filename);
extern RGBColorF tex_imageValue(const Image * restrict img, 
                CFLOAT u, CFLOAT v);

#endif
