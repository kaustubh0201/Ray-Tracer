#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <check.h>
#include <tgmath.h>
#include <types.h>
#include <assert.h>
#include <stdbool.h>
#include "sphere.h"


#define ck_float_equal(a, b) ck_assert_int_eq(float_equal((a), (b)), 1)

#define float_equal(a, b) nearly_equal(a, b, 128 * F_EPSILON, F_MIN)

#define ck_assert_ld_vec3_eq(v1, v2) { \
    ck_float_equal((v1).x, (v2).x); \
    ck_float_equal((v1).y, (v2).y); \
    ck_float_equal((v1).z, (v2).z); \
} \

#define ck_assert_colorf_eq(v1, v2) { \
    ck_float_equal((v1).r, (v2).r); \
    ck_float_equal((v1).g, (v2).g); \
    ck_float_equal((v1).b, (v2).b); \
} \

#define ck_assert_coloru8_eq(v1, v2) { \
    ck_assert_int_eq(ck_assert_u8_eq((v1).r, (v2).r), 1); \
    ck_assert_int_eq(ck_assert_u8_eq((v1).g, (v2).g), 1); \
    ck_assert_int_eq(ck_assert_u8_eq((v1).b, (v2).b), 1); \
} \

#define ck_assert_objectLLNode_eq(n1, n2) { \
    ck_assert_int_eq(n1 == n2, 1); \
} \

extern bool nearly_equal(CFLOAT a, CFLOAT b, CFLOAT epsilon, CFLOAT abs_th){
    assert(F_EPSILON <= epsilon);
    assert(epsilon < 1.0);

    if (a == b) return true;

    CFLOAT diff = fabs(a - b);
    CFLOAT norm = fmin((fabs(a) + fabs(b)), F_MAX);
    return diff < fmax(abs_th, epsilon * norm);
}


extern bool ck_assert_u8_eq(uint8_t a, uint8_t b){
    return a == b;
}

#endif 