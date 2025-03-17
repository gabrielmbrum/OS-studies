/*
  fig 2.5
  multi-thread program
  - it has three instructions: load, increment and store
  - these doesnt run all at once (atomically) -> generate strange results with big inputs (ex.: 100k)
  - the counter is shared between the threads, so this is where the mess happens 
*/
#include <stdio.h>
#include <stdlib.h>
#include "common_threads.h"

volatile int counter = 0;
int loops;

void *worker(void *arg) {
  int i;
  for (i = 0; i < loops; i++) {
counter++;
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) { 
fprintf(stderr, "usage: threads <loops>\n"); 
exit(1); 
  } 
  loops = atoi(argv[1]);
  pthread_t p1, p2;
  printf("Initial value : %d\n", counter);
  Pthread_create(&p1, NULL, worker, NULL); 
  Pthread_create(&p2, NULL, worker, NULL);
  Pthread_join(p1, NULL);
  Pthread_join(p2, NULL);
  printf("Final value   : %d\n", counter);
  return 0;
}