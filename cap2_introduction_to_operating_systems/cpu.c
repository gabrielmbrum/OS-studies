/*

fig 2.1
  - we can run this program some times simultaneosly 
  - this is the virtualization, where a unique cpu can run more than one programa at the same time
  
  run like this: ./cpu A & ./cpu B & ./cpu C & ./cpu D &
  
  to kill use:
    prompt> jobs
    [1] + Running ./cpu A &
    [2] - Running ./cpu B &
    [3]   Running ./cpu C &
    [4]   Running ./cpu D &
    prompt> kill %1
    [1] + Terminated ./cpu A 

*/
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

int
main (int argc, char *argv[]) 
{
  if (argc != 2) {
    fprintf(stderr, "usage: cpu <string>\n");
    exit(1);
  }

  char *str = argv[1];
  while (1) {
    Spin(1);
    printf("%s\n", str);
  }

  return 0;
}