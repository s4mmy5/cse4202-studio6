#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  struct timespec real_time;
  struct timespec coarse_real_time;
  struct timespec monotonic_time;
  struct timespec coarse_monotonic_time;
  struct timespec tai_time;

  clock_getres(CLOCK_REALTIME, &real_time);
  clock_getres(CLOCK_MONOTONIC, &monotonic_time);
  clock_getres(CLOCK_REALTIME_COARSE, &coarse_real_time);
  clock_getres(CLOCK_MONOTONIC_COARSE, &coarse_monotonic_time);
  clock_getres(CLOCK_TAI, &tai_time);

  printf("Resolution of different clock_id's\n");

  printf("CLOCK_REALTIME precision (nsec): %ld\n", real_time.tv_nsec);
  printf("CLOCK_REALTIME_COARSE precision (nsec): %ld\n", coarse_real_time.tv_nsec);
  printf("CLOCK_MONOTONIC precision (nsec): %ld\n", monotonic_time.tv_nsec);
  printf("CLOCK_MONOTONIC_COARSE precision (nsec): %ld\n", coarse_monotonic_time.tv_nsec);
  printf("CLOCK_TAI precision (nsec): %ld\n", tai_time.tv_nsec);

  exit(EXIT_SUCCESS);
}
