#include "hitRecord.h"
#include "material.h"

void hr_setRecordi(CFLOAT distanceFromOrigin, vec3 point, vec3 normal, vec3 direction, HitRecord* restrict outRecord, const Material * restrict hitObjMat){
    // if temp < 0 then the ray has intersected the object at the front face
    // otherwise it has intersected the object at the back face 
    CFLOAT temp = vector3_dot_product(&direction, &normal);
    bool frontFace = (temp < 0) ? true : false;

    // Adjusting the normal so it always points away in the opposite direction 
    // of the ray
    if(!frontFace) {
        vector3_negate(&normal);
    }
    
    outRecord->point = point;
    outRecord->distanceFromOrigin = distanceFromOrigin;
    outRecord->normal = normal;
    outRecord->valid = true;
    outRecord->frontFace = frontFace;
    outRecord->hitObjMat = hitObjMat;
}

HitRecord hr_setRecord(CFLOAT distanceFromOrigin, vec3 point, vec3 normal, vec3 direction, const Material * restrict hitObjMat){
    // if temp < 0 then the ray has intersected the object at the front face
    // otherwise it has intersected the object at the back face 
    CFLOAT temp = vector3_dot_product(&direction, &normal);
    bool frontFace = (temp < 0) ? true : false;

    // Adjusting the normal so it always points away in the opposite direction 
    // of the ray
    if(!frontFace) {
        vector3_negate(&normal);
    }
    
    return (HitRecord){
        .point = point,
        .distanceFromOrigin = distanceFromOrigin,
        .normal = normal,
        .valid = true,
        .frontFace = frontFace,
        .hitObjMat = hitObjMat
    };
}

