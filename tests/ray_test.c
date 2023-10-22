#include <stdlib.h>

#define HYPATIA_IMPLEMENTATION

#include <check.h>
#include <stdbool.h>

#include "hypatiaINC.h"
#include "ray.h"
#include "types.h"
#include "testutils.h"


START_TEST(check_ray_1){
    vec3 origin = {
        .x = 1.0,
        .y = 2.0,
        .z = 3.0
    };

    vec3 direction ={
        .x = 4.0,
        .y = 5.0,
        .z = 6.0
    };
    
    vec3 expected_direction = {
        .x = 0.45584230583855179,
        .y = 0.56980288229818976,
        .z =0.68376345875782762
    };
    Ray t_ray = ray_create(origin, direction);
    ck_assert_ld_vec3_eq(origin, t_ray.origin);
    ck_assert_ld_vec3_eq(expected_direction, t_ray.direction);
}
END_TEST

Suite* hr_suite(void){
    Suite *s;
    TCase *tc_core;

    s = suite_create("Ray");

    //Core test case
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, check_ray_1);
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
