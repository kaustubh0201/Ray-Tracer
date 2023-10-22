#ifndef UTIL_H
#define UTIL_H

#include "hypatiaINC.h"
#include "types.h"

#include <stdint.h>
#include <stdbool.h>

extern CFLOAT util_floatClamp(CFLOAT c, CFLOAT lower, CFLOAT upper);
extern uint8_t util_uint8Clamp(uint8_t c, uint8_t lower, uint8_t upper);

// not in use
extern uint32_t util_randomRange(uint32_t lower, uint32_t upper);


extern vec3 util_randomUnitSphere();

extern vec3 util_randomUnitVector();
extern CFLOAT util_randomFloat(CFLOAT lower, CFLOAT upper);

extern vec3 util_vec3Reflect(vec3 v,vec3 n);

extern bool util_isVec3Zero(vec3 v);

extern vec3 util_randomUnitDisk();

#endif

