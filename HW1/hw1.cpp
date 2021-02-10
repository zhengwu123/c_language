/*
 * cs 574 assignment 1
 * Author zheng wu
 */
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <time.h>

using namespace std;
#define BILLION 1000000000L

#define INT2VOIDP(i) (void*)(uintptr_t)(i)
//declare global variables here
struct timespec ts;
int nThreads = 1;
int matrixSize = 100;
int matrixValue = 0;
float ** arr2D;
float * totalrowSums;
float sum;
int row;
int col;
//declare lock here

pthread_mutex_t mylock;

//define timing variables here.
long * threadTimeArr;

//function that initialize matrix with random number
float ** initializeMatrixRandom(int n ) {
    float ** arr = new float*[n];

    for(int i = 0;i < n; i++) {
        arr[i] = new float[n];
        for(int j = 0; j < n; j++) {
            arr[i][j] = rand() % 100 + 1;
        }
    }
    return arr;
}
//function that initializeMatrix with fixed value v
float ** initializeMatrixFixedValue(int n , float v) {
    printf("matrix size is %d \n",n);
    float ** arr = new float*[n];

    for(int i = 0;i < n; i++) {
        arr[i] = new float[n];
        for(int j = 0; j < n; j++) {
            arr[i][j] = v;
        }
    }
    return arr;
}

// print matrix for testing purpose
void printMatrix(float ** matrix, int n){
    for(int i = 0; i < n; i++){
        printf("\n");
        for(int j = 0; j < n; j++){
            printf("%f,", matrix[i][j]);
        }
    }
}
//function to detemine is int is prime
bool isPrime(int num){
    bool flag=true;
    for(int i = 2; i <= num / 2; i++) {
        if(num % i == 0) {
            flag = false;
            break;
        }
    }
    return flag;
}
//function determine is float is integer
bool isInteger(float k)
{
    return floor(k) == k;
}

//find 2 closet factors
vector<int> Factorize(int num) {
    vector<int> ans;
   bool prime =  isPrime(num);
   //check if prime
   if(prime) {
       ans.push_back(1);
       ans.push_back(num);
       return ans;
   }
   //check if sqrt is integer
   if(isInteger(sqrt(num))) {
       ans.push_back(sqrt(num));
       ans.push_back(sqrt(num));
       return ans;
   }
    int up = (int)sqrt(num) + 1;
    //printf("value of up : %d,", up);
    for (int i = up; i >= 2; i--) {
        if (num % i == 0) {
            //printf("value of i : %d,", i);
            ans.push_back(i);
            ans.push_back(num / i);
            break;
        }
    }
    if(ans.size() != 2) {
        cout << "Invalid value! " << ans.size() << " ";
        exit(1);
    }

    //printf( "ans size: %lu,", ans.size());
    return ans;
}
// function tha calculates sum of matrix
float sumOfMatrix(float ** matrix, int n) {
    float sum = 0;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            sum += matrix[i][j];
        }
    }
    return sum;
}
//print vector for testing purpose
void printVector(vector<int> path) {
    for(int i = 0; i < path.size(); i++)
        cout << path[i] << " ";
}

//function that used to calculate each small portion of matrix
void *rowSumMatrixWithNthreads(void * i){
    uint64_t diff;
    struct timespec start, end;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);	/* mark start time */
    pthread_mutex_lock(&mylock);
    long n  = (long) i;

        for (int i = 0; i < matrixSize/col; i++) {
            for (int j = 0; j < matrixSize/row; j++) {
                //add each sub portion to small matrix
                sum+= arr2D[i + n /(matrixSize/col)][j + n%(matrixSize/row)];
                totalrowSums[n] += arr2D[i + n /(matrixSize/col)][j + n%(matrixSize/row)];
            }

        }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    threadTimeArr[n] = diff;
    printf("Thread %ld elapsed process CPU time = %llu nanoseconds\n", n, (long long unsigned int) diff);
    cout << "rowSums : " << n << "=" << totalrowSums[n] << "\n ";
    pthread_mutex_unlock(&mylock);
    pthread_exit(NULL);

}

int main(int argc, char *argv[]) {
    uint64_t timeDiff;
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);	/* mark start time */
    cout << "Program is running "<< "\n";
    cout << "----------------------------- "<< "\n";
    for (int i = 0; i < argc; ++i) {
        //cout << argv[i] << "\n";
        char *cmd = strdup(argv[i]);

        if (strcmp(cmd, "-t") == 0) {

            if (i + 1 < argc) {
                char *currentToken = strdup(argv[i + 1]);
                nThreads = atoi(currentToken);
                cout << "Number of threads "
                     << nThreads << "\n";
            } else {
                cout << "Not enough arguments "
                     << "\n";
                exit(1);
            }
        }

        if (strcmp(cmd, "-s") == 0) {
            if (i + 1 < argc) {
                char *currentToken = strdup(argv[i + 1]);
                matrixSize = atoi(currentToken);
                cout << "Size of the matrix "
                     << matrixSize << "\n";
            } else {
                cout << "Not enough arguments "
                     << "\n";
                exit(1);
            }
        }

        if (strcmp(cmd, "-v") == 0) {
            if (i + 1 < argc) {
                char *currentToken = strdup(argv[i + 1]);
                matrixValue = atoi(currentToken);
                cout << "Value for each cell "
                     << matrixValue << "\n";
            } else {
                cout << "Not enough arguments "
                     << "\n";
                exit(1);
            }
        }
    }
    if(matrixValue != 0) {
        //intialize matrix with value
        arr2D = initializeMatrixFixedValue(matrixSize, matrixValue);
    } else {
        //intialize matrix without a given value
        arr2D = initializeMatrixRandom(matrixSize);
    }
    threadTimeArr = new long[nThreads];
    //main thread computes sum
    float Mainsum = sumOfMatrix(arr2D,matrixSize);
    cout << "Main thread calculated matrix sum "
         << Mainsum <<"\n";
    vector<int> result = Factorize(nThreads);
    row = max(result[0],result[1]);
    col = min(result[0],result[1]);
    totalrowSums =  new float[nThreads];
    sum = 0;

    int tc ;
    //create threads
    pthread_t threads[nThreads];
    for(int i = 0; i <nThreads; i++) {
     tc = pthread_create(&threads[i], NULL, rowSumMatrixWithNthreads, INT2VOIDP(i));
       // rowSumMatrixWithNthreads(t);
        pthread_join(threads[i],NULL);
        if (tc) {
            cout << "Error:unable to create thread," << tc << endl;
            exit(-1);
        }

    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    timeDiff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    printf("Whole program running CPU time = %llu nanoseconds\n", (long long unsigned int) timeDiff);
    printf(" sum of matrix after all threads finished adding is %f \n",sum);
    pthread_mutex_destroy(&mylock);

    //clean up memory after use
    printf("\n");
    printf("Cleaning up matrix memory...\n");
    for (  int x= 0; x < matrixSize; x++)
    {
        delete [] arr2D[x];
    }
    delete [] arr2D;
    arr2D = 0;
    printf("Matrix cleaned. ready for next test..\n");

    //vector<int> factors = Factorize(10);
    //printVector(factors);
    pthread_exit(NULL);
    return 0;

}


