#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>
#include "types.h"

typedef struct rgbColorU8 {

    uint8_t r;

    uint8_t g;

    uint8_t b;

} RGBColorU8;

typedef struct rgbColorF {

    CFLOAT r;

    CFLOAT g;

    CFLOAT b;

} RGBColorF;




extern RGBColorU8 coloru8_create(uint8_t r, uint8_t g, uint8_t b);
// a
extern RGBColorU8 coloru8_createf(CFLOAT r, CFLOAT g, CFLOAT b);

extern RGBColorF colorf_create(CFLOAT r, CFLOAT g, CFLOAT b);
extern RGBColorF convertU8toF(RGBColorU8 in);

extern RGBColorU8 convertFtoU8(RGBColorF in);

// a
extern RGBColorF colorf_multiply(RGBColorF x, RGBColorF y);
extern RGBColorF colorf_add(RGBColorF x, RGBColorF y);

#define COLOR_U8CREATE(r, g, b) _Generic((r), uint8_t: coloru8_create((r), (g), (b)), CFLOAT: coloru8_createf((r), (g), (b)))

#endif