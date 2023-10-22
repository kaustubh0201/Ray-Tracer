#include <stdlib.h>

#define HYPATIA_IMPLEMENTATION

#include <check.h>
#include <stdbool.h>

#include "hypatiaINC.h"
#include "hitRecord.h"
#include "types.h"
#include "testutils.h"
#include "material.h"

START_TEST(check_hr_1){
    vec3 point = {
        .x = -1.0,
        .y = -2.0,
        .z = -3.0
    };
    
    vec3 normal = {
        .x = 3.0,
        .y = 1.0,
        .z = -7.0
    };
    
    vec3 direction = {
        .x = 2.0,
        .y = 3.0,
        .z = 10.0
    };
    
    Material m;

    HitRecord temp_hr_1 = hr_setRecord(4.0, point, normal, direction, &m);
    
    ck_assert_ld_vec3_eq(point, temp_hr_1.point);
    ck_assert_ld_vec3_eq(normal, temp_hr_1.normal);
    ck_assert_int_eq(1, temp_hr_1.frontFace);
    ck_float_equal(4.0, temp_hr_1.distanceFromOrigin);
    ck_assert_int_eq(1, temp_hr_1.valid);
    ck_assert_ptr_eq(temp_hr_1.hitObjMat, &m);
}
END_TEST

START_TEST(check_hr_2){
    vec3 p ={
        .x = -3.0,
        .y = -2.0,
        .z = -4.0
    };

    vec3 n = {
        .x = -4.0,
        .y = -5.0,
        .z = -9.0
    };

    vec3 d = {
        .x = -2.0,
        .y = -7.0,
        .z = -4.0
    };

    Material m;
    HitRecord t_hr = hr_setRecord(7.0, p, n, d, &m);

    vec3 ne_n = {
        .x = 4.0,
        .y = 5.0,
        .z = 9.0
    };

    ck_assert_ld_vec3_eq(p, t_hr.point);
    ck_assert_ld_vec3_eq(ne_n, t_hr.normal);
    ck_float_equal(7.0, t_hr.distanceFromOrigin);
    ck_assert_int_eq(0, t_hr.frontFace);
    ck_assert_int_eq(1, t_hr.valid);
    ck_assert_ptr_eq(t_hr.hitObjMat, &m);
}
END_TEST

Suite* hr_suite(void){
    Suite *s;
    TCase *tc_core;

    s = suite_create("hitRecord");

    //Core test case
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, check_hr_1);
    tcase_add_test(tc_core, check_hr_2);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void){

    int number_failed;
    Suite* s;
    SRunner *sr;

    s = hr_suite();
    sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);


    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
