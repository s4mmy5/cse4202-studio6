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
#include <stdio.h>  //For printf()
#include <stdlib.h> //For exit() and atoi()
#include <string.h> //For memcpy
#include <time.h>   //For timespec and clock_gettime()
#include <limits.h> //For LONG_MAX

const int num_expected_args = 3;
const unsigned sqrt_of_UINT32_MAX = 65536;

// The following line can be used to verify that the parallel computation
// gives identical results to the serial computation. If the verficiation is
// successful then the program executes normally. If the verification fails
// the program will terminate with an assertion error.
// #define VERIFY_PARALLEL

// c = a - b
void timespecsub(struct timespec *a, struct timespec *b,
                        struct timespec *c) {
  c->tv_nsec = a->tv_nsec - b->tv_nsec;
  c->tv_sec = a->tv_sec - b->tv_sec;

  if (c->tv_nsec < 0) {
    c->tv_sec--;
    c->tv_nsec += 1000000000L;
  }
}

// c = a + b
void timespecadd(struct timespec *a, struct timespec *b,
                        struct timespec *c) {
  c->tv_nsec = a->tv_nsec + b->tv_nsec;
  c->tv_sec = a->tv_sec + b->tv_sec;

  if (c->tv_nsec < 0) {
    c->tv_sec++;
    c->tv_nsec -= 1000000000L;
  }
}

int main(int argc, char *argv[]) {

  unsigned index, row, col;   // loop indicies
  struct timespec *time_data; // time stats array
  struct timespec max_time, min_time; // time stats array
  struct timespec imm;
  struct timespec sum;
  struct timespec pre_time;
  struct timespec post_time;
  struct timespec elapsed_time;
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
  time_data = calloc(iter_count, sizeof(struct timespec));

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
    clock_gettime(CLOCK_MONOTONIC_RAW, &pre_time);

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
    clock_gettime(CLOCK_MONOTONIC_RAW, &post_time);
    timespecsub(&post_time, &pre_time, &elapsed_time);
    time_data[iter] = elapsed_time;

  }

  max_time.tv_sec = 0;
  max_time.tv_nsec = 0;
  min_time.tv_sec = LONG_MAX;
  min_time.tv_nsec = LONG_MAX;
  sum.tv_sec = 0;
  sum.tv_nsec = 0;

  for (int iter = 0; iter < iter_count; ++iter) {
    timespecsub(&time_data[iter], &min_time, &imm);
    if (imm.tv_sec < 0)
      min_time = time_data[iter];

    timespecsub(&time_data[iter], &max_time, &imm);
    if (imm.tv_sec >= 0 && imm.tv_nsec >= 0)
      max_time = time_data[iter];

    timespecadd(&time_data[iter], &sum, &sum);
  }

  printf("STATS: \n");
  printf("Longest Multiplication: %ld.%.9ld\n", max_time.tv_sec, max_time.tv_nsec);
  printf("Shortest Multiplication: %ld.%.9ld\n", min_time.tv_sec, min_time.tv_nsec);
  printf("Average Multiplication: %ld.%.9ld\n", (sum.tv_sec / iter_count), (sum.tv_nsec / iter_count));

  return 0;
}
