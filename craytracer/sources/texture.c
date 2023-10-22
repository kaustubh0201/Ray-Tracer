#include "texture.h"

#include "util.h"

#include <tgmath.h>
#include <stdio.h>

#include "stb_imageINC.h"

RGBColorF tex_value(const Texture * restrict t, CFLOAT u, CFLOAT v, vec3 p){
    if(t->texType == SOLID_COLOR){
        return tex_solidColorValue((SolidColor *)t->tex);
    }else if(t->texType == CHECKER){
        return tex_checkerValue((Checker *)t->tex, u, v, p);
    }else if(t->texType == IMAGE){
        return tex_imageValue((Image *)t->tex, u, v);
    }
    
    v += u + p.x;
    return (RGBColorF){.r = 0.0, .g = 0.0, .b = 0.0};
}

RGBColorF tex_solidColorValue(const SolidColor * restrict t){
    return t->color;
}

RGBColorF tex_checkerValue(const Checker * restrict c,
        CFLOAT u, CFLOAT v, vec3 p){


    CFLOAT sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z); 
    if(sines < 0){
        return tex_value(&c->odd, u, v, p);
    }else{
        return tex_value(&c->even, u, v, p);
    }
}


void tex_loadImage(Image * restrict img, const char* filename){
    img->data = stbi_load(filename, &img->width, &img->height, &img->compsPerPixel, 0);

    if(!img->data){
        printf("%s\n", filename);
        return;
    }

    img->bytesPerScanLine = img->compsPerPixel * img->width;
}

RGBColorF tex_imageValue(const Image * restrict img, 
        CFLOAT u, CFLOAT v){
    u = util_floatClamp(u, 0.0, 1.0);
    v = 1.0 - util_floatClamp(v, 0.0, 1.0);
    
    uint32_t i = u * img->width;
    uint32_t j = v * img->height;


    if(i >= (uint32_t)img->width){ i = img->width - 1; }
    if(j >= (uint32_t)img->height) { j = img->height - 1; }

    CFLOAT colorScale = 1.0/255.0;

    uint8_t * pixel = img->data + j * img->bytesPerScanLine + i * img->compsPerPixel;
    
    return (RGBColorF){
        .r = colorScale * pixel[0],
        .g = colorScale * pixel[1],
        .b = colorScale * pixel[2]
    };
}

