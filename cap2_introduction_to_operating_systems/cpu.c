/*

fig 2.1
    - we can run this program some times simultaneosly 
    - this is the virtualization, where a unique cpu can run more than one programa at the same time
    ./cpu A & ./cpu B & ./cpu C & ./cpu D &


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