//===========================================================================
// training_data.h
//===========================================================================
// @brief: This header defines a 2D array that includes 10 training sets, 
//         where each set contains 1800 training data.


#ifndef TRAINING_DATA_H
#define TRAINING_DATA_H

#include "typedefs.h"


#define TRAIN_SIZE 90
#define TEST_SIZE 60

#define NUM_LABELS 3
#define NUM_FEATURES 4


const feature_type iris_trn[TRAIN_SIZE][4] = {
    #include "data_iris/training_iris.dat" 
};

const bit2_t trn_label[TRAIN_SIZE] = {
    #include "data_iris/training_iris_labels.dat" 
};

// const feature_type testing_data[TEST_SIZE] = {
//     {
//     #include "data_iris/testing_iris.dat" 
//     },
// }; 

// const bit2_t testing_data_labels[TEST_SIZE] = {
//     {
//     #include "data_iris/testing_iris_labels.dat" 
//     },
// };

#endif
