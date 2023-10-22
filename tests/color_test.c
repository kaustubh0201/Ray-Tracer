#include <stdlib.h>
#include <check.h>

#define HYPATIA_IMPLEMENTATION

#include "color.h"
#include "testutils.h"
#include "hypatiaINC.h"

START_TEST(check_uint8_create_color){

    RGBColorU8 color = coloru8_create(232, 134, 178);

    RGBColorU8 exp_color = {
        .r = 232,
        .g = 134, 
        .b = 178
    };

    ck_assert_coloru8_eq(color, exp_color);

}END_TEST



START_TEST(check_f_create_color){

    RGBColorF color = colorf_create(0.5, 0.3, 0.9);

    RGBColorF exp_color = {
        .r = 0.5,
        .g = 0.3,
        .b = 0.9
    };  

    ck_assert_colorf_eq(color, exp_color);

}END_TEST

START_TEST(convert_u8_f){

    RGBColorU8 in = {
        .r = 145,
        .g = 234,
        .b = 78
    };
    
    RGBColorF exp_c = {
        .r = 0.56640625,
        .g = 0.9140625,
        .b = 0.3046875
    };

    RGBColorF out = convertU8toF(in);

    ck_assert_colorf_eq(exp_c, out);

}END_TEST

START_TEST(convert_f_u8){

    RGBColorF in = {
        .r = 0.5,
        .g = 0.3,
        .b = 1
    };

    RGBColorU8 out = convertFtoU8(in);

    RGBColorU8 exp_c = {
        .r = 128,
        .g = 76,
        .b = 255
    };

    ck_assert_coloru8_eq(exp_c, out);

}END_TEST

START_TEST(colorfMultiply){
    RGBColorF in_x = {
        .r = 0.5,
        .g = 0.3,
        .b = 1
    };

    RGBColorF in_y = {
        .r = 0.6,
        .g = 0.4,
        .b = 2
    };

    RGBColorF exp = {
        .r = 0.3,
        .g = 0.12,
        .b = 2 
    };

    RGBColorF out = colorf_multiply(in_x,in_y);

    ck_assert_colorf_eq(exp, out);


}END_TEST

START_TEST(colorfAdd){
    RGBColorF in_x = {
        .r = 0.5,
        .g = 0.3,
        .b = 1
    };

    RGBColorF in_y = {
        .r = 0.6,
        .g = 0.4,
        .b = 0.9
    };

    RGBColorF exp = {
        .r = 1.0,
        .g = 0.7,
        .b = 1.0
    };

    RGBColorF out = colorf_add(in_x, in_y);
    
    ck_assert_colorf_eq(exp, out);
    

}END_TEST

START_TEST(coloru8Createf){

    RGBColorU8 out = coloru8_createf(0.8, 0.5, 0.2);

    RGBColorU8 exp_out = {
        .r = 204,
        .g = 128,
        .b = 51
    };

    ck_assert_coloru8_eq(exp_out, out);

}END_TEST


START_TEST(COLORU8CREATE){

    uint8_t a = 232;
    uint8_t b = 134;
    uint8_t c = 178;

    RGBColorU8 color = COLOR_U8CREATE(a, b, c);

    RGBColorU8 exp_color = {
        .r = 232,
        .g = 134, 
        .b = 178
    };

    RGBColorU8 out = COLOR_U8CREATE(0.8, 0.5, 0.2);

    RGBColorU8 exp_out = {
        .r = 204,
        .g = 128,
        .b = 51
    };

    ck_assert_coloru8_eq(color, exp_color);
    ck_assert_coloru8_eq(out, exp_out);

}END_TEST

Suite* hr_suite(void){
    Suite *s;
    TCase *tc_core;

    s = suite_create("Camera");

    //Core test case
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, check_uint8_create_color);
    tcase_add_test(tc_core, check_f_create_color);
    tcase_add_test(tc_core, convert_u8_f);
    tcase_add_test(tc_core, convert_f_u8);
    tcase_add_test(tc_core, colorfMultiply);
    tcase_add_test(tc_core, colorfAdd);
    tcase_add_test(tc_core, coloru8Createf);
    tcase_add_test(tc_core, COLORU8CREATE);
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