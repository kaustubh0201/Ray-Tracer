#include <assert.h>
#include <tgmath.h>

#include "material.h"
#include "util.h"
#include "ray.h"
#include "types.h"

static CFLOAT reflectance(CFLOAT cosine, CFLOAT ref_idx){
    //using schlik's approximation
    CFLOAT r0 = (1-ref_idx)/(1+ref_idx);
    r0 *= r0;

    return r0 + (1-r0) * pow((1 - cosine), 5);
}

static vec3 util_vec3Refract(vec3 uv, vec3 n, CFLOAT etai_over_etat){
    // negate the incoming ray
    vec3 negate_uv = uv;
    vector3_negate(&negate_uv);
    
    // finding cos-theta for uv and n
    CFLOAT cos_theta = CF_MIN(vector3_dot_product(&negate_uv, &n), 1.0);
    
    
    vec3 r_out_perp = n;    
    // multiply normal with cos 
    vector3_multiplyf(&r_out_perp, cos_theta);
    // add with above
    vector3_add(&r_out_perp, &uv);
    vector3_multiplyf(&r_out_perp, etai_over_etat);
    
    CFLOAT r_out_perp_length_squared = (r_out_perp.x*r_out_perp.x + r_out_perp.y*r_out_perp.y + r_out_perp.z*r_out_perp.z);

    r_out_perp_length_squared = 1.0 - r_out_perp_length_squared;
    r_out_perp_length_squared = fabs(r_out_perp_length_squared);
    r_out_perp_length_squared = - sqrt(r_out_perp_length_squared);

    vec3 r_out_parallel = n;
    vector3_multiplyf(&r_out_parallel, r_out_perp_length_squared);

    vector3_add(&r_out_perp, &r_out_parallel);

    return r_out_perp;
}

static bool mat_metalScatter(const MetalMat * restrict nmetalMat, 
                             const Ray * restrict rayIn, 
                             const HitRecord * restrict rec, 
                             RGBColorF * restrict attenuation, Ray * restrict out){
    
    vec3 direction = rayIn->direction;
        
    vec3 reflected = util_vec3Reflect(direction, rec->normal);

    vec3 fuzz_rndm = util_randomUnitSphere();

    CFLOAT fuzz = nmetalMat->fuzz;

    fuzz = util_floatClamp(fuzz, 0, 1.0);

    vector3_multiplyf(&fuzz_rndm, fuzz);

    vector3_add(&fuzz_rndm, &reflected);

    *out = ray_create(rec->point, fuzz_rndm);

    *attenuation = nmetalMat->albedo;

    CFLOAT dot_prod = vector3_dot_product(&out->direction, &rec->normal);

    return (dot_prod > 0);
}

static bool mat_lambScatter(const LambertianMat * restrict nlambMat, 
                            const HitRecord * restrict rec, 
                            RGBColorF * restrict attenuation, Ray * restrict out){
    
    vec3 scatter_direction = rec->normal;
    vec3 rndm = util_randomUnitVector();

    vector3_add(&scatter_direction, &rndm);

    if(util_isVec3Zero(scatter_direction)){
        scatter_direction = rec->normal;
    }

    *out = ray_create(rec->point,scatter_direction);

    *attenuation = tex_value(&nlambMat->lambTexture, rec->u, rec->v, rec->point); 

    return true;
}

static bool mat_dielectricScatter(const DielectricMat * restrict ndielectricMat, const Ray * restrict rayIn, const HitRecord * restrict rec, RGBColorF * restrict attenuation, Ray * restrict out ){
     
    *attenuation = (RGBColorF){
        .r = 1.0,
        .g = 1.0,
        .b = 1.0
    };

    CFLOAT refraction_ratio = rec->frontFace ? (1.0/ndielectricMat->ir) : ndielectricMat->ir;
    
    vec3 negate_rayIn_direction = rayIn->direction;
    vector3_negate(&negate_rayIn_direction);
    CFLOAT cos_theta = CF_MIN(vector3_dot_product(&negate_rayIn_direction, &rec->normal), 1.0);
    CFLOAT sin_theta = sqrt(1 - cos_theta*cos_theta);

    bool cannot_refract = (refraction_ratio * sin_theta) > 1.0;
    vec3 direction;

    if(cannot_refract || reflectance(cos_theta, refraction_ratio) > util_randomFloat(0.0, 1.0)){
        direction = util_vec3Reflect(rayIn->direction, rec->normal);
    }else{
        direction = util_vec3Refract(rayIn->direction, rec->normal, refraction_ratio);
    }

    Ray r = {
        .origin = rec->point,
        .direction = direction
    };

    *out = r;

    return true;
}


bool mat_scatter (const Ray * restrict rayIn, const HitRecord * restrict rec, RGBColorF * restrict attenuation, Ray * restrict out){
    const Material * restrict m = rec->hitObjMat;

    if(m->matType == METAL){
        return mat_metalScatter((MetalMat * ) m->mat, rayIn, rec, attenuation, out);
    }else if(m->matType == DIELECTRIC){
        return mat_dielectricScatter((DielectricMat * ) m->mat, rayIn, rec, attenuation, out);
    }else if (m->matType == LAMBERTIAN){
        return mat_lambScatter((LambertianMat * ) m->mat, rec, attenuation, out);
    }

    return false;
}


