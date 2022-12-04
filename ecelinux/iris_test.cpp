//=========================================================================
// testbench.cpp
//=========================================================================
// @brief: testbench for k-nearest-neighbor digit recongnition application

#include <stdio.h>
#include <iostream>
#include <fstream>

#include "iris.h"
#include "timer.h"

using namespace std;

//------------------------------------------------------------------------
// Helper function for hex to int conversion
//------------------------------------------------------------------------
int64_t hexstring_to_int64 (std::string h) {
  int64_t x = 0;
  for (int i = 0; i < h.length(); ++i) {
    char c = h[i];
    int k = (c > '9') ? toupper(c)-'A'+10 : c - '0';
    x = x*16 + k;
  }
  return x;
}

//------------------------------------------------------------------------
// Helper function for float to feature_type conversion
//------------------------------------------------------------------------
feature_type float2feature_type (float x){
  #ifdef FIXED_TYPE
    fixed_point = ((ap_uint<TOT_WITDH>)(x*(2**DEC_BITS))); // 2**DEC_BITS effectively shifts the float left by DEC_BITS
    return fixed_point;
  #else
  return (feature_type)(x);
  #endif
}

//------------------------------------------------------------------------
// Digitrec testbench
//------------------------------------------------------------------------
int main() 
{
  // Output file that saves the test bench results
  std::ofstream outfile;
  outfile.open("out.dat");
  
  // Read input file for the testing set
  std::string line_feature;
  std::string line_label;
  std::ifstream myfeatures ("data_iris/testing_iris.dat");
  
  // HLS streams for communicating with the iris block
  hls::stream<bit32_t> iris_in;
  hls::stream<bit32_t> iris_out;

  // Number of test instances
  const int N = TEST_SIZE;

  feature_type inputs[N][NUM_FEATURES] = {
    #include "data_iris/testing_iris.dat"
  };

  int expecteds[N] = {
    #include "data_iris/testing_iris_labels.dat" 
  };

  // Timer
  Timer train_timer("Iris Train FPGA");
  Timer test_timer("Iris Test FPGA");
  
  int nbytes;
  int error = 0;
  int num_test_insts = 0;


  if ( outfile.is_open() ) {

    //-------------------------------------------------------------------------------
    // Training Model
    //-------------------------------------------------------------------------------
    //Train model on training data
    iris_in.write( 0 );

    train_timer.start();
    dut( iris_in, iris_out );
    train_timer.stop();

    bit32_t train_done = iris_out.read();

    if (train_done != 1){
      std::cout << " Training Finish Signal Not Received "<< std::endl;
    }
    //----------------------------------------------------------------------------------


    test_timer.start();
    //--------------------------------------------------------------------
    // Send data iris
    //--------------------------------------------------------------------
    for (int i = 0; i < N; ++i ) {
      // Read input from array and split into two 32-bit words
      for(int f = 0; f < 4; f++){
        union FeatureUnion input_f;
        input_f.fval = inputs[i][f];
        //Write float to device
        iris_in.write( input_f.ival );
      }
    }

    // Call design under test (DUT)
    dut( iris_in, iris_out );

    //--------------------------------------------------------------------
    // Execute the iris sim and receive data
    //--------------------------------------------------------------------
    for (int i = 0; i < N; ++i ) {

      // Read result
      bit2_t interpreted_iris = iris_out.read();
      printf("Num iris %d, expect %d, predict %d\n", i, (int)expecteds[i], (int)interpreted_iris);

      num_test_insts++;
      
      // Check for any difference between GNB predicted iris and expected iris
      if ( interpreted_iris != expecteds[i] ) {
        error++;
      }
    }
    
    test_timer.stop();
    
    // Report overall error out of all testing instances
    std::cout << "Number of test instances = " << num_test_insts << std::endl;
    std::cout << "Overall Error Rate = " << std::setprecision(3)
              << ( (double)error / num_test_insts ) * 100
              << "%" << std::endl;
    outfile << "Number of test instances = " << num_test_insts << std::endl;
    outfile << "Overall Error Rate = " << std::setprecision(3)
            << ( (double) error / num_test_insts ) * 100 
            << "%" << std::endl;
    
    // Close input file for the testing set
    outfile.close();
    
  }
  else std::cout << "Unable to open file for the testing set!" << std::endl; 
  
  // Close output file
  outfile.close();

  return 0;
}
