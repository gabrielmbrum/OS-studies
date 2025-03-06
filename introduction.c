#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
//#include "common.h"
  // para fig 2.6
#include <fcntl.h>    // Contém a definição de open()
#include <sys/stat.h> // Contém as permissões de arquivos (S_IRWXU, etc.)
#include <sys/types.h> // Tipos de dados POSIX (opcional)
#include <unistd.h>   // Para close(), write()

// ctlr + c & ps aux | grep intro & kill the process num

void 
Spin(int seconds) 
{
  struct timeval start, end;
  gettimeofday(&start, NULL);

  do {
    gettimeofday(&end, NULL);
  } while ((end.tv_sec - start.tv_sec) < seconds);
}

/*
  fig 2.6

  I/O program
  - to see a example of how the system calls facilitate this process -> importance of file systems
  - open(), write() and close() -> systems calls used, which handles the requests and returns a error code -> contained into a "standar library"
  - the OS doesnt create a virtualized disk, it's all the same one
  - files are shared across diferent processes
  - protocols to dont let the system crash -> journaling or copy-to-write -> to avoid losing data


int
main (int argc, char * argv[]) 
{
  int fd = open("/tmp/file", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  assert(fd > -1);
  int rc = write(fd, "hello world\n", 13);
  assert(rc == 13);
  close(fd);
  return 0;
}

*/

//-----------------------------//-------------------------------//

/*

  fig 2.1
    - we can run this program some times simultaneosly 
    - this is the virtualization, where a unique cpu can run more than one programa at the same time

  cpu.c-

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
*/


/*
  fig 2.3
  virtualize memory
  - if run more than one, each one creates each own memory space
  - the same adress printed, doesnt mean the same physical adress

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

  return 0;
}

*/

//-----------------------------//-------------------------------//

/*
  fig 2.5
  multi-thread program
  - it has three instructions: load, increment and store
  - these doesnt run all at once (atomically) -> generate strange results with big inputs (ex.: 100k)
  - the counter is shared between the threads, so this is where the mess happens 



volatile int counter = 0;
int loops;

void *worker (void *arg) {
  int i;
  for (i = 0; i < loops; i++)
    counter++;
  return NULL;
}

int
main (int argc, char *argv[]) 
{
  if (argc != 2) {
    fprintf(stderr, "usage: threads <value>\n");
    exit(1);
  }

  loops = atoi(argv[1]);
  pthread_t p1, p2;
  printf("Initial value: %d\n", counter);
  
  pthread_create(&p1, NULL, worker, NULL);
  pthread_create(&p2, NULL, worker, NULL);
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  printf("final value : %d\n", counter);

  return 0;
}

*/
