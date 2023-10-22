#ifndef TYPES_H
#define TYPES_H

#include <float.h>
#include <tgmath.h>

#ifdef C_RAY_SINGLE_PRECISION 
    typedef float CFLOAT;
    #define CF_MIN(x, y) fmin(x, y)
    #define CF_MAX(x, y) fmax(x, y) 
    #define F_EPSILON FLT_EPSILON
    #define F_MAX FLT_MAX
    #define F_MIN FLT_MIN
    #define CF_PI 3.14159265 
#else
    typedef double CFLOAT;
    #define CF_MIN(x, y) fmin(x, y)
    #define CF_MAX(x, y) fmax(x, y) 
    #define F_EPSILON DBL_EPSILON
    #define F_MAX DBL_MAX
    #define F_MIN DBL_MIN
    #define CF_PI 3.1415926535897932 
#endif

#endif

