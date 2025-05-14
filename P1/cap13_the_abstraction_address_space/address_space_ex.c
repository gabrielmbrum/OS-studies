#include <stdio.h>
#include <stdlib.h>

/*

  running this program will print the location of the code, heap, and stack

  you can see that the code is at a lower address than the heap and stack,
  and the heap is at a lower address than the stack (by a large margin)

*/

int main (int argc, char *argv[]) {
  printf("location of code: %p\n", main);
  printf("location of heap: %p\n", malloc(1));
  int x = 3;
  printf("location of stack: %p\n", &x);
  return x;
}