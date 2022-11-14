#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<math.h>
#include<assert.h>

#include<iostream>
#include<fstream>

#include "typedefs.h"
#include "timer.h"
#include "training_data.h"

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

//--------------------------------------
// main function
//--------------------------------------
int main(int argc, char** argv)
{
  // Open channels to the FPGA board.
  // These channels appear as files to the Linux OS
  int fdr = open("/dev/xillybus_read_32", O_RDONLY);  // Iris out
  int fdw = open("/dev/xillybus_write_32", O_WRONLY); // Iris in
  // int fdtw = open("/dev/xillybus_write_8", O_WRONLY); // Train flag

  // Check that the channels are correctly opened
  if ((fdr < 0) || (fdw < 0)) {
    fprintf (stderr, "Failed to open Xillybus device channels\n");
    exit(-1);
  }
  
  std::cout << "Start" << std::endl;
  // Number of test instances
  const int N = TEST_SIZE;

  // Timer
  Timer train_timer("Iris Train FPGA");
  Timer test_timer("Iris Test FPGA");
  
  int nbytes;
  int error = 0;
  int num_test_insts = 0;
  bit32_t prediction;
    
  //--------------------------------------------------------------------
  // Read data from the input file into two arrays
  //--------------------------------------------------------------------
  // Arrays to store test data and expected results  
  feature_type inputs[N][NUM_FEATURES] = {
    #include "data_iris/testing_iris.dat"
  };

  int expecteds[N] = {
    #include "data_iris/testing_iris_labels.dat" 
  };

  std::cout << "Imported Data" << std::endl;
   
  //-------------------------------------------------------------------------------
  // Training Model
  //-------------------------------------------------------------------------------
  // Train model on training data

  train_timer.start();

  // bit8_t train_flag = 1;
  // nbytes = write (fdtw, (void*)&train_flag, sizeof(train_flag)); // Write Train flag to 1
  // assert (nbytes == sizeof(train_flag));

  int32_t train_flag = 0;
  nbytes = write (fdw, (void*)&train_flag, sizeof(train_flag)); // Write Train flag to 1
  assert (nbytes == sizeof(train_flag));

  bit32_t train_done;
  nbytes = read(fdr, (void*)&train_done, sizeof(train_done));
  assert (nbytes == sizeof(train_done));

  train_timer.stop();

  if (train_done != 1){
    std::cout << "Training Finish Signal Not Received" << std::endl;
  }
  std::cout << "Finished Training" << std::endl;

  //--------------------------------------------------------------------
  // Send data iris
  //--------------------------------------------------------------------
    
  test_timer.start();
  
  for (int i = 0; i < N; ++i) {
    // Read input from array and split into two 32-bit words
    for (int j = 0; j < 4; j++) {
      feature_type feature = inputs[i][j];
      // bit32_t feature_i;
      // feature_i(feature.length()-1,0) = feature(feature.length()-1,0);
      // int32_t input = feature_i;
      // Write words to the device
      nbytes = write(fdw, (void*)&feature, sizeof(feature));
      assert (nbytes == sizeof(feature));
    }
  }

  //Read all output classifications
  for (int i = 0; i < N; ++i ) {
    bit32_t l_out;
    // Read result
    nbytes = read (fdr, (void*)&l_out, sizeof(l_out));
    assert (nbytes == sizeof(l_out));

    //Assign to prediction
    prediction = l_out;

    num_test_insts++;
    
    // Check for any difference between k-NN prediction vs. expected
    if ( prediction != expecteds[i] ) {
      error++;
    }
  }

  test_timer.stop();
  
  // Report overall error out of all testing instances
  std::cout << "Number of test instances = " << num_test_insts << std::endl;
  std::cout << "Overall Error Rate = " << std::setprecision(3)
            << ( (double)error / num_test_insts ) * 100
            << "%" << std::endl;
  
  return 0;
}