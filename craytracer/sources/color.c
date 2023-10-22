#include "color.h"
#include "util.h"

#include <tgmath.h>

RGBColorU8 coloru8_create(uint8_t r, uint8_t g, uint8_t b){
    return (RGBColorU8){
        .r = util_uint8Clamp(r, 0, 255),
        .g = util_uint8Clamp(g, 0, 255),
        .b = util_uint8Clamp(b, 0, 255)
    };
}

RGBColorF colorf_create(CFLOAT r, CFLOAT g, CFLOAT b){

    return (RGBColorF){
        .r = util_floatClamp(r, 0, 1.0),
        .g = util_floatClamp(g, 0, 1.0),
        .b = util_floatClamp(b, 0, 1.0)
    };
}

RGBColorF colorf_add(RGBColorF x, RGBColorF y){
    
    return (RGBColorF){
        .r = util_floatClamp(x.r + y.r, 0, 1.0),
        .g = util_floatClamp(x.g + y.g, 0, 1.0),
        .b = util_floatClamp(x.b + y.b, 0, 1.0)
    };
}

RGBColorF colorf_multiply(RGBColorF x, RGBColorF y){
    return (RGBColorF) {
        .r = x.r * y.r,
        .g = x.g * y.g,
        .b = x.b * y.b
    };
}


RGBColorF convertU8toF(RGBColorU8 in){

    return (RGBColorF){
        .r = in.r/256.0,
        .g = in.g/256.0,
        .b = in.b/256.0
    };
}

RGBColorU8 coloru8_createf(CFLOAT r, CFLOAT g, CFLOAT b){
    return (RGBColorU8){
        .r = (uint8_t)(fmin(r * 256.00, 255.00)),
        .g = (uint8_t)(fmin(g * 256.00, 255.00)),
        .b = (uint8_t)(fmin(b * 256.00, 255.00))
    };
}

RGBColorU8 convertFtoU8(RGBColorF in){
    return (RGBColorU8){
        .r = (uint8_t)(fmin(in.r * 256.0, 255.0)),
        .g = (uint8_t)(fmin(in.g * 256.0, 255.0)),
        .b = (uint8_t)(fmin(in.b * 256.0, 255.0))
    };
}
