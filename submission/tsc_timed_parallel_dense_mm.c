/******************************************************************************
 *
 * dense_mm.c
 *
 * This program implements a dense matrix multiply and can be used as a
 * hypothetical workload.
 *
 * Usage: This program takes a single input describing the size of the matrices
 *        to multiply. For an input of size N, it computes A*B = C where each
 *        of A, B, and C are matrices of size N*N. Matrices A and B are filled
 *        with random values.
 *
 * Written Sept 6, 2015 by David Ferry
 ******************************************************************************/

#include <assert.h> //For assert()
#include <limits.h> //For LONG_MAX
#include <stdio.h>  //For printf()
#include <stdlib.h> //For exit() and atoi()
#include <string.h> //For memcpy
#include <time.h>   //For timespec and clock_gettime()

const int num_expected_args = 3;
const unsigned sqrt_of_UINT32_MAX = 65536;

// The following line can be used to verify that the parallel computation
// gives identical results to the serial computation. If the verficiation is
// successful then the program executes normally. If the verification fails
// the program will terminate with an assertion error.
// #define VERIFY_PARALLEL

static inline unsigned long long rdtsc_get(void) {
  unsigned long high, low;
  asm volatile("rdtsc" : "=a"(low), "=d"(high));
  return ((unsigned long long)low) | (((unsigned long long)high) << 32);
}

int main(int argc, char *argv[]) {

  unsigned index, row, col; // loop indicies
  long long *time_data;
  long long max_time, min_time, imm, sum;
  long long pre_time;
  long long post_time;
  long long elapsed_time;
  unsigned matrix_size, iter_count, squared_size;
  double *A, *B, *C;
#ifdef VERIFY_PARALLEL
  double *D;
#endif

  if (argc != num_expected_args) {
    printf("Usage: ./dense_mm <size of matrices> <number of iterations>\n");
    exit(-1);
  }

  matrix_size = atoi(argv[1]);
  iter_count = atoi(argv[2]);
  time_data = calloc(iter_count, sizeof(long long));

  if (matrix_size > sqrt_of_UINT32_MAX) {
    printf("ERROR: Matrix size must be between zero and 65536!\n");
    exit(-1);
  }

  squared_size = matrix_size * matrix_size;

  for (int iter = 0; iter < iter_count; iter++) {

    printf("Generating matrices...\n");

    A = (double *)malloc(sizeof(double) * squared_size);
    B = (double *)malloc(sizeof(double) * squared_size);
    C = (double *)malloc(sizeof(double) * squared_size);
#ifdef VERIFY_PARALLEL
    D = (double *)malloc(sizeof(double) * squared_size);
#endif

    for (index = 0; index < squared_size; index++) {
      A[index] = (double)rand();
      B[index] = (double)rand();
      C[index] = 0.0;
#ifdef VERIFY_PARALLEL
      D[index] = 0.0;
#endif
    }

    printf("Multiplying matrices...\n");
    pre_time = rdtsc_get();

#pragma omp parallel for private(col, row, index)
    for (col = 0; col < matrix_size; col++) {
      for (row = 0; row < matrix_size; row++) {
        for (index = 0; index < matrix_size; index++) {
          C[row * matrix_size + col] +=
              A[row * matrix_size + index] * B[index * matrix_size + col];
        }
      }
    }

#ifdef VERIFY_PARALLEL
    printf("Verifying parallel matrix multiplication...\n");
    for (col = 0; col < matrix_size; col++) {
      for (row = 0; row < matrix_size; row++) {
        for (index = 0; index < matrix_size; index++) {
          D[row * matrix_size + col] +=
              A[row * matrix_size + index] * B[index * matrix_size + col];
        }
      }
    }

    for (index = 0; index < squared_size; index++)
      assert(C[index] == D[index]);
#endif // ifdef VERIFY_PARALLEL

    printf("Multiplication done!\n");
    post_time = rdtsc_get();
    elapsed_time = post_time - pre_time;
    time_data[iter] = elapsed_time;
  }

  max_time = 0;
  min_time = LLONG_MAX;
  sum = 0;

  for (int iter = 0; iter < iter_count; ++iter) {
    min_time = time_data[iter] - min_time > 0 ? min_time : time_data[iter];
    max_time = time_data[iter] - max_time < 0 ? max_time : time_data[iter];
    sum += time_data[iter];
  }

  printf("STATS: \n");
  printf("Longest Multiplication (tsc): %lld\n", max_time);
  printf("Shortest Multiplication (tsc): %lld\n", min_time);
  printf("Average Multiplication (tsc): %lld\n", (sum / iter_count));

  return 0;
}
