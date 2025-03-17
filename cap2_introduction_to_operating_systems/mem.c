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
#include <stdint.h>

int
main (int argc, char *argv[]) 
{
  // if (argc != 2) {
  //   fprintf(stderr, "usage: mem <value>\n");
  //   exit(1);
  // }
  int *p = malloc(sizeof(int)); //a1: malloc is a system call that allocates memory
  assert(p != NULL);
  printf("(%d) address of p: %08x\n", (int) getpid(), (unsigned int) (uintptr_t) p);  //a2: getpid() returns the process id & prints the address's memory of p
  *p = 0; //a3: puts zero into the first slot of p

  while (1) {
    Spin(1);
    *p = *p + 1;
    printf("(%d) p: %d\n", getpid(), *p); //a4: prints the value of p
  }
  return 0;
}