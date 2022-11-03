//=========================================================================
// testbench.cpp
//=========================================================================
// @brief: testbench for k-nearest-neighbor digit recongnition application

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#include "iris.h"
#include "typedefs.h"
#include "timer.h"

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
  std::ifstream mylabels ("data_iris/testing_iris_labels.dat");
  
  // HLS streams for communicating with the cordic block
  hls::stream<bit32_t> iris_in;
  hls::stream<bit32_t> iris_out;
  hls::stream<bit1_t>  train_flag;

  // Number of test instances
  const int N = TEST_SIZE;
  
  // Arrays to store test data and expected results
  feature_type inputs[N][NUM_FEATURES];
  int   expecteds[N];

  // Timer
  Timer train_timer("Iris Train FPGA");
  Timer test_timer("Iris Test FPGA");
  
  int nbytes;
  int error = 0;
  int num_test_insts = 0;


  if ( myfile.is_open() ) {

    //-------------------------------------------------------------------------------
    // Read data from the input files into test feature array and test label array
    //-------------------------------------------------------------------------------
    for (int i = 0; i < N; ++i) {
      assert( std::getline( myfeatures, line_feature) );
      assert( std::getline( mylabels, line_label) );

      // Read feature input

      start_pos = 0;
      for(int f = 0; f < NUM_FEATURES; f++) { //Splits feature string into 4 float values, based on comma positions
        int comma_pos = line_feature.find(",", start_pos);
        inputs[i][f] = float2feature_type(stof(line_feature.substr(start_pos, comma_pos-start_pos))); //TODO: ADD feature type conversion
        start_pos = comma_pos+1;
      }
      
      //Read test input
      expecteds[i] = stoi(line_label.substr(0,1)); //Ignores comma character

      // Store the digits into arrays
      // inputs[i] = input_digit;

    }

    //-------------------------------------------------------------------------------
    // Training Model
    //-------------------------------------------------------------------------------
    //Train model on training data
    train_flag.write( 1 );

    train_timer.start();
    dut( iris_in, iris_out );
    train_timer.stop()

    bit1_t train_done = train_flag.read();

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
      for(int f = 0; f < NUM_FEATURES; f++){
        feature_type input_f = inputs[i][f];
        //Write float to device
        iris_in.write( input_f );
      }
      train_flag.write( 0 );
    }

    
    //--------------------------------------------------------------------
    // Execute the iris sim and receive data
    //--------------------------------------------------------------------
    for (int i = 0; i < N; ++i ) {
      // Call design under test (DUT)
      dut( iris_in, iris_out );

      // Read result
      bit2_t interpreted_iris = digitrec_out.read();

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
    myfile.close();
    
  }
  else
      std::cout << "Unable to open file for the testing set!" << std::endl; 
  
  // Close output file
  outfile.close();

  return 0;
}
