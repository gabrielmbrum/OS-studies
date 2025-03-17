/*
  fig 2.3
  virtualize memory
  - if run more than one, each one creates each own memory space
  - the same adress printed, doesnt mean the same physical adress

  ./mem &; ./mem &
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"

int
main (int argc, char *argv[]) 
{
  if (argc != 2) {
    fprintf(stderr, "usage: mem <value>\n");
    exit(1);
  }
  int *p = malloc(sizeof(int));
  assert(p != NULL);
  printf("(%d) addr pointed to by p: %p\n", (int) getpid(), p);
  *p = atoi(argv[1]); // assign value to addr stored in p
  *p = 0;

  while (1) {
    Spin(1);
    *p = *p + 1;
    printf("(%d) value of p: %d\n", getpid(), *p);
  }
  return 0;
}
  