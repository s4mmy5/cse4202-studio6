#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(void) {
  struct timespec pre_time;
  struct timespec post_time;

  clock_gettime(CLOCK_REALTIME, &pre_time);
  clock_gettime(CLOCK_REALTIME, &post_time);

  printf("Time (pre-call): %lld.%.9ld\n", pre_time.tv_sec, pre_time.tv_nsec);
  printf("Time (post-call): %lld.%.9ld\n", pre_time.tv_sec, pre_time.tv_nsec);
  printf("Time (elapsed): %lld.%.9ld\n", post_time.tv_sec - pre_time.tv_sec, post_time.tv_nsec - pre_time.tv_nsec);

  exit(EXIT_SUCCESS);
}
