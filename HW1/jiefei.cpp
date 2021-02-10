// Jiefei Liu
// CS 574 Project 1
// 1/31/2021

// import necessary libraries
#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctime>
#include <cstdlib>

using namespace std;

pthread_mutex_t myMutex;
float sumArray = 0;
long *totalRowsum;
// This function will fill a array
float* fillArray(int array_number, int size){
    srand((unsigned)time(NULL));
    // Create array
    float *a = new float[size * size];
    if (array_number != -1){
        for (int i =0; i<size; i++){
            for (int j=0; j<size; j++){
               *(a + i*size + j)= array_number;
            } // end inner for loop
        }  // end outer for loop
    }
    else{  // if user does not input array number it will generate the random numbers for 2D array
        for (int i =0; i<size; i++){
            for (int j=0; j<size; j++){
                *(a + i*size + j) = 1 + (rand() % 100);
            } // end inner for loop
        }  // end outer for loop
    }  // end else
    return a;
}

// This funtion is help to print the 2D array
void print_array(float *arr, int size){
    for (int i = 0; i < size; ++i){
        for (int j = 0; j < size; ++j){
            cout << setw(3) << *(arr + i*size + j) << ' ';
        }  // end inner for loop
        std::cout << std::endl;
    }  // end outer for loop
}  // end function

// find all factors of a number
void factors(int Threads, int &fact1, int &fact2){
    int j = 0;
    if (Threads == 1){
    	fact1 = 1;
	fact2 = 1;
    }
    else{
        // Get the number of factors for threads number
        for(int i = 1; i <= Threads; ++i){
           if(Threads % i == 0){
               j++;
           } // end if
        }  // end for
        int factor[j];
        int index = 0;
        // find two factors which are close
        for(int k = 1; k <= Threads; ++k){
            if(Threads % k == 0){
                //cout << k << endl;
                factor[index] = k;
                index++;
             } // end if
         }  // end for
        if (j%2 == 0){
            fact1 = factor[j/2-1];
            fact2 = factor[j/2];
         }  // end if
         else{
            fact1 = factor[j/2-1];
            fact2 = factor[j/2+1];
         }  // end else
    }
}  // end factors

// Declare struct args
struct args {
    int threads;
    int ArraySize;
    int Thread_num;
    float *Arr;
    int factor1;
    int factor2;
    float *Sum;
    double *Time;
    int start;
    int end;
};

// This function will sum the array
void* sum_array(void* input){
    // lock threads
    pthread_mutex_lock(&myMutex);
    // declare the time recording
    struct timespec start, finish;
    double elapsed;
    // Start time recording
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
    // Each thread computes sum of part of the array
    int threads_num = ((struct args*)input)->Thread_num;
    float *a = ((struct args*)input)->Arr;
    int thread_part = threads_num;
    int MAX = ((struct args*)input)->ArraySize;
    int MAX_THREADS = ((struct args*)input)->threads;
    float *sumfunc = ((struct args*)input)->Sum;
    double *time = ((struct args*)input)->Time;
    int rows = ((struct args*)input)->start;
    int cols = ((struct args*)input)->end;
    cout << "I am thread " << thread_part << endl;
    // print_array(a, MAX);
    // sum of the matrix
    for (int i = thread_part * rows; i < (thread_part + 1) * rows; i++){
        for (int j = thread_part * cols; j < (thread_part + 1) * cols; j++) {
            totalRowsum[thread_part] += *(a + i*MAX + j);
            sumArray += *(a + i*MAX + j);
           // cout << "sum[]" << totalRowsum << endl;
        } // end inner for
        cout << "total row sum " << totalRowsum[thread_part] <<" finished." << endl;
      }  // end outer for
    cout << "Thread " << thread_part <<" finished." << endl;
    // end time recording
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &finish);
    // calculate threads running time
    elapsed = 1000.0*finish.tv_sec + 1e-6*finish.tv_nsec - (1000.0*start.tv_sec + 1e-6*start.tv_nsec);
    time[thread_part] = elapsed;
    cout << "Thread " << thread_part << " takes " << elapsed << " ms." << endl;
    // unlock threads
    pthread_mutex_unlock(&myMutex);
    pthread_exit(NULL);
    return 0;
}  // end sum array function


int main(int argc, char* argv[]){
    // Create local veriables
    int MAX = 100;  // Size of array
    int MAX_THREADS = 1;  // Number of threads
    int ARRAY_NUMBER = 1;  // Array element
    cout << "I am here" << endl; 
    // Deal with the commond line inputs
    for(int i = 0; i < argc; ++i) {
        if (string(argv[i]) == "-t"){
            MAX_THREADS = atoi(argv[i+1]);
            //cout<< "Threads is "<< argv[i+1] << endl;
        }  // end if
        if (string(argv[i]) == "-s"){
            MAX = atoi(argv[i+1]);
            //cout<< "Array size is "<< argv[i+1] << endl;
        }  // end if
        if (string(argv[i]) == "-v"){
            ARRAY_NUMBER = atoi(argv[i+1]);
            //cout<< "Array element is "<< argv[i+1] << endl;
        }  // end if
    }  // end for
    cout << "I am here too" << endl;
    // declare the time recording
    struct timespec start, finish;
    double elapsed;
    totalRowsum = new long[MAX_THREADS];  //dynamically allocate 1D array to store row sums
    double *countTime = new double[MAX_THREADS]; //dynamically allocate 1D array to store timings
    for( int i = 0; i < MAX_THREADS; i++)       // fill the array with default value 0
        countTime[i] = 0;
    int threads_num = 0; // define the thread number
    int fact1, fact2;  // Get two factors based on the number of threads
    // create a 2D array and fill the array
    float *a = fillArray(ARRAY_NUMBER, MAX);
    // Helper function to print array
    //print_array(a, MAX);
    cout << "hello" << endl;
    // Start time recording for mian thread
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

    // Get two factors based on the number of threads
    factors(MAX_THREADS, fact1, fact2);
    //cout << " Two factors are " << fact1 << " " << fact2 << endl;
    cout << "I am here yea!" << endl;    
    // declare the threads
    pthread_t threads[MAX_THREADS];
    // Declare structs
    struct args *Data;
    Data = (struct args*)malloc(MAX_THREADS * sizeof(struct args));
   	cout << "end struct create" << endl;
    // Create each thread arguments
    for (int i=0; i<MAX_THREADS; ++i){
        (Data+i)->threads = MAX_THREADS;
        (Data+i)->ArraySize = MAX;
        (Data+i)->Arr = a;
        (Data+i)->Thread_num = i;
        (Data+i)->factor1 = fact1;
        (Data+i)->factor2 = fact2;
        //(Data+i)->Sum = sum;
        (Data+i)->Time = countTime;
        (Data+i)->start = MAX/fact1;
        (Data+i)->end = MAX/fact1;
    }
    cout << "Great" << endl;
    //pthread_mutex_init(&myMutex,NULL);
    // Creating threads
    for (int i=0; i<MAX_THREADS; i++){
        pthread_create(&threads[i], NULL, sum_array, (void*)(Data+i));
        pthread_join(threads[i], NULL);
    }  // end for

   // pthread_mutex_destroy(&myMutex);
    
    // Adding sum of all threads results
    int total_sum = 0;
    for (int i=0; i<MAX_THREADS; i++){
        total_sum += totalRowsum[i];
    }  // end for
    cout << "Sum of 2D array from child threads is " << sumArray << endl;
    cout << "Sum of 2D array from main is " << total_sum << endl;
    // end time recording for main thread
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &finish);
    elapsed = 1000.0*finish.tv_sec + 1e-6*finish.tv_nsec - (1000.0*start.tv_sec + 1e-6*start.tv_nsec);
    cout << "Main thread takes " << elapsed << " ms." << endl;
    delete totalRowsum;
    pthread_exit(NULL);
    exit(0);
}  // end main
