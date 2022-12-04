//==========================================================================
// dummy.cpp
//==========================================================================
// dummy program for timing stream in and stream out

#include "iris.h"

#define mymax(a, b) ((a>=b)?a:b)
#define mymin(a, b) ((a<b)?a:b)

#define SQRT(n) ()

using namespace std;

float mean[NUM_LABELS][NUM_FEATURES];
float std_dev[NUM_LABELS][NUM_FEATURES];
float prior[NUM_LABELS];

//----------------------------------------------------------
// Top function
//----------------------------------------------------------

void dut(
    hls::stream<bit32_t> &strm_in,
    hls::stream<bit32_t> &strm_out
)
{

  float test_iris[TEST_SIZE][NUM_FEATURES];
  //Decide if training
  bit32_t input = strm_in.read();
  if (input == 0){
    gnb_train(iris_trn, trn_label);
    strm_out.write( 1 );
  } else {
    // ------------------------------------------------------
    // Input processing
    // ------------------------------------------------------
    // Read the two input 32-bit words (low word first)
    
    union FeatureUnion input1;
    union FeatureUnion input2;
    union FeatureUnion input3;
    union FeatureUnion input4;

    TEST_LOOP: for(int i = 0; i < TEST_SIZE; i++){

      input1.ival = (i == 0) ? input : strm_in.read();
      input2.ival = strm_in.read();
      input3.ival = strm_in.read();
      input4.ival = strm_in.read();

      // ------------------------------------------------------
      // Call IRIS
      // ------------------------------------------------------
      bit2_t prediction = 0;

      // ------------------------------------------------------
      // Output processing
      // ------------------------------------------------------
      // Write out the voted digit value (low word first)
      strm_out.write( prediction );

    } //End test for loop
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

    //Accrue Std dev
    for (int i = 0; i < TRAIN_SIZE; i++){
      for (int j = 0; j < NUM_FEATURES; j++){
        // Check each standard deviation
        if(labels[i] == output_labels[0]){
          std_dev[0][j] += (features[i][j]-mean[0][j])*(features[i][j]-mean[0][j]) / m1_c;
        }
        else if(labels[i] == output_labels[1]){
          std_dev[1][j] += (features[i][j]-mean[1][j])*(features[i][j]-mean[1][j]) / m2_c;
        }
        else if(labels[i] == output_labels[2]){
          std_dev[2][j] += (features[i][j]-mean[2][j])*(features[i][j]-mean[2][j]) / m3_c;
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
  
  PREDICT_LOOP: for(int i = 0; i < NUM_LABELS; i++){
    float gnb_prior = prior[i];
    //float smooth = 1.0;
    for(int j = 0; j < NUM_FEATURES; j++){
                                          
      float std = std_dev[i][j];
      float std_2_2 = (std)*(std)*2;
      float first_term = sqrt(3.14159265358979*std_2_2); 
      float mn = mean[i][j];
      float x = X[j];
      float exp_term = exp( -( (x-mn)*(x-mn) / std_2_2 ) );

      gnb_prior *= (exp_term) / (first_term);
      
    }
    if(gnb_prior > labelprob){
      prediction = i;
      labelprob = gnb_prior;
    }
  }

  return prediction;

}