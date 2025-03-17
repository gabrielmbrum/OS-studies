/*
  fig 2.6

  I/O program
  - to see a example of how the system calls facilitate this process -> importance of file systems
  - open(), write() and close() -> systems calls used, which handles the requests and returns a error code -> contained into a "standar library"
  - the OS doesnt create a virtualized disk, it's all the same one
  - files are shared across diferent processes
  - protocols to dont let the system crash -> journaling or copy-to-write -> to avoid losing data
*/

#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

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