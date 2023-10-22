#define HYPATIA_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <assert.h>
#include <tgmath.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdalign.h>
#include <omp.h>

#include "outfile.h"
#include "util.h"
#include "camera.h"
#include "sphere.h"
#include "hitRecord.h"
#include "types.h"
#include "hypatiaINC.h"
#include "ray.h"
#include "material.h"
#include "color.h"
#include "allocator.h"
#include "texture.h"

RGBColorU8 writeColor(CFLOAT r, CFLOAT g, CFLOAT b, int sample_per_pixel){
    CFLOAT scale = 1.0/sample_per_pixel;

    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    return COLOR_U8CREATE(r, g, b);
}
/*
HitRecord* hittableList(int n, Sphere sphere[n], Ray ray, LinearAllocFC * restrict hrAlloc, CFLOAT t_min, CFLOAT t_max){
    HitRecord * r = (HitRecord *) alloc_linearAllocFCAllocate(hrAlloc);
    HitRecord * h = NULL;

    for(int i = 0; i < n; i++){
        obj_sphereHit(sphere + i, ray, t_min, t_max, r);

        if(r->valid){

            if(h == NULL ){
                h = r;
                r = (HitRecord *) alloc_linearAllocFCAllocate(hrAlloc);
            }else if(r->distanceFromOrigin < h->distanceFromOrigin){
                // alloc_poolAllocFree(hrAlloc, h);
                h = r;
                r = (HitRecord *) alloc_linearAllocFCAllocate(hrAlloc);
            }
        }
    }    
    return h;
}
*/

RGBColorF ray_c(Ray r, const ObjectLL * world, int depth){

    if(depth <= 0){
        return (RGBColorF){0};
    }
    
    HitRecord rec;
    rec.valid = false;
    /*HitRecord *  rec =*/bool checkHit = obj_objLLHit(world, r, 0.00001, FLT_MAX, &rec);
    if(checkHit){
        Ray scattered = {0};
        RGBColorF attenuation = {0};
        
        if(mat_scatter(&r, &rec, &attenuation, &scattered)){
            RGBColorF color = ray_c(scattered, world, depth - 1);
            color = colorf_multiply(color, attenuation); 

            
            return color;
        }

        return (RGBColorF){0};
    }

    vec3 ud = r.direction;
    vector3_normalize(&ud);
    CFLOAT t = 0.5 * (ud.y + 1.0);
    vec3 inter4;
    vector3_setf3(&inter4, 1.0 - t, 1.0 - t, 1.0 - t);
    vec3 inter3;
    vector3_setf3(&inter3, 0.5 * t, 0.7 * t, 1.0 * t);
    vector3_add(&inter3, &inter4);
    
    return (RGBColorF){
        .r = inter3.x,
        .g = inter3.y,
        .b = inter3.z
    };
}
        
void printProgressBar(int i, int max){
    int p = (int)(100 * (CFLOAT)i/max);

    printf("|");
    for(int j = 0; j < p; j++){
        printf("=");
    }

    for(int j = p; j < 100; j++){
        printf("*");
    }

    if(p == 100){
        printf("|\n");
    }else{
        printf("|\r");
    }
}

#define randomFloat() util_randomFloat(0.0, 1.0)

void randomSpheres(ObjectLL * world, DynamicStackAlloc * dsa){

    LambertianMat* materialGround = alloc_dynamicStackAllocAllocate(dsa, 
                                    sizeof(LambertianMat), 
                                    alignof(LambertianMat));
    SolidColor * sc1 = alloc_dynamicStackAllocAllocate(dsa,
                      sizeof(SolidColor), alignof(SolidColor));
    
    SolidColor * sc = alloc_dynamicStackAllocAllocate(dsa,
                      sizeof(SolidColor), alignof(SolidColor));

    Checker * c = alloc_dynamicStackAllocAllocate(dsa, sizeof(Checker), alignof(Checker));

    sc1->color = (RGBColorF) {.r = 0.2, .b = 0.3, .g = 0.1};
    sc->color = (RGBColorF){.r = 0.9, .b = 0.9, .g = 0.9};

    c->even.tex = sc1;
    c->even.texType = SOLID_COLOR;
    c->odd.tex = sc;
    c->odd.texType = SOLID_COLOR;



    materialGround->lambTexture.tex = c;
    materialGround->lambTexture.texType = CHECKER;

    /*materialGround->albedo.r = 0.5;
    materialGround->albedo.g = 0.5;
    materialGround->albedo.b = 0.5;*/

    obj_objLLAddSphere(world, (Sphere){
        .center = {.x = 0, .y = -1000, .z = 0}, .radius = 1000, .sphMat = MAT_CREATE_LAMB_IP(materialGround) 
    });

    for (int a = -11; a < 11; a++){
        for (int b = -11; b < 11; b++){
            CFLOAT chooseMat = randomFloat();
            vec3 center = {
                .x = a + 0.9 * randomFloat(),
                .y = 0.2, 
                .z = b + 0.9 * randomFloat()
            };

            CFLOAT length = sqrtf((center.x - 4) * (center.x - 4) 
                        +   (center.y - 0.2) * (center.y - 0.2)
                        +   (center.z - 0) * (center.z - 0));
            
            if(length > 0.9){
                if(chooseMat < 0.8){
                    // diffuse
                    RGBColorF albedo = {
                        .r = randomFloat() * randomFloat(),
                        .g = randomFloat() * randomFloat(),
                        .b = randomFloat() * randomFloat(),
                    };

                    LambertianMat* lambMat = alloc_dynamicStackAllocAllocate(dsa, 
                                            sizeof(LambertianMat), 
                                            alignof(LambertianMat));
                    
                    SolidColor * sc = alloc_dynamicStackAllocAllocate(dsa,
                                      sizeof(SolidColor), alignof(SolidColor));

                    sc->color = albedo;

                    lambMat->lambTexture.tex = sc;
                    lambMat->lambTexture.texType = SOLID_COLOR;

                    obj_objLLAddSphere(world, (Sphere) {
                        .center = center,
                        .radius = 0.2,
                        .sphMat = MAT_CREATE_LAMB_IP(lambMat) 
                    });

                }else if(chooseMat < 0.95){
                    // metal
                    RGBColorF albedo = {
                        .r = util_randomFloat(0.5, 1.0),
                        .g = util_randomFloat(0.5, 1.0),
                        .b = util_randomFloat(0.5, 1.0)
                    };
                    CFLOAT fuzz = util_randomFloat(0.5, 1.0);
                        
                    MetalMat* metalMat = alloc_dynamicStackAllocAllocate(dsa, 
                                         sizeof(MetalMat), 
                                         alignof(MetalMat));

                    metalMat->albedo = albedo;
                    metalMat->fuzz = fuzz;

                    obj_objLLAddSphere(world, (Sphere) {
                        .center = center,
                        .radius = 0.2,
                        .sphMat = MAT_CREATE_METAL_IP(metalMat) 
                    });

 
                }else{
                    DielectricMat * dMat = alloc_dynamicStackAllocAllocate(dsa, 
                                           sizeof(DielectricMat), 
                                           alignof(DielectricMat));
                    dMat->ir = 1.5;
                    obj_objLLAddSphere(world, (Sphere) {
                        .center = center,
                        .radius = 0.2,
                        .sphMat = MAT_CREATE_DIELECTRIC_IP(dMat)
                    });

                }

            }
        }
    }

    DielectricMat* material1 = alloc_dynamicStackAllocAllocate(dsa, 
                                           sizeof(DielectricMat), 
                                           alignof(DielectricMat));    
    material1->ir = 1.5;

    obj_objLLAddSphere(world, (Sphere){
        .center = {.x = 0, .y = 1, .z = 0},
        .radius = 1.0,
        .sphMat = MAT_CREATE_DIELECTRIC_IP(material1) 
    });

    
    LambertianMat* material2 = alloc_dynamicStackAllocAllocate(dsa, 
                                           sizeof(LambertianMat), 
                                           alignof(LambertianMat));

    sc = alloc_dynamicStackAllocAllocate(dsa,
                      sizeof(SolidColor), alignof(SolidColor));
    
    
    sc->color = (RGBColorF) { .r = 0.4, .g = 0.2, .b = 0.1 };
    material2->lambTexture.tex = sc;
    material2->lambTexture.texType = SOLID_COLOR;
    /*material2->albedo.r = 0.4;
    material2->albedo.g = 0.2;
    material2->albedo.b = 0.1;
    */

    obj_objLLAddSphere(world, (Sphere){
        .center = {.x = -4, .y = 1, .z = 0},
        .radius = 1.0,
        .sphMat = MAT_CREATE_LAMB_IP(material2) 
    });
        

    MetalMat* material3 = alloc_dynamicStackAllocAllocate(dsa, 
                                       sizeof(MetalMat), 
                                       alignof(MetalMat));     
    material3->albedo.r = 0.7;
    material3->albedo.g = 0.6;
    material3->albedo.b = 0.5;
    material3->fuzz = 0.0;

    obj_objLLAddSphere(world, (Sphere){
        .center = {.x = 4, .y = 1, .z = 0},
        .radius = 1.0,
        .sphMat = MAT_CREATE_METAL_IP(material3) 
    });

}

CFLOAT lcg(int * n){

  static int seed;
  const int m = 2147483647;
  const int a = 1103515245;
  const int c = 12345;

  if(n != NULL){
    seed = *n;
  }

  seed = (seed * a + c) % m;
  *n = seed;

  return fabs((CFLOAT)seed / m);  
}

void randomSpheres2(ObjectLL * world, DynamicStackAlloc * dsa, int n, Image imgs[n], int * seed){

    LambertianMat* materialGround = alloc_dynamicStackAllocAllocate(dsa, 
                                    sizeof(LambertianMat), 
                                    alignof(LambertianMat));
    SolidColor * sc1 = alloc_dynamicStackAllocAllocate(dsa,
                      sizeof(SolidColor), alignof(SolidColor));
    
    SolidColor * sc = alloc_dynamicStackAllocAllocate(dsa,
                      sizeof(SolidColor), alignof(SolidColor));

    Checker * c = alloc_dynamicStackAllocAllocate(dsa, sizeof(Checker), alignof(Checker));

    sc1->color = (RGBColorF) {.r = 0.0, .b = 0.0, .g = 0.0};
    sc->color = (RGBColorF){.r = 0.4, .b = 0.4, .g = 0.4};

    c->even.tex = sc1;
    c->even.texType = SOLID_COLOR;
    c->odd.tex = sc;
    c->odd.texType = SOLID_COLOR;



    materialGround->lambTexture.tex = c;
    materialGround->lambTexture.texType = CHECKER;

    /*materialGround->albedo.r = 0.5;
    materialGround->albedo.g = 0.5;
    materialGround->albedo.b = 0.5;*/

    obj_objLLAddSphere(world, (Sphere){
        .center = {.x = 0, .y = -1000, .z = 0}, .radius = 1000, .sphMat = MAT_CREATE_LAMB_IP(materialGround) 
    });

    for (int a = -2; a < 9; a++){
        for (int b = -9; b < 9; b++){
            CFLOAT chooseMat = lcg(seed);
            vec3 center = {
                .x = a + 0.9 * lcg(seed),
                .y = 0.2, 
                .z = b + 0.9 * lcg(seed)
            };

            if(chooseMat < 0.8){
                // diffuse
                RGBColorF albedo = {
                        .r = lcg(seed) * lcg(seed),
                        .g = lcg(seed) * lcg(seed),
                        .b = lcg(seed) * lcg(seed),

                };

                LambertianMat* lambMat = alloc_dynamicStackAllocAllocate(dsa, 
                                        sizeof(LambertianMat), 
                                        alignof(LambertianMat));
                
                SolidColor * sc = alloc_dynamicStackAllocAllocate(dsa,
                                  sizeof(SolidColor), alignof(SolidColor));

                sc->color = albedo;

                lambMat->lambTexture.tex = sc;
                lambMat->lambTexture.texType = SOLID_COLOR;

                obj_objLLAddSphere(world, (Sphere) {
                    .center = center,
                    .radius = 0.2,
                    .sphMat = MAT_CREATE_LAMB_IP(lambMat) 
                });

            }else if(chooseMat < 0.95){
                // metal
                RGBColorF albedo = {
                    .r = lcg(seed)/2 + 0.5,
                    .g = lcg(seed)/2 + 0.5,
                    .b = lcg(seed)/2 + 0.5
                };
                CFLOAT fuzz = lcg(seed)/2 + 0.5;
                    
                MetalMat* metalMat = alloc_dynamicStackAllocAllocate(dsa, 
                                     sizeof(MetalMat), 
                                     alignof(MetalMat));

                metalMat->albedo = albedo;
                metalMat->fuzz = fuzz;

                obj_objLLAddSphere(world, (Sphere) {
                    .center = center,
                    .radius = 0.2,
                    .sphMat = MAT_CREATE_METAL_IP(metalMat) 
                });


            }else{
                DielectricMat * dMat = alloc_dynamicStackAllocAllocate(dsa, 
                                       sizeof(DielectricMat), 
                                       alignof(DielectricMat));
                dMat->ir = 1.5;
                obj_objLLAddSphere(world, (Sphere) {
                    .center = center,
                    .radius = 0.2,
                    .sphMat = MAT_CREATE_DIELECTRIC_IP(dMat)
                });

            }

        }
    
    }

    LambertianMat* material2 = alloc_dynamicStackAllocAllocate(dsa, 
                                           sizeof(LambertianMat), 
                                           alignof(LambertianMat));

    
    
    material2->lambTexture.tex = &imgs[0];
    material2->lambTexture.texType = IMAGE;
    /*material2->albedo.r = 0.4;
    material2->albedo.g = 0.2;
    material2->albedo.b = 0.1;
    */

    obj_objLLAddSphere(world, (Sphere){
        .center = {.x = -4, .y = 1, .z = 0},
        .radius = 1.0,
        .sphMat = MAT_CREATE_LAMB_IP(material2) 
    });


   material2 = alloc_dynamicStackAllocAllocate(dsa, 
                                           sizeof(LambertianMat), 
                                           alignof(LambertianMat));

    
    
    material2->lambTexture.tex = &imgs[1];
    material2->lambTexture.texType = IMAGE;
    /*material2->albedo.r = 0.4;
    material2->albedo.g = 0.2;
    material2->albedo.b = 0.1;
    */

    obj_objLLAddSphere(world, (Sphere){
        .center = {.x = -4, .y = 1, .z = -2.2},
        .radius = 1.0,
        .sphMat = MAT_CREATE_LAMB_IP(material2) 
    });

   material2 = alloc_dynamicStackAllocAllocate(dsa, 
                                           sizeof(LambertianMat), 
                                           alignof(LambertianMat));

    
    
    material2->lambTexture.tex = &imgs[2];
    material2->lambTexture.texType = IMAGE;
    /*material2->albedo.r = 0.4;
    material2->albedo.g = 0.2;
    material2->albedo.b = 0.1;
    */

    obj_objLLAddSphere(world, (Sphere){
        .center = {.x = -4, .y = 1, .z = +2.2},
        .radius = 1.0,
        .sphMat = MAT_CREATE_LAMB_IP(material2) 
    });

   material2 = alloc_dynamicStackAllocAllocate(dsa, 
                                           sizeof(LambertianMat), 
                                           alignof(LambertianMat));

    
    
    material2->lambTexture.tex = &imgs[3];
    material2->lambTexture.texType = IMAGE;
    /*material2->albedo.r = 0.4;
    material2->albedo.g = 0.2;
    material2->albedo.b = 0.1;
    */

    obj_objLLAddSphere(world, (Sphere){
        .center = {.x = -4, .y = 1, .z = -4.2},
        .radius = 1.0,
        .sphMat = MAT_CREATE_LAMB_IP(material2) 
    });
}
#undef randomFloat

int main(int argc, char *argv[]){

    if(argc < 2){
        printf("FATAL ERROR: Output file name not provided.\n");
        printf("EXITING ...\n");
        return 0;
    }

    srand(time(NULL));


    const CFLOAT aspect_ratio = 16.0 / 9.0;
    const int WIDTH = 1024;
    const int HEIGHT = (int)(WIDTH/aspect_ratio);
    const int SAMPLES_PER_PIXEL = 100;
    const int MAX_DEPTH = 50;
    RGBColorU8* image = (RGBColorU8*) malloc(sizeof(RGBColorF) * HEIGHT * WIDTH);
    
    CFLOAT start = omp_get_wtime();


    uint32_t stepSize = 500;
    uint32_t totalSteps = (WIDTH * HEIGHT)/stepSize + 1;
    size_t stepsCompleted = 0;

#pragma omp parallel num_threads(1)
{
    size_t localSteps = 0;

    int seed = 100;

    vec3 lookFrom = {.x = 13.0, .y = 2.0, .z = 3.0};
    vec3 lookAt = {.x = 0.0, .y = 0.0, .z = 0.0};
    vec3 up = {.x = 0.0, .y = 1.0, .z = 0.0};

    CFLOAT distToFocus = 10.0;
    CFLOAT aperture = 0.1;

    Camera c;
    cam_setLookAtCamera(&c, lookFrom, lookAt, up, 20, aspect_ratio, aperture, distToFocus);

    Ray r;
    RGBColorF temp;
    

    DynamicStackAlloc * dsa = alloc_createDynamicStackAllocD(1024, 100);
    // DynamicStackAlloc * dsa1 = alloc_createDynamicStackAllocD(1024, 100);
    DynamicStackAlloc * dsa0 = alloc_createDynamicStackAllocD(1024, 10);
    ObjectLL * world = obj_createObjectLL(dsa0, dsa);
   
    Image img[4];

    tex_loadImage(&img[0], "/home/lenovo/VTOP/pdc/project/pdc_project/testtextures/tex1.jpg");
    tex_loadImage(&img[1], "/home/lenovo/VTOP/pdc/project/pdc_project/testtextures/tex2.jpg");
    tex_loadImage(&img[2], "/home/lenovo/VTOP/pdc/project/pdc_project/testtextures/tex3.jpg");
    tex_loadImage(&img[3], "/home/lenovo/VTOP/pdc/project/pdc_project/testtextures/tex4.jpg");

    randomSpheres2(world, dsa, 4, img, &seed);

#pragma omp barrier
    /* 
    const char * file = "/home/lenovo/VTOP/pdc/project/pdc_project/testtextures/img2.png";

    Image img = {0};
    tex_loadImage(&img, file);

    Texture t = {
        .tex = &img,
        .texType = IMAGE
    };


    LambertianMat* matgnd = alloc_dynamicStackAllocAllocate(dsa, 
                                    sizeof(LambertianMat), 
                                    alignof(LambertianMat));

    matgnd->lambTexture = t;
    
    
    obj_objLLAddSphere(world, (Sphere){
        .center = {.x = 0, .y = 0, .z = 0}, .radius = 2, .sphMat = MAT_CREATE_LAMB_IP(matgnd) 
    });*/

   
    // BVH * bvh = obj_createBVH(dsa0, dsa1);
    // obj_fillBVH(bvh, world, 0, world->numObjects - 1);

    LinearAllocFC * lafc = alloc_createLinearAllocFC(
            MAX_DEPTH * world->numObjects, 
            sizeof(HitRecord), 
            alignof(HitRecord));

    world->hrAlloc = lafc;
    

    CFLOAT pcR, pcG, pcB;
    
    #pragma omp for
    for(int l = 0; l < WIDTH * HEIGHT; l++){
        //int z = omp_get_thread_num();

        int j = (HEIGHT - 1) - l / WIDTH;
        int i = l % WIDTH;
        pcR = pcG = pcB = 0.0;

        for(int k = 0; k < SAMPLES_PER_PIXEL; k++){
            CFLOAT u = ((CFLOAT)i + util_randomFloat(0.0, 1.0)) / (WIDTH - 1);
            CFLOAT v = ((CFLOAT)j + util_randomFloat(0.0, 1.0)) / (HEIGHT - 1);
            r = cam_getRay(&c, u, v);
          
            temp = ray_c(r, world, MAX_DEPTH);

            pcR += temp.r;
            pcG += temp.g;
            pcB += temp.b;   
            
            alloc_linearAllocFCFreeAll(lafc);
        }
        
        image[i + WIDTH * (HEIGHT - 1 - j)] = writeColor(pcR, pcG, pcB, SAMPLES_PER_PIXEL);
        
        localSteps += 1;

        if(localSteps % stepSize == stepSize - 1){
            #pragma omp atomic
            stepsCompleted += 1;


            if(stepsCompleted % 100 == 1){
                #pragma omp critical
                printf("Progress %lu of %u (%0.2lf%%)\n", stepsCompleted, totalSteps, 100.0 * (CFLOAT)stepsCompleted/totalSteps);
            }
        }
        /*#pragma omp single
        {
        } */

        //if(i == WIDTH - 1)
        //printProgressBar(HEIGHT - 1 - j, HEIGHT - 1);

    }


    alloc_freeLinearAllocFC(lafc);
    alloc_freeDynamicStackAllocD(dsa);
    alloc_freeDynamicStackAllocD(dsa0);
}

    CFLOAT end = omp_get_wtime();
    
    printf("Execution time: %lf\n", end - start);

    writeToPPM(argv[1], WIDTH, HEIGHT, image);
    free(image);
    //alloc_freeDynamicStackAllocD(dsa1);
    
    return 0;
}


    
/*    for (int j = HEIGHT - 1; j >= 0; j--){
        for (int i = 0; i < WIDTH; i++){
            
            pcR = pcG = pcB = 0.0;

            for(int k = 0; k < SAMPLES_PER_PIXEL; k++){
                CFLOAT u = ((CFLOAT)i + util_randomFloat(0.0, 1.0)) / (WIDTH - 1);
                CFLOAT v = ((CFLOAT)j + util_randomFloat(0.0, 1.0)) / (HEIGHT - 1);
                r = cam_getRay(&c, u, v);
              
                temp = ray_c(r, world, MAX_DEPTH);

                pcR += temp.r;
                pcG += temp.g;
                pcB += temp.b;   

                alloc_linearAllocFCFreeAll(lafc);
            }

            image[i + WIDTH * (HEIGHT - 1 - j)] = writeColor(pcR, pcG, pcB, SAMPLES_PER_PIXEL);
        }

        printProgressBar(HEIGHT - 1 - j, HEIGHT - 1);
    }
*/

