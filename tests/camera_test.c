#include <stdlib.h>

#define HYPATIA_IMPLEMENTATION

#include <check.h>
#include <stdbool.h>

#include "hypatiaINC.h"
#include "camera.h"
#include "types.h"
#include "testutils.h"


START_TEST(check_set_camera){
    // set camera
    Camera c = {0};

    vec3 ip_origin = {
        .x = 0.0,
        .y = 0.0,
        .z = 0.0
    };

    CFLOAT ip_aspect_ratio = 16.0/9.0;
    CFLOAT ip_focalLength = 5.0;
    CFLOAT ip_vfov = 10.0;
    

    cam_setCamera(&c, ip_origin, ip_aspect_ratio, ip_focalLength, ip_vfov);

    CFLOAT exp_aspect_ratio = 16.0/9.0;
    CFLOAT exp_focalLength = 5.0;    
    CFLOAT exp_vfov = 0.17453292519943295;
    

    CFLOAT exp_viewportHeight = 0.17497732705184801;
    CFLOAT exp_viewportWidth  = 0.3110708036477297;
    CFLOAT exp_verticalY   = 0.17497732705184801;
    CFLOAT exp_horizontalX = 0.3110708036477297;
    
    vec3 exp_lower_left_corner = {
        .x = -0.15553540182386488,
        .y = -0.08748866352592400, 
        .z = -5.0
    };
    
    vec3 exp_horizontal = {
        .x = exp_horizontalX,
        .y = 0.0,
        .z = 0.0
    };
    
    vec3 exp_vertical = {
        .x = 0.0,
        .y = exp_verticalY,
        .z = 0.0
    };
    
    ck_float_equal(exp_viewportWidth, c.viewportWidth);
    ck_float_equal(exp_aspect_ratio, c.aspectRatio);
    ck_float_equal(exp_focalLength, c.focalLength);
    ck_float_equal(exp_vfov, c.verticalFOV);
    ck_float_equal(exp_viewportHeight, c.viewportHeight);
    ck_assert_ld_vec3_eq(c.horizontal, exp_horizontal);
    ck_assert_ld_vec3_eq(c.vertical, exp_vertical);
    ck_assert_ld_vec3_eq(c.lowerLeftCorner, exp_lower_left_corner);
}
END_TEST 

START_TEST(check_camera_getRay){
    // camera get ray
      Camera c = {
        .origin = {
            .x = 0.0,
            .y = 0.0,
            .z = 0.0
        },

        .u = {
            .x = 0.3,
            .y = 0.4,
            .z = 0.9
        },

        .v = {
            .x = 0.5,
            .y = 0.2,
            .z = 0.9
        },

        .vertical = {
            .x = 0.0,
            .y = 0.17497732705184801,
            .z = 0.0
        },
        
        .horizontal = {
            .x = 0.3110708036477297, 
            .y = 0.0,
            .z = 0.0
        },

        .lowerLeftCorner = {
            .x = -0.15553540182386488,
            .y = -0.08748866352592400, 
            .z = -5.0
        },

        .lensRadius = 1.0
    };
    
    //{0.68037543430941905, -0.21123414636181392, 0}
    // u = 0.5, v = 0.6
    // cam.u = {0.3, 0.4, 0.9}
    // cam.v = {0.5, 0.2, 0.9}
    // rand.y * cam.v = {-0.10561707318090696, -0.042246829272362784, -0.190110731725632528}
    // rand.x * cam.u = {0.204112630292825715, 0.27215017372376762, 0.612337890878477145}
    // offset = (0.09849555711191876, 0.22990334445140484, 0.422227159152844617)
    // origin + abv = (0.09849555711191876, 0.22990334445140484, 0.422227159152844617)
    // veri*v = {0, 0.104986396231108806, 0}
    // hori*u = (0.15553540182386485,0,0)
    // lowerleft + veri*v + hori*u = {0, 0.017497732705184806, -5.0}
    // abv - origin = {0, 0.017497732705184806, -5.0}
    // abv - offset = (-0.09849555711191876, -0.21240561174622003, -5.422227159152844617)
    // norma(abv) = {-0.01814823680613573, -0.03913666213941406, -0.9990690482531294}

    // 
    vec3 exp_direction = {
        .x = -0.01814823680613573, 
        .y = -0.03913666213941406, 
        .z = -0.9990690482531294
    };
    // {v = {-0.018148236806135729, -0.039136662139414059, -0.99906904825312948}
    vec3 exp_origin = {
        .x = 0.0984955571119187, 
        .y = 0.2299033444514048, 
        .z = 0.4222271591528446
    };

    Ray t_ray = cam_getRay(&c, 0.5, 0.6);

    ck_assert_ld_vec3_eq(exp_direction, t_ray.direction);
    ck_assert_ld_vec3_eq(exp_origin, t_ray.origin);
    
}END_TEST

START_TEST(check_set_look_at_camera){
    Camera c = {0};

    vec3 ip_lookfrom = {
        .x = 3.0,
        .y = 3.0, 
        .z = 2.0
    };

    vec3 ip_lookat = {
        .x = 0.0, 
        .y = 0.0, 
        .z = -1.0
    };

    vec3 ip_up = {
        .x = 0.0, 
        .y = 1.0, 
        .z = 0.0
    };

    CFLOAT ip_vfov = 10.0;
    CFLOAT ip_aspect_ratio = 16.0/9.0;
    CFLOAT ip_aperture = 2.0;
    CFLOAT ip_focusDist = 5.19615242271;
        
    cam_setLookAtCamera(&c, ip_lookfrom, ip_lookat, ip_up, ip_vfov, ip_aspect_ratio, ip_aperture, ip_focusDist);
    
    CFLOAT exp_aspect_ratio = 16.0/9.0;
    CFLOAT exp_verticalFOV = 0.17453292519943295;
    CFLOAT exp_viewportHeight = 0.17497732705184801;
    CFLOAT exp_viewportWidth = 0.3110708036477297;


    // CFLOAT exp_vfov = 0.17453292519943295;
    // exp_viewportHeight = 0.17497732705184801
    // exp_viewportWidth  = 0.3110708036477297
    
    // w = normalize c-w = (0.5773502691896257, 0.5773502691896257, 0.5773502691896257)
    // cross(up, w) = (-0.5773502691896257, 0, 0.5773502691896257)
    // u = normalized = (0.707106781186548, 0, -0.707106781186548)
    // v = cross(w, u) = (0.408248290463863, -0.816496580927726, 0.408248290463863)
    // c->horizontal = {1.1429471142223925, 0, -1.1429471142223925}
    // {0.57147355711119625, 0, -0.57147355711119625}
    // c->vertical = {-0.3711829635370147, 0.7423659270740294, -0.3711829635370174}
    // {-0.1855914817685074, 0.3711829635370147, -0.1855914817685087}
    // horizontal + vertical = (0.7717641506853778, 0.7423659270740294, -1.5141300777594099)
    // (hor + ver) * -0.5 = {-0.3858820753426889, -0.3711829635370147, 0.75706503887970495}
    // w * focusDist = (3.0000000000019442,3.0000000000019442,3.0000000000019442)
    // lower_left = (-3.3858820753426889, -3.37118296354, -2.24293496112)
    // lens_radius = 1.0
    // {-0.38588207534463281, -0.37118296353895897, -0.24293496112224089}
    // // - (hori/2 + veri/2) = (-0.385882075342688, -0.3711829635370147, 0.7570650388797050)
    // // abv - w*focusDist = (-3.3858820753446330, -3.3711829635389589, -2.242934961122239)
    // ans = {-0.3858820753446330, -0.3711829635389589, -0.242934961122239}
    // (-0.3858820753426888, -0.3711829635370147, -0.242934961120295)

    // horizontal = {{v = {1.1429471142223919, 0, -1.1429471142223919}
    // vertical = {{v = {-0.37118296353701474, 0.74236592707402949, -0.37118296353701474}
    // hor + veri = (0.7717641506853772, 0.74236592707402949, -1.5141300777594066)
    // abv / 2 = {0.3858820753426886, 0.37118296353701474, -0.75706503887970330}
    // (3.3858820753446328, 3.3711829635389589, 2.2429349611222409)
    // origin = {3, 3, 2}
    // w * focusDist = (3.0000000000019442,3.0000000000019442,3.0000000000019442)
    // ans = {-0.3858820753446328, -0.3711829635389589, -0.2429349611222409}
    vec3 exp_w = {
        .x = 0.5773502691896257,
        .y = 0.5773502691896257,
        .z = 0.5773502691896257
    };

    vec3 exp_u = {
        .x = 0.707106781186548,
        .y = 0,
        .z = -0.707106781186548
    };

    vec3 exp_v = {
        .x = -0.408248290463863,
        .y = 0.816496580927726,
        .z = -0.408248290463863
    };

    vec3 vertical = {
        .x = -0.3711829635370147,
        .y = 0.7423659270740294,
        .z = -0.3711829635370174
    };

    vec3 horizontal = {
        .x = 1.1429471142223925,
        .y = 0,
        .z = -1.1429471142223925
    };

    CFLOAT exp_lens_radius = 1.0;

    vec3 exp_lower_left_corner = {
        .x = -0.3858820753446328,
        .y = -0.3711829635389589,
        .z = -0.2429349611222409
    };

    ck_assert_ld_vec3_eq(c.origin, ip_lookfrom);
    ck_assert_ld_vec3_eq(c.u, exp_u);
    ck_assert_ld_vec3_eq(c.w, exp_w);
    ck_assert_ld_vec3_eq(c.v, exp_v);    
    ck_assert_ld_vec3_eq(c.horizontal, horizontal);
    ck_assert_ld_vec3_eq(c.vertical, vertical);
    ck_float_equal(exp_aspect_ratio, c.aspectRatio);
    ck_float_equal(exp_lens_radius, c.lensRadius);
    ck_float_equal(exp_viewportWidth, c.viewportWidth);
    ck_float_equal(exp_viewportHeight, c.viewportHeight);        
    ck_float_equal(exp_verticalFOV, c.verticalFOV);
    ck_assert_ld_vec3_eq(c.lowerLeftCorner, exp_lower_left_corner);
}END_TEST

/*
{origin = {{v = {3, 3, 2}, {x = 3, y = 3, z = 2}, {yaw = 3, pitch = 3, roll = 2}}},
  lowerLeftCorner = {{v = {-0.38588207534463281, -0.37118296353895897, -0.24293496112224089}, {
        x = -0.38588207534463281, y = -0.37118296353895897, z = -0.24293496112224089}, {
        yaw = -0.38588207534463281, pitch = -0.37118296353895897, roll = -0.24293496112224089}}},
  horizontal = {{v = {1.1429471142223919, 0, -1.1429471142223919}, {x = 1.1429471142223919, y = 0,
        z = -1.1429471142223919}, {yaw = 1.1429471142223919, pitch = 0,
        roll = -1.1429471142223919}}}, vertical = {{v = {-0.37118296353701474, 0.74236592707402949,
        -0.37118296353701474}, {x = -0.37118296353701474, y = 0.74236592707402949,
        z = -0.37118296353701474}, {yaw = -0.37118296353701474, pitch = 0.74236592707402949,
        roll = -0.37118296353701474}}}, u = {{v = {0.70710678118654746, 0, -0.70710678118654746}, {
        x = 0.70710678118654746, y = 0, z = -0.70710678118654746}, {yaw = 0.70710678118654746,
        pitch = 0, roll = -0.70710678118654746}}}, v = {{v = {-0.40824829046386296,
        0.81649658092772592, -0.40824829046386296}, {x = -0.40824829046386296,
        y = 0.81649658092772592, z = -0.40824829046386296}, {yaw = -0.40824829046386296,
        pitch = 0.81649658092772592, roll = -0.40824829046386296}}}, w = {{v = {
        0.57735026918962573, 0.57735026918962573, 0.57735026918962573}, {x = 0.57735026918962573,
        y = 0.57735026918962573, z = 0.57735026918962573}, {yaw = 0.57735026918962573,
        pitch = 0.57735026918962573, roll = 0.57735026918962573}}},
  aspectRatio = 1.7777777777777777, focalLength = 0, lensRadius = 1,
  viewportHeight = 0.17497732705184801, viewportWidth = 0.31107080364772977,
  verticalFOV = 0.17453292519943295}
*/

Suite* hr_suite(void){
    Suite *s;
    TCase *tc_core;

    s = suite_create("Camera");

    //Core test case
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, check_set_camera);
    tcase_add_test(tc_core, check_camera_getRay);
    tcase_add_test(tc_core, check_set_look_at_camera);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void){
    srand(1);
    
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
