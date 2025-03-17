/*
  fig 2.3
  virtualize memory
  - if run more than one, each one creates each own memory space
  - the same adress printed, doesnt mean the same physical adress

  ./mem &; ./mem &
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

void 
Spin(int seconds) 
{
  struct timeval start, end;
  gettimeofday(&start, NULL);

  do {
    gettimeofday(&end, NULL);
  } while ((end.tv_sec - start.tv_sec) < seconds);
}

int
main (int argc, char *argv[]) 
{
  int *p = malloc(sizeof(int));
  assert(p != NULL);
  printf(" (%d) address of p: %08x\n", getpid(), (unsigned) p);
  *p = 0;

  while (1) {
    Spin(1);
    *p = *p + 1;
    printf("(%d) p: %d\n", getpid(), *p);
  }
}
  