//===========================================================================
// typedefs.h
//===========================================================================
// @brief: This header defines the shorthand of several ap_uint data types.

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include "ap_int.h"
#include "ap_fixed.h"
#include <stdint.h>

#ifndef DEC_BITS
#define DEC_BITS 2 // Number of decimal bits for fixed point representation
#define TOT_WIDTH DEC_BITS+3
#endif

typedef ap_uint<2> bit2_t;
typedef ap_uint<4> bit4_t;
typedef ap_uint<6> bit6_t;
typedef ap_uint<8> bit8_t;
typedef ap_uint<32> bit32_t;
typedef ap_uint<64> bit64_t;

#ifdef FIXED_TYPE // Fixed-point settings
typedef ap_ufixed<TOT_WIDTH, 3> feature_type;
extern "C" { union FeatureUnion {ap_ufixed<TOT_WIDTH, 3> fval; int32_t ival;}; }
#else // Floating-point settings
extern "C" { union FeatureUnion {float fval; int32_t ival;}; }
typedef float feature_type;
#endif



#endif
