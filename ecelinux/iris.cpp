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
    hls::stream<bit32_t> &strm_out
)
{

  float test_iris[NUM_FEATURES];
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

    // feature_type input1 = static_cast<float> (input);
    // input = strm_in.read();
    // feature_type input2 = static_cast<float> (input);
    // input = strm_in.read();
    // feature_type input3 = static_cast<float> (input);
    // input = strm_in.read();
    // feature_type input4 = static_cast<float> (input);

    // std::memcpy(&input1, &input, sizeof input1);
    // input = strm_in.read();
    // std::memcpy(&input2, &input, sizeof input2);
    // input = strm_in.read();
    // std::memcpy(&input3, &input, sizeof input3);
    // input = strm_in.read();
    // std::memcpy(&input4, &input, sizeof input4);

    union FeatureUnion input1;
    union FeatureUnion input2;
    union FeatureUnion input3;
    union FeatureUnion input4;

    input1.ival = input;
    input2.ival = strm_in.read();
    input3.ival = strm_in.read();
    input4.ival = strm_in.read();

    // printf("inputs:");
    // printf(" %d", input1.ival);
    // printf(" %d", input2.ival);
    // printf(" %d", input3.ival);
    // printf(" %d\n", input4.ival);
    
    test_iris[0] = input1.fval;
    test_iris[1] = input2.fval;
    test_iris[2] = input3.fval;
    test_iris[3] = input4.fval;

    // printf("test iris:");
    // printf(" %f", test_iris[0]);
    // printf(" %f", test_iris[1]);
    // printf(" %f", test_iris[2]);
    // printf(" %f\n", test_iris[3]);

    // Convert input raw bits to fixed-point representation via bit slicing
    // test_digit(31, 0) = input_lo;
    // test_digit(test_digit.length()-1, 32) = input_hi;

    // ------------------------------------------------------
    // Call IRIS
    // ------------------------------------------------------
    bit2_t prediction = gnb_predict (test_iris);

    // ------------------------------------------------------
    // Output processing
    // ------------------------------------------------------
    // Write out the voted digit value (low word first)
    strm_out.write( prediction );
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

    //Accrue Std dev
    // for sample in range(X.shape[0]):
    //     for feature in range(X.shape[1]):
    for (int i = 0; i < TRAIN_SIZE; i++){
      for (int j = 0; j < NUM_FEATURES; j++){
        // Check each standard deviation
        // print(y[sample])
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
      float exp_term = exp( -( (X[j]-mn)*(X[j]-mn) / std_2_2 ) ); // 1 + x + x^2/2 + x^3/6 + x^4/24
      // float base_x = -( (X[j]-mn)*(X[j]-mn) / std_2_2 );
      // float exp_term_2;
      // float clamp_val = 1.263 * sqrt(std);
      // if (base_x > clamp_val || base_x < -clamp_val) exp_term_2 = 0; //clamping
      // else {
      //   float x_2 = base_x * base_x;
      //   float x_3 = x_2 * base_x;
      //   float x_4 = x_3 * base_x;
      //   exp_term_2 = 1 + base_x + x_2/2 + x_3/6 + x_4/24;
      // }

      // printf("OG Exp Term: %f, New Exp: %f\n", exp_term, exp_term_2);
      gnb_prior *= (exp_term) / (first_term);
      
    }
    if(gnb_prior > labelprob){
      prediction = i;
      labelprob = gnb_prior;
    }
  }

  return prediction;

}