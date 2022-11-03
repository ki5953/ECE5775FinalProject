//===========================================================================
// iris.h
//===========================================================================
// @brief: This header file defines the interface for the core functions.

#ifndef DIGITREC_H
#define DIGITREC_H

#include<hls_stream.h>

#include "typedefs.h"
#include "training_data.h"

#define NUM_LABELS 3
#define NUM_FEATURES 4

float mean[NUM_LABELS][NUM_FEATURES];
float std_dev[NUM_LABELS][NUM_FEATURES];
float prior[3];

// Top function for synthesis
void dut(
    hls::stream<bit32_t> &strm_in,
    hls::stream<bit32_t> &strm_out,
    hls::stream<bit1_t>  &strm_train,
);

// GNB Training function, given input feature map
void gnb_train( feature_type features[TRAIN_SIZE][NUM_FEATURES], bit2_t labels[TRAIN_SIZE] );

// GNB Predict function, given input feature array
bit2_t gnb_predict( feature_type X[NUM_FEATURES] );

#endif
