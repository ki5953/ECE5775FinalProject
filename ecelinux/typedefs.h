//===========================================================================
// typedefs.h
//===========================================================================
// @brief: This header defines the shorthand of several ap_uint data types.

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include "ap_int.h"
#include "ap_fixed.h"
#include <stdint.h>

typedef ap_uint<2> bit2_t;
typedef ap_uint<4> bit4_t;
typedef ap_uint<6> bit6_t;
typedef ap_uint<8> bit8_t;
typedef ap_uint<32> bit32_t;
typedef ap_uint<64> bit64_t;


extern "C" { union FeatureUnion {float fval; int32_t ival;}; }
typedef float feature_type;



#endif
