//===========================================================================
// iris.h
//===========================================================================
// @brief: This header file defines the interface for the core functions.

#ifndef IRIS_H
#define IRIS_H

#include<hls_stream.h>

#include "typedefs.h"
#include "training_data.h"
#include "math.h"
#include <string>

// Top function for synthesis
void dut(
    hls::stream<bit32_t> &strm_in,
    hls::stream<bit32_t> &strm_out,
    hls::stream<bit8_t>  &strm_train
);

// GNB Training function, given input feature map
void gnb_train( const feature_type features[TRAIN_SIZE][4], const bit2_t labels[TRAIN_SIZE] );

// GNB Predict function, given input feature array
bit2_t gnb_predict( feature_type X[NUM_FEATURES] );

#endif
