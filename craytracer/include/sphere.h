#ifndef SPHERE_H
#define SPHERE_H

#include <stdbool.h>
#include <stdio.h>

#include "hypatiaINC.h"
#include "hitRecord.h"

#include "types.h"
#include "ray.h"
#include "material.h"
#include "allocator.h"

typedef struct aabb AABB;

typedef struct {
    // center of the sphere
    vec3 center;

    //material
    Material sphMat;

    // radius of the sphere
    CFLOAT radius;
} Sphere;


extern bool obj_sphereHit(const Sphere* restrict s, Ray r, CFLOAT t_min, CFLOAT t_max, HitRecord * outRecord);
extern bool obj_sphereCalcBoundingBox(const Sphere* restrict s, AABB * outbox);
extern void obj_sphereTexCoords(vec3 pointOnSphere, CFLOAT * outU, CFLOAT * outV);

// enum contaning different types of objects
typedef enum {
    SPHERE,
    OBJLL,
    OBJBVH
} ObjectType;

typedef struct {
    // ptr to the object stored in this node
    void * restrict object;

    // type of the object 
    ObjectType objType;
} Object;


extern Object * obj_createObject(void * restrict object, ObjectType type, 
                                 DynamicStackAlloc * restrict dsa );

// node of the linked list 
typedef struct objectLLNode ObjectLLNode;
typedef struct objectLLNode{
    Object obj;

    // points to the next node
    ObjectLLNode * restrict next;
} ObjectLLNode;

// linked list storing pointer to objects in the scene
typedef struct objectLL {
    // number of objects 
    size_t numObjects;
    
    // points to the first node in the linked list 
    ObjectLLNode * restrict head;
    
    // Dynamic allocation stack
    DynamicStackAlloc * restrict dsa;

    // Linear allocator
    LinearAllocFC * restrict hrAlloc;

    // whether the object is valid or not
    bool valid;
} ObjectLL;


// create and setup an object linked list and return a pointer to it
extern ObjectLL * obj_createObjectLL(
    DynamicStackAlloc * dsaAlloc, 
    DynamicStackAlloc * dsaObjs
);

// function to add an object to the linked list
// returns true if the operation is successful
extern bool obj_objectLLAdd(
        ObjectLL * restrict objll, 
        void * restrict obj, 
        ObjectType objType
);

// function to add spheres
// returns true if operation is successful
extern bool obj_objLLAddSphere(ObjectLL * restrict objll,
        Sphere s);

// remove an object at any index
extern bool obj_objectLLRemove(ObjectLL * restrict objll, size_t index); 

extern Object * obj_objectLLGetAT(const ObjectLL * restrict objll, size_t index);
extern void obj_objectLLSetAT(const ObjectLL * restrict objll, size_t index, Object object);

typedef bool (*ObjectComparator)(const Object * obj1, const Object * obj2);
extern void obj_objectLLSort(const ObjectLL * restrict objll, 
                             size_t start, 
                             size_t end, 
                             ObjectComparator comp);

// returns a hit record if any object in the list is intersected by the given ray
// under the given conditions
extern /*HitRecord**/bool obj_objLLHit (const ObjectLL* restrict objll, 
                          Ray r, 
                          CFLOAT t_min, 
                          CFLOAT t_max,
                          HitRecord * out);

extern bool obj_objectLLCalcBoundingBox(const ObjectLL * restrict objll, AABB* restrict outbox);

typedef struct aabb {
    vec3 maximum;
    vec3 minimum;
} AABB;

extern bool obj_AABBHit(const AABB* restrict s, Ray r, CFLOAT t_min, CFLOAT t_max);


typedef struct {
    AABB box;

    DynamicStackAlloc * restrict dsa;

    Object * restrict right;
    Object * restrict left;
} BVH;

extern BVH * obj_createBVH(DynamicStackAlloc * alloc, DynamicStackAlloc * dsa);

extern void obj_fillBVH(BVH * restrict bvh, 
                          const ObjectLL * restrict objects,
                          size_t start, size_t end); 
extern bool obj_bvhCalcBoundingBox(const BVH * restrict bvh, AABB * restrict outbox);
extern bool obj_bvhHit(const BVH* restrict bvh, Ray r, CFLOAT t_min, CFLOAT t_max, HitRecord * out); 

#endif

