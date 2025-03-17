/*
  fig 2.6

  I/O program
  - to see a example of how the system calls facilitate this process -> importance of file systems
  - open(), write() and close() -> systems calls used, which handles the requests and returns a error code -> contained into a "standar library"
  - the OS doesnt create a virtualized disk, it's all the same one
  - files are shared across diferent processes
  - protocols to dont let the system crash -> journaling or copy-to-write -> to avoid losing data
*/
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int
main (int argc, char * argv[]) 
{
  int fd = open("/tmp/file", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
  assert(fd >= 0);
  char buffer[20];
  sprintf(buffer, "hello world\n");
  int rc = write(fd, buffer, strlen(buffer));
  assert(rc == strlen(buffer));
  fsync(fd);
  close(fd);
  return 0;
}