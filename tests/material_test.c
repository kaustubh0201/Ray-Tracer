#define HYPATIA_IMPLEMENTATION
#include <stdlib.h>
#include <stdbool.h>
#include <tgmath.h>
#include <check.h>

#include "hypatiaINC.h"
#include "material.h"
#include "hitRecord.h"
#include "ray.h"
#include "testutils.h"
#include "color.h"

START_TEST(check_material_lambertian){
/*    LambertianMat t_lmat = {
        .albedo = {
            .r = 1.0,
            .g = 2.0,
            .b = 3.0
        }
    };

    Ray rayIn = {
        .origin = {
            .x = 0,
            .y = 0,
            .z = 0
        },

        .direction = {
            .x = -2.0,
            .y = -3.0,
            .z = -2.0
        }
    };

    Material ma = {
        .mat = &t_lmat,
        .matType = LAMBERTIAN,
    };

    HitRecord t_hr =  {
        .point = {
            .x = 4.0,
            .y = 3.0,
            .z = 2.0
        },
        .normal = {
            .x = -1.0,
            .y = 3.0,
            .z = 2.0
        },
        .distanceFromOrigin = 2.0,
        
        .frontFace = true,
        .valid = true,
        .hitObjMat = &ma
    };

    Ray out = {
        .origin = {
            .x = 0,
            .y = 0,
            .z = 0
        },
        .direction = {
            .x = 0,
            .y = 0,
            .z = 0
        }
        
    };

    RGBColorF attenuation = {
        .r = 0.0,
        .g = 0.0,
        .b = 0.0
    };

    //random_numbers = x = 0.37963565706642355, y = 0.87001058857811875, z = 0.31457645119382399
    
    bool b = mat_scatter(&rayIn, &t_hr, &attenuation, &out);

    // (-0.1362891883338393, 0.850211021907733, 0.5084943218659496)
    Ray expected_ray = {
        .origin = {
            .x = 4.0,
            .y = 3.0,
            .z = 2.0
        },
        .direction = {
        .x = -0.1362891883338393,
        .y = 0.850211021907733,
        .z = 0.5084943218659496
        }   
    };

    RGBColorF exp_attenuation = {
        .r = 1.0,
        .g = 2.0,
        .b = 3.0
    };

    ck_assert_ld_vec3_eq(expected_ray.origin, out.origin);
    ck_assert_ld_vec3_eq(expected_ray.direction, out.direction);
    ck_assert_colorf_eq(exp_attenuation, t_lmat.albedo);
    ck_assert_int_eq(b, 1);*/
}
END_TEST

START_TEST (check_material_metal_true){

    MetalMat nmetalMat = {
        .albedo = {
            .r = 1.0,
            .g = 2.0,
            .b = 3.0
        },
        .fuzz = 0.2
    };

    Ray rayIn = {
        .origin = {
            .x = 0,
            .y = 0,
            .z = 0
        },

        .direction = {
            .x = -0.48507125007266594,
            .y = -0.7276068751089989,
            .z = -0.48507125007266594
        }
    };
    // dot_prod = -0.198029508595334818600171328015662
    // 2*dot_prod = -0.396059017190669637200342656031324
    // normal * 2*dot_prod = {0.1616904166908886, -0.3233808333817773, 0.1616904166908886}
    // d - n = (-0.6467616667635545, -0.404226041727222, -0.6467616667635545)
    
    // 0.1980295085953345561950357848837

    // cnst rndm_nbrs = {0.33522275571488902, 0.768229594811904, 0.27777471080318777}
    

    // fuzz * rndm = {0.067044551142977804, 0.1536459189623808, 0.055554942160637554}
    // above + reflected = (-0.5797171156205767, -0.250580122764841, -0.5912067246029169)
    // nrml = (-0.670120332634210, -0.289656507793361, -0.683401673456519)
    
    Material ma = {
            .mat = &nmetalMat,
            .matType = METAL
    };

    HitRecord rec = {
        .point = {
            .x = 4.0,
            .y = 3.0, 
            .z = 2.0
        },
        
        .normal = {
            .x = -0.4082482904638631,
            .y = 0.8164965809277261,
            .z = -0.4082482904638631
        },
        
        .distanceFromOrigin = 3.0,

        .frontFace = true,
        .valid = true,
        .hitObjMat = &ma
    };

    RGBColorF attenuation = {
        .r = 0,
        .g = 0,
        .b = 0
    };

    Ray out = {
        .origin = {
            .x = 0,
            .y = 0,
            .z = 0
        },

        .direction = {
            .x = 0,
            .y = 0,
            .z = 0
        }
    };

    Ray exp_out = {

        .origin = {
            .x = 4.0,
            .y = 3.0, 
            .z = 2.0 
        },
        .direction = {
            .x = -0.670120332634210,
            .y = -0.289656507793361,
            .z = -0.683401673456519
        }
    };

    RGBColorF exp_attenuation = {
        .r = 1.0,
        .g = 2.0,
        .b = 3.0
    };


    bool b = mat_scatter(&rayIn, &rec, &attenuation, &out);
    
    ck_assert_colorf_eq(exp_attenuation, attenuation);
    ck_assert_ld_vec3_eq(exp_out.origin, out.origin);
    ck_assert_ld_vec3_eq(exp_out.direction, out.direction);
    ck_assert_int_eq(1, b);


}END_TEST

START_TEST (check_material_metal_false) {

    MetalMat nmetalMat = {
        .albedo = {
            .r = 1.0,
            .g = 2.0,
            .b = 3.0
        },
        .fuzz = 0.3
    };

    Ray rayIn = {
        .origin = {
            .x = 0,
            .y = 0,
            .z = 0
        },

        .direction = {
            .x = -0.48507125007266594,
            .y = -0.7276068751089989,
            .z = -0.48507125007266594
        }
    };

    // 2*dot_prod = 1.980295085953348833406939841573836
    // n * 2_dot_prod = {-0.8084520834544435, -1.616904166908887, -0.8084520834544435}
    // d - n = (0.3233808333817776, 0.889297291799888, 0.3233808333817776)
    // normalized = (0.323380833381777, 0.88929729179989, 0.323380833381777)

    // cnst rndm_nbrs = {0.33522275571488902, 0.768229594811904, 0.27777471080318777}
    // fuzz*rndm = {0.100566826714466706, 0.2304688784435712, 0.083332413240956331}
    // above + reflected = (0.423947660096244, 1.11976617024346, 0.406713246622733)
    // nrml = (0.335262370437306, 0.88552313388426, 0.321633210854460)

    Material ma = {
        .mat = &nmetalMat,
        .matType = METAL
    };

    HitRecord rec = {
        .point = {
            .x = 4.0,
            .y = 3.0, 
            .z = 2.0
        },
        
        .normal = {
            .x = -0.4082482904638631,
            .y = -0.8164965809277261,
            .z = -0.4082482904638631
        },
        
        .distanceFromOrigin = 3.0,
        .frontFace = true,
        .valid = true,
        .hitObjMat = &ma
    };

    RGBColorF attenuation = {
        .r = 0,
        .g = 0,
        .b = 0
    };

    Ray out = {
        .origin = {
            .x = 0,
            .y = 0,
            .z = 0
        },

        .direction = {
            .x = 1.0,
            .y = 4.0,
            .z = 3.0
        }
    };

    Ray exp_out = {

        .origin = {
            .x = 4.0,
            .y = 3.0, 
            .z = 2.0 
        },

        .direction = {
            .x = 0.335262370437306,
            .y = 0.88552313388426,
            .z = 0.321633210854460
        }
    };

    RGBColorF exp_attenuation = {
        .r = 1.0,
        .g = 2.0,
        .b = 3.0
    };

    bool b = mat_scatter(&rayIn, &rec, &attenuation, &out);
    
    ck_assert_colorf_eq(exp_attenuation, attenuation);
    ck_assert_ld_vec3_eq(exp_out.origin, out.origin);
    ck_assert_ld_vec3_eq(exp_out.direction, out.direction);
    ck_assert_int_eq(0, b);


    
}END_TEST


// 2*dot_prod = 1.980295085953348833406939841573836
    // n * 2_dot_prod = {-0.8084520834544435, -1.616904166908887, -0.8084520834544435}
    // d - n = (0.3233808333817776, 0.889297291799888, 0.3233808333817776)
    // normalized = (0.323380833381777, 0.88929729179989, 0.323380833381777)

    // cnst rndm_nbrs = {0.33522275571488902, 0.768229594811904, 0.27777471080318777}
    // fuzz*rndm = {0.100566826714466706, 0.2304688784435712, 0.083332413240956331}
    // above + reflected = (0.423947660096244, 1.11976617024346, 0.406713246622733)
    // nrml = (0.335262370437306, 0.88552313388426, 0.321633210854460)

START_TEST(check_material_dielectric_reflect){

    DielectricMat ndielectricMat = {
        .ir = 1.5
    };

    Ray rayIn = {
        .origin = {
            .x = 0,
            .y = 0,
            .z = 0
        },

        .direction = {
            .x = -0.48507125007266594,
            .y = -0.7276068751089989,
            .z = -0.48507125007266594
        }
    };

    Material ma = {
        .mat = &ndielectricMat,
        .matType = DIELECTRIC
    };

    HitRecord rec = {
        .point = {
            .x = 4.0,
            .y = 3.0, 
            .z = 2.0
        },

        .normal = {
            .x = -0.4082482904638631,
            .y = -0.8164965809277261,
            .z = -0.4082482904638631
        },

        .distanceFromOrigin = 3.0,
        .frontFace = false,
        .valid = true,
        .hitObjMat = &ma
    };

    // uv = {-0.48507125007266594, -0.7276068751089989, -0.48507125007266594}
    // n = {-0.4082482904638631, -0.8164965809277261, -0.4082482904638631}
    // dot = -0.990147542976674416703469920786918
    // cos_theta = -0.990147542976674416703469920786918
    // n * cos_theta = {0.4042260417272217, 0.8084520834544434, 0.4042260417272217}
    
    // abv + uv = (-0.0808452083454442, 0.080845208345444, -0.0808452083454442)
    // abv * et_ea = {-0.1212678125181663, 0.121267812518166, -0.1212678125181663}
    // length_squared = 0.044117783472
    // 1 - abv = 0.955882216528
    // -sqrt = -0.977692291331
    // abv * n = {0.399141206536, 0.798282413071, 0.399141206536}
    // r_out_parallel + r_out+perp = (0.27787342249795688, 0.91955028255041327, 0.27787342249795688)
    // // cos*cos = 0.9803921568627453110203418861192663315222407256317286856323599387
    // 1 - abv = 0.0196078431372546889796581138807336684777592743682713143676400613
    // sin = 0.140028008402800219858194714742080986597179923186757330782312116
    // refraction_ratio * sin = 0.210042012604200329787292072113121479895769884780135996173468174
    
    //reflactance function used here-on
    // r0 = -0.2
    // r0*r0 = 0.0400000000000000
    // 0.96 + 0.0400000000000000 * 31.219530949541927
    // 2.208781237981677

    
    RGBColorF attenuation = {
        .r = 0,
        .g = 0,
        .b = 0
    };

    Ray out = {
        .origin = {
            .x = 0,
            .y = 0,
            .z = 0
        },

        .direction = {
            .x = 1.0,
            .y = 4.0,
            .z = 3.0
        }
    };

    Ray exp_out = {

        .origin = {
            .x = 4.0,
            .y = 3.0, 
            .z = 2.0 
        },

        .direction = {
            .x = 0.323380833381777,
            .y = 0.88929729179989,
            .z = 0.323380833381777
        }

        
    };

    RGBColorF exp_attenuation = {
        .r = 1.0,
        .g = 1.0,
        .b = 1.0
    };


    bool b = mat_scatter(&rayIn, &rec, &attenuation, &out);

    ck_assert_int_eq(1, b);
    ck_assert_colorf_eq(exp_attenuation, attenuation);
    ck_assert_ld_vec3_eq(exp_out.origin, out.origin);
    ck_assert_ld_vec3_eq(exp_out.direction, out.direction);

}END_TEST



START_TEST(check_material_dielectric_refract){

    DielectricMat ndielectricMat = {
        .ir = 0.8
    };

    Ray rayIn = {
        .origin = {
            .x = 0,
            .y = 0,
            .z = 0
        },

        .direction = {
            .x = -0.3333333333333333,
            .y = -0.3333333333333333,
            .z = -0.3333333333333333
        }
    };

    Material ma = {
        .mat = &ndielectricMat,
        .matType = DIELECTRIC
    };

    HitRecord rec = {
        .point = {
            .x = 4.0,
            .y = 3.0, 
            .z = 2.0
        },

        .normal = {
            .x = 0.047619047619047616,
            .y = 0.09523809523809523,
            .z = 0.19047619047619047
        },

        .distanceFromOrigin = 3.0,
        .frontFace = false,
        .valid = true,
        .hitObjMat = &ma
    };

    // cos = 0.11111111111111 refractive_idx = 0.8 
    // after reflectance 0.56042366153742923
    // 0.84018771715470952
    // sin_theta = 0.99380798999990666
    // ratio * sin = 0.7950463919999253
    // uv = {-0.3333333333333333, -0.3333333333333333, -0.3333333333333333}
    // n = {0.047619047619047616, 0.09523809523809523, 0.19047619047619047}
    // etai_over_etat = 0.8
    // n * cos = {0.0052910052910052, 0.010582010582010, 0.021164021164021}
    // abv + uv = (-0.3280423280423281, -0.322751322751323, -0.312169312169312)
    // abv * etai_over = (-0.26243386243386248, -0.25820105820105843, -0.2497354497354496)
    // 0.06236779485456727368662691414016 + 0.0666677864561463627320623722740649 + 0.0688715321519554565948321715517504
    // abv.length_squared = 0.1979071134626690930135214579659753
    // 1 - abv = 0.8020928865373309069864785420340247
    // sqrt(abv) = 0.8955963859559343414905968366503420
    // r_perp_len = -0.8955963859559343414905968366503420
    // abv * n = {-0.042647446950282585, -0.08529489390056517, -0.17058978780113035}
    // res = (-0.30508130938414506, -0.3434959521016236, -0.4203252375365800)

    RGBColorF attenuation = {
        .r = 0,
        .g = 0,
        .b = 0
    };

    Ray out = {
        .origin = {
            .x = 0,
            .y = 0,
            .z = 0
        },

        .direction = {
            .x = 1.0,
            .y = 4.0,
            .z = 3.0
        }
    };

    Ray exp_out = {

        .origin = {
            .x = 4.0,
            .y = 3.0, 
            .z = 2.0 
        },

        .direction = {
            .x = -0.30508130938414506,
            .y = -0.3434959521016236,
            .z = -0.4203252375365800
        }        
    };

    RGBColorF exp_attenuation = {
        .r = 1.0,
        .g = 1.0,
        .b = 1.0
    };


    bool b = mat_scatter(&rayIn, &rec, &attenuation, &out);

    ck_assert_int_eq(1, b);
    ck_assert_colorf_eq(exp_attenuation, attenuation);
    ck_assert_ld_vec3_eq(exp_out.origin, out.origin);
    ck_assert_ld_vec3_eq(exp_out.direction, out.direction);

}END_TEST

/*
START_TEST(check_material_create_lambertian)
{
    LambertianMat lm = {
        .albedo = {.r = 0.5, .b = .03, .g = 0.7}
    };


    Material m1 = MAT_CREATE_LAMB_IP(&lm);
    Material m2;
    m2 = MAT_CREATE_LAMB(&lm);
    
    ck_assert_ptr_eq(m2.mat, &lm);
    ck_assert_int_eq(m2.matType, LAMBERTIAN);

    ck_assert_ptr_eq(m1.mat, &lm);
    ck_assert_int_eq(m1.matType, LAMBERTIAN);

}
END_TEST
*/

START_TEST(check_material_create_metal)
{
    MetalMat mm = {
        .albedo = {.r = 0.5, .b = .03, .g = 0.7},
        .fuzz = 0.3
    };


    Material m1 = MAT_CREATE_METAL_IP(&mm);
    Material m2;
    m2 = MAT_CREATE_METAL(&mm);
    
    ck_assert_ptr_eq(m2.mat, &mm);
    ck_assert_int_eq(m2.matType, METAL);

    ck_assert_ptr_eq(m1.mat, &mm);
    ck_assert_int_eq(m1.matType, METAL);

}
END_TEST

START_TEST(check_material_create_dielectric)
{
    DielectricMat dm = {
        .ir = 0.3
    };


    Material m1 = MAT_CREATE_DIELECTRIC_IP(&dm);
    Material m2;
    m2 = MAT_CREATE_DIELECTRIC(&dm);
    
    ck_assert_ptr_eq(m2.mat, &dm);
    ck_assert_int_eq(m2.matType, DIELECTRIC);

    ck_assert_ptr_eq(m1.mat, &dm);
    ck_assert_int_eq(m1.matType, DIELECTRIC);

}
END_TEST


Suite* util_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Material");

    //Core test case
    tc_core = tcase_create("Core");
    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, check_material_lambertian);
    tcase_add_test(tc_core, check_material_metal_true);
    tcase_add_test(tc_core, check_material_metal_false);
    tcase_add_test(tc_core, check_material_dielectric_reflect);
    tcase_add_test(tc_core, check_material_dielectric_refract);
    //tcase_add_test(tc_core, check_material_create_lambertian);
    tcase_add_test(tc_core, check_material_create_metal);
    tcase_add_test(tc_core, check_material_create_dielectric);


    return s;
}

int main(void){
    srand(1);

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
