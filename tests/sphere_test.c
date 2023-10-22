#define HYPATIA_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include <float.h>
#include "hypatiaINC.h"
#include "sphere.h"
#include "testutils.h"
#include "ray.h"
#include "allocator.h"
#include "material.h"

//discriminant < 0 so no intersection
START_TEST(check_sphere_1){
   
    Sphere test_sphere = {
        .center = {
            .x = 2.0,
            .y = 4.0,
            .z = 7.0
        },
        .radius = 5.0
    };

    vec3 origin = {
        .x = 0.0,
        .y = -20.0,
        .z = 0.0
    };    

    vec3 direction = {
        .x = 72.1,
        .y = 33.03,
        .z = 0.0
    };

    Ray r = ray_create(origin, direction);
    
    HitRecord t_hr;
    obj_sphereHit(&test_sphere, r, 0.001, F_MAX, &t_hr);
    ck_assert_int_eq(t_hr.valid, 0);
}
END_TEST

// ray originates from outside and hits the sphere
START_TEST(check_sphere_2){
    
    Sphere t_sphere = {
        .center = {
            .x = -2.0,
            .y = -4.0,
            .z = -7.0
        },
        .radius = 3.0
    };
    
    vec3 origin = {
        .x = 0.0,
        .y = 0.0,
        .z = 0.0
    }; 
       
    vec3 direction = {
        .x = -2.0,
        .y = -4.0,
        .z = -7.0
    };

    Ray r = ray_create(origin, direction);

    HitRecord t_hr;
    obj_sphereHit(&t_sphere, r, 0.001, F_MAX, &t_hr);

    vec3 exp_point = {
        .x = -1.277684881485385,
        .y = -2.55536976297077,
        .z = -4.471897085198847
    };
    
    vec3 exp_normal = direction;
    vector3_normalize(&exp_normal);
    vector3_negate(&exp_normal);

    ck_float_equal(5.30662386291807, t_hr.distanceFromOrigin);

    ck_assert_int_eq(1, t_hr.valid);
    ck_assert_ld_vec3_eq(exp_point, t_hr.point);
    ck_assert_ld_vec3_eq(exp_normal, t_hr.normal);
    ck_assert_int_eq(t_hr.frontFace, 1);
}
END_TEST

// ray coming from outside - root < t_min
START_TEST(check_sphere_3){

    Sphere t_sphere = {
        .center = {
            .x = -2.0,
            .y = -4.0,
            .z = -7.0
        },
        .radius = 3.0
    };

    vec3 origin={
        .x = 0.0,
        .y = 0.0,
        .z = 0.0
    };
    vec3 direction={
        .x = -2.0,
        .y = -4.0,
        .z = -7.0

    };

    Ray r = ray_create(origin, direction);

    HitRecord t_hr;
    obj_sphereHit(&t_sphere, r, 100.0, F_MAX, &t_hr);
    ck_assert_int_eq(0 ,t_hr.valid);
   
}
END_TEST

// ray coming from outside - root > t_max 
START_TEST(check_sphere_4){

    Sphere t_sphere = {
        .center = {
            .x = -2.0,
            .y = -4.0,
            .z = -7.0
        },        
        .radius = 3.0
    };

    vec3 origin = {
        .x = 0.0,
        .y = 0.0,
        .z = 0.0
    };
    
    vec3 direction = {
        .x = -2.0,
        .y = -4.0,
        .z = -7.0
    };

    Ray r = ray_create(origin, direction);

    HitRecord t_hr;
    obj_sphereHit(&t_sphere, r, 0.001, 4.0, &t_hr);
    
    ck_assert_int_eq(0 ,t_hr.valid);
}
END_TEST

// ray originates from within the sphere
START_TEST(check_sphere_5){

    Sphere t_sphere = {
        .center = {
            .x = -2.0,
            .y = -4.0,
            .z = -7.0
        },
        .radius = 3.0
    };

    vec3 origin={
        .x = -2.0,
        .y = -4.0,
        .z = -7.0
    };
    
    vec3 direction = {
        .x = -1.0,
        .y = -2.0,
        .z = -2.0
    };
    
    Ray r = ray_create(origin, direction);

    HitRecord t_hr;
    obj_sphereHit(&t_sphere, r, 0.001, F_MAX, &t_hr);
    
    vec3 exp_point = {
        .x = -3.0,
        .y = -6.0,
        .z = -9.0
    };

    vec3 exp_normal = direction;
    vector3_normalize(&exp_normal);
    vector3_negate(&exp_normal);

    ck_float_equal(3.0, t_hr.distanceFromOrigin);
    ck_assert_int_eq(1 ,t_hr.valid);
    
    ck_assert_ld_vec3_eq(exp_point, t_hr.point);
    ck_assert_ld_vec3_eq(exp_normal, t_hr.normal);
    ck_assert_int_eq(t_hr.frontFace, 0);
}
END_TEST

// ray originates inside the sphere and and root < t_min
START_TEST(check_sphere_6){

    Sphere t_sphere = {
        .center = {
            .x = -2.0,
            .y = -4.0,
            .z = -7.0 
        },
        .radius = 3.0
    };

    vec3 origin={
        .x = -3.95,
        .y = -3.48,
        .z = -7.96
    };
    vec3 direction = {
        .x = -0.36,
        .y = -2.0,
        .z = -2.17
    };
    
    Ray r = ray_create(origin, direction);

    HitRecord t_hr;
    obj_sphereHit(&t_sphere, r, 10000.0, F_MAX, &t_hr);

    ck_assert_int_eq(0 ,t_hr.valid);
    
}
END_TEST


// ray originates inside the sphere and and root > t_max
START_TEST(check_sphere_7){

    Sphere t_sphere = {
        .center = {
            .x = -2.0,
            .y = -4.0,
            .z = -7.0 
        },
        .radius = 3.0
    };

    vec3 origin = {
        .x = -3.95,
        .y = -3.48,
        .z = -7.96
    };

    vec3 direction = {
        .x = -0.36,
        .y = -2.00,
        .z = -2.17
    };

    Ray r = ray_create(origin, direction);

    HitRecord t_hr;
    obj_sphereHit(&t_sphere, r, 0.001, 1.0, &t_hr);

    ck_assert_int_eq(t_hr.valid, 0);

    
}
END_TEST
/*
START_TEST(check_createObjectLL){

    DynamicStackAlloc * d = alloc_createDynamicStackAllocD(1024, 10);

    ObjectLL * o = obj_createObjectLL(d);

    int expected_numObjects = 0;
    ObjectLLNode * expected_head = NULL;
    int expected_valid = 1;

    ck_assert_int_eq(expected_numObjects, o->numObjects);
    ck_assert_objectLLNode_eq(expected_head, o->head);
    ck_assert_int_eq(expected_valid, o->valid);

    alloc_freeDynamicStackAllocD(d);

}END_TEST
*/
/*
START_TEST(check_objectLLAdd){

    DynamicStackAlloc * da0 = alloc_createDynamicStackAlloc(1024, 10);
    DynamicStackAlloc * da1 = alloc_createDynamicStackAlloc(1024, 100);

    ObjectLL * o = alloc_dynamicStackAllocAllocate(da0, sizeof(ObjectLL), alignof(ObjectLL));
    o->numObjects = 0;
    o->head = NULL;
    o->valid = true;

    Sphere s = {
        .center = {
            .x = 0,
            .y = 1,
            .z = 0
        },

        .radius = 1.0,


    };

    alloc_freeDynamicStackAllocD(d0);
    alloc_freeDynamicStackAllocD(d1);
}
*/

Suite* sphere_suite(void){
    Suite *s;
    TCase *tc_core;

    s = suite_create("sphere");

    //Core test case
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, check_sphere_1);
    tcase_add_test(tc_core, check_sphere_2);
    tcase_add_test(tc_core, check_sphere_3);
    tcase_add_test(tc_core, check_sphere_4);
    tcase_add_test(tc_core, check_sphere_5);
    tcase_add_test(tc_core, check_sphere_6);
    tcase_add_test(tc_core, check_sphere_7);
    //tcase_add_test(tc_core, check_createObjectLL);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void){

    int number_failed;
    Suite* s;
    SRunner *sr;

    s = sphere_suite();
    sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);


    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

