#include <stdlib.h>

#define HYPATIA_IMPLEMENTATION

#include <check.h>
#include <time.h>
#include "util.h"

#include "testutils.h"

START_TEST(check_util_h_1){
    ck_assert_float_eq(util_floatClamp(8.9L, 6.0L, 8.0L), 8.0L);
}
END_TEST

START_TEST(check_util_h_2){
    ck_assert_float_eq(util_floatClamp(5.9L, 6.0L, 8.0L), 6.0L);
}
END_TEST

START_TEST(check_util_h_3){
    ck_assert_float_eq(util_floatClamp(7.0L, 6.0L, 8.0L),7.0L);
}
END_TEST

START_TEST(check_util_h_4){
    ck_assert_ldouble_lt(util_randomFloat(3.0L,5.0L),5.0L);
    ck_assert_ldouble_ge(util_randomFloat(3.0L,5.0L),3.0L);

    ck_assert_ldouble_lt(util_randomFloat(100.0L,50.0L),100.0L);
    ck_assert_ldouble_ge(util_randomFloat(100.0L,50.0L),50.0L);

    ck_assert_ldouble_lt(util_randomFloat(34.0L,1353.0L),1353.0L);
    ck_assert_ldouble_ge(util_randomFloat(34.0L,1353.0L),34.0L);
}END_TEST

START_TEST(check_util_h_5){
    vec3 t;
    t = util_randomUnitSphere();
    
    ck_assert_ldouble_lt(t.x, 1.0L);
    ck_assert_ldouble_ge(t.x, 0.0L);

    ck_assert_ldouble_lt(t.y, 1.0L);
    ck_assert_ldouble_ge(t.y, 0.0L);

    ck_assert_ldouble_lt(t.z, 1.0L);
    ck_assert_ldouble_ge(t.z, 0.0L);

    int sq = t.x*t.x + t.y*t.y + t.z*t.z;

    ck_assert_ldouble_lt(sq, 1.0L);

}END_TEST

START_TEST(check_util_h_6){

    vec3 t = util_randomUnitVector();

    ck_assert_ldouble_lt(t.x, 1.0L);
    ck_assert_ldouble_ge(t.x, 0.0L);

    ck_assert_ldouble_lt(t.y, 1.0L);
    ck_assert_ldouble_ge(t.y, 0.0L);

    ck_assert_ldouble_lt(t.z, 1.0L);
    ck_assert_ldouble_ge(t.z, 0.0L);

    CFLOAT sq = t.x * t.x + t.y * t.y + t.z * t.z;

    ck_float_equal(sq, 1.0);

}END_TEST

START_TEST(check_util_h_7){
    vec3 v = {
        .x = 1.0,
        .y = 2.0,
        .z = 3.0
    };
    
    vec3 n = {
        .x = 4.0,
        .y = 5.0,
        .z = 6.0
    };

    vec3 expected = {
        .x = -255.0,
        .y = -318.0,
        .z = -381.0
    };

    ck_assert_ld_vec3_eq(expected, util_vec3Reflect(v, n));

}END_TEST

START_TEST(check_util_near_zero_corr){
    bool b = util_isVec3Zero((vec3){.x = 0.0, .y = 0.0, .z = 0.0});
    ck_assert_int_eq(b, 1);
}END_TEST

START_TEST(check_util_near_zero_incorr){
    bool b = util_isVec3Zero((vec3){.x = 1.0, .y = 0.0, .z = 0.0});
    ck_assert_int_eq(b, 0);
}END_TEST

START_TEST(check_uint8_clamp_1){
    ck_assert_float_eq(util_uint8Clamp(9, 6, 8), 8);
}
END_TEST

START_TEST(check_uint8_clamp_2){
    ck_assert_float_eq(util_uint8Clamp(5, 6, 8), 6);
}
END_TEST

START_TEST(check_uint8_clamp_3){
    ck_assert_float_eq(util_uint8Clamp(7, 6, 8), 7);
}
END_TEST

START_TEST(check_random_unit_disk){

    vec3 th = util_randomUnitDisk();

    ck_assert_ldouble_ge(th.x,-1.0);
    ck_assert_ldouble_le(th.x,1.0);

    ck_assert_ldouble_ge(th.y,-1.0);
    ck_assert_ldouble_le(th.y,1.0);

    CFLOAT temp = th.x*th.x + th.y*th.y;

    ck_assert_ldouble_lt(temp,1);

}END_TEST

Suite* util_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Util");

    //Core test case
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, check_util_h_1);
    tcase_add_test(tc_core, check_util_h_2);
    tcase_add_test(tc_core, check_util_h_3);
    tcase_add_test(tc_core, check_util_h_4);
    tcase_add_test(tc_core, check_util_h_5);
    tcase_add_test(tc_core, check_util_h_6);
    tcase_add_test(tc_core, check_util_h_7);
    tcase_add_test(tc_core, check_util_near_zero_corr);
    tcase_add_test(tc_core, check_util_near_zero_incorr);
    tcase_add_test(tc_core, check_uint8_clamp_1);
    tcase_add_test(tc_core, check_uint8_clamp_2);
    tcase_add_test(tc_core, check_uint8_clamp_3);
    tcase_add_test(tc_core, check_random_unit_disk);
    

    suite_add_tcase(s, tc_core);

    return s;
}

int main(void){
    srand(time(NULL));

    int number_failed;
    Suite* s;
    SRunner *sr;

    s = util_suite();
    sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);


    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


