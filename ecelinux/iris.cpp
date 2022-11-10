//==========================================================================
// iris.cpp
//==========================================================================
// @brief: A k-nearest-neighbor implementation for digit recognition (k=1)

#include "iris.h"

using namespace std;

float mean[NUM_LABELS][NUM_FEATURES];
float std_dev[NUM_LABELS][NUM_FEATURES];
float prior[NUM_LABELS];

//----------------------------------------------------------
// Top function
//----------------------------------------------------------

void dut(
    hls::stream<bit32_t> &strm_in,
    hls::stream<bit32_t> &strm_out,
    hls::stream<bit8_t>  &strm_train
)
{

  feature_type test_iris[NUM_FEATURES];
  bit2_t       voted_iris;

  //Decide if training
  bit8_t train = strm_train.read();
  if (train){
    gnb_train(iris_trn, trn_label);
    strm_out.write( 1 );
  } else {
    // ------------------------------------------------------
    // Input processing
    // ------------------------------------------------------
    // Read the two input 32-bit words (low word first)
    feature_type input1 = strm_in.read();
    feature_type input2 = strm_in.read();
    feature_type input3 = strm_in.read();
    feature_type input4 = strm_in.read();

    test_iris[0] = input1;
    test_iris[1] = input2;
    test_iris[2] = input3;
    test_iris[3] = input4;

    // Convert input raw bits to fixed-point representation via bit slicing
    // test_digit(31, 0) = input_lo;
    // test_digit(test_digit.length()-1, 32) = input_hi;

    // ------------------------------------------------------
    // Call IRIS
    // ------------------------------------------------------
    voted_iris = gnb_predict( test_iris );

    // ------------------------------------------------------
    // Output processing
    // ------------------------------------------------------
    // Write out the voted digit value (low word first)
    strm_out.write( voted_iris );
  }
}

//-----------------------------------------------------------------------
// gnb_train function
//-----------------------------------------------------------------------
// Given the training features and their labels, this
// function maintains/updates an arrays of the mean and std_dev 
// for each feature for each label

// @param[in] : input_features - the 90x4 feature array
// @param[in] : input_labels   - the 90x1 label array

void gnb_train( const float features[TRAIN_SIZE][4], const bit2_t labels[TRAIN_SIZE] ){
    // features is the training set where each row is a sample and each column is a feature.
    // labels are the training labels which correspond to each row of "features"
    bit2_t output_labels[3] = {0,1,2}; //all possible outputs in the training se
    // NUM_LABELS = output_size = 3 // the number of all possible outputs
    
    // mean = np.zeros((output_size, X.shape[1])) # a m by n matrix where m is the number of outputs, 
    //                                         # and n is the number of features
    //                                         # mean[i,j] is the mean for feature j for samples whose output is i.
    // std = np.zeros((output_size, X.shape[1])) # a m by n matrix where m is the number of outputs, 
    //                                         # and n is the number of features
    //                                         # std[i,j] is the standard deviation for feature j for samples whose output is i.
    // ======= Your Code Starts Here =======
    // for label in y:
    //     if(label == output_labels[0]):
    //         prior[0] += 1
    //     elif(label == output_labels[1]):
    //         prior[1] += 1
    //     elif(label == output_labels[2]):
    //         prior[2] += 1

    //Zero out arrays
    for(int i = 0; i < NUM_LABELS; i++){
      for(int j = 0; j < NUM_FEATURES; j++){
        mean[i][j] = 0.0;
        std_dev[i][j] = 0.0;
      }
      prior[i] = 0.0;
    }
    
    for (int i = 0; i < TRAIN_SIZE; i++){
        if (labels[i] == 0) prior[0] += 1;
        else if (labels[i] == 1) prior[1] += 1;
        else if (labels[i] == 2) prior[2] += 1;
    }
    
    float m1_c = prior[0]; // Accrue running totals of each label for division in mean calculations
    float m2_c = prior[1];
    float m3_c = prior[2];
    for(int i = 0; i < NUM_LABELS; i++){
      prior[i] = prior[i] / (float) TRAIN_SIZE;
    }
    // Accrue Means
    for (int i = 0; i < TRAIN_SIZE; i++){
      for (int j = 0; j < NUM_FEATURES; j++){
        //Check to see what the label is for this attribute, and add to corresponding index of mean array
        //Need to accrue means for each attribute, for each type of flower.
        //print(y[sample])
        if(labels[i] == output_labels[0]){
          mean[0][j] += features[i][j] / m1_c;
        }
        else if(labels[i] == output_labels[1]){
          mean[1][j] += features[i][j] / m2_c;
        }
        else if(labels[i] == output_labels[2]){
          mean[2][j] += features[i][j] / m3_c;
        }
      }
    }
  // TODO Can move divides out here?

    //Accrue Std dev
    // for sample in range(X.shape[0]):
    //     for feature in range(X.shape[1]):
    for (int i = 0; i < TRAIN_SIZE; i++){
      for (int j = 0; j < NUM_FEATURES; j++){
        // Check each standard deviation
        // print(y[sample])
        if(labels[i] == output_labels[0]){
          std_dev[0][j] += pow((features[i][j]-mean[0][j]), 2) / m1_c;
        }
        else if(labels[i] == output_labels[1]){
          std_dev[1][j] += pow((features[i][j]-mean[1][j]), 2) / m2_c;
        }
        else if(labels[i] == output_labels[2]){
          std_dev[2][j] += pow((features[i][j]-mean[2][j]), 2) / m3_c;
        }
      }
    }
    for (int i = 0; i < NUM_LABELS; i++){
      for (int j = 0; j < NUM_FEATURES; j++){
        std_dev[i][j]= sqrt(std_dev[i][j]); 
      }
    }
}

//-----------------------------------------------------------------------
// gnb_predict function
//-----------------------------------------------------------------------
// Predict one iris
//
// @param[in] : X - the input feature array
// @return : the recognized flower
// 

bit2_t gnb_predict( feature_type X[4] ){
  bit2_t prediction = 3;
  float labelprob = 0;
  
  for(int i = 0; i < NUM_LABELS; i++){
    float gnb_prior = prior[i];
    for(int j = 0; j < NUM_FEATURES; j++){
                                          
      float std = std_dev[i][j];   
      float first_term = 1 / sqrt(2*M_PI*(pow(std, 2)));
      float mn = mean[i][j];
      float exp_term = exp(-(pow((X[j]-mn),2)/(2*pow(std,2))));
      gnb_prior *= first_term*exp_term;
    }
    if(gnb_prior > labelprob){
      prediction = i;
      labelprob = gnb_prior;
    }
  }

  return prediction;

}
