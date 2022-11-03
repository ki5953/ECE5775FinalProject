//==========================================================================
//digitrec.cpp
//==========================================================================
// @brief: A k-nearest-neighbors implementation for digit recognition

#include "digitrec.h"

//----------------------------------------------------------
// Top function
//----------------------------------------------------------
// @param[in] : input - the testing instance
// @return : the recognized digit (0~9)

bit4 digitrec( digit input ) 
{
  #include "training_data.h"

  // This array stores K minimum distances per training set
  bit6 knn_set[10][K_CONST];

  // Initialize the knn set
  for ( int i = 0; i < 10; ++i )
    for ( int k = 0; k < K_CONST; ++k )
      // Note that the max distance is 49
      knn_set[i][k] = 50; 

  for ( int i = 0; i < TRAINING_SIZE; ++i ) {
    for ( int j = 0; j < 10; j++ ) {
      #pragma HLS unroll
      // Read a new instance from the training set
      digit training_instance = training_data[j * TRAINING_SIZE + i];
      // Update the KNN set
      update_knn( input, training_instance, knn_set[j] );
    }
  } 

  // Compute the final output
  return knn_vote( knn_set ); 
}



//-----------------------------------------------------------------------
// update_knn function
//-----------------------------------------------------------------------
// Given the test instance and a (new) training instance, this
// function maintains/updates an array of K minimum
// distances per training set.

// @param[in] : test_inst - the testing instance
// @param[in] : train_inst - the training instance
// @param[in/out] : min_distances[K_CONST] - the array that stores the current
//                  K_CONST minimum distance values per training set

void update_knn( digit test_inst, digit train_inst, bit6 min_distances[K_CONST] )
{
  // calculate distance
  digit distance = 0;
  digit diff = test_inst ^ train_inst;
  for (int i = 0; i < 49; i++) {
    #pragma HLS unroll
    if(diff[i]) distance++;
  }

  // find current max distance
  bit6 largest_distance = min_distances[0];
  int max_index = 0;
  for(int i=1; i < K_CONST;  i++){
    if (min_distances[i] > largest_distance) {
      largest_distance = min_distances[i];
      max_index = i;
    }
  }

  // check if distance is less than largest distance
  if (distance < largest_distance) min_distances[max_index] = distance;
}


//-----------------------------------------------------------------------
// knn_vote function
//-----------------------------------------------------------------------
// Given 10xK minimum distance values, this function 
// finds the actual K nearest neighbors and determines the
// final output based on the most common digit represented by 
// these nearest neighbors (i.e., a vote among KNNs). 
//
// @param[in] : knn_set - 10xK_CONST min distance values
// @return : the recognized digit (0-9)
// 

bit4 knn_vote( bit6 knn_set[10][K_CONST] )
{
  bit6 min_distances[K_CONST];
  bit4 min_digits[K_CONST];

  // initialize min_distances
  for(int i=0; i<K_CONST;i++){
    min_distances[i] = knn_set[0][i];
    min_digits[i] = 0;
  }

  // loop through knn_set
  for(int i=1;i<10;i++){
    for(int j=0;j<K_CONST;j++){
      //find max min_distance
      bit6 max_distance = min_distances[0];
      int max_ind = 0;
      for(int k=1; k<K_CONST; k++){
        if (min_distances[k] > max_distance){
          max_distance = min_distances[k];
          max_ind = k;
        }
      }

      // potentially replace largest current minimum
      if (knn_set[i][j] < max_distance){
        min_distances[max_ind] = knn_set[i][j];
        min_digits[max_ind] = i;
      } 
    }
  }

  //vote 
  int num_votes[10] = {0,0,0,0,0,0,0,0,0,0};
  digit smallest_dist[10] = {50,50,50,50,50,50,50,50,50,50};
  for(int i=0; i<K_CONST; i++){
    num_votes[min_digits[i]]++;
    if (min_distances[i] < smallest_dist[min_digits[i]]) smallest_dist[min_digits[i]] = min_distances[i];
  }

  bit4 guess = 0;
  int guess_votes = num_votes[0];
  for(int i=1; i<10;i++){
    if (num_votes[i] > guess_votes){
      guess = i;
      guess_votes = num_votes[i];
    }
    else if(num_votes[i] == guess_votes){
      if (smallest_dist[guess] > smallest_dist[i]) guess = i;
    }
  }
  return guess;
}
