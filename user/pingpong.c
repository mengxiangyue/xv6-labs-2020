#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/syscall.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p[2];
  int buffer_size = 4;
  char buf[buffer_size];

  char str_ping[] = "ping";
  char str_pong[] = "pong";

  pipe(p);

  int pid = fork();
  if (pid > 0) // parent
  {
    int n = write(p[1], str_ping, buffer_size);
    if (n != 4)
    {
      fprintf(2, "parent: write error\n");
      exit(1);
    }
    /*
    The wait system call returns the PID of an exited (or killed) child of 
    the current process and copies the exit status of the child to the address 
    passed to wait; if none of the caller’s children has exited, 
    wait waits for one to do so. If the caller has no children, 
    wait immediately returns -1. 
    If the parent doesn’t care about the exit status of a child, it can pass a 0 address to wait.
    */
    pid = wait((int *) 0);
    n = read(p[0], buf, buffer_size);
    if (n != 4)
    {
      fprintf(2, "parent: read error\n");
      exit(1);
    }
    fprintf(1, "parent receive: %d, %s\n", getpid(), buf);
    fprintf(1, "%d: received %s\n", getpid(), buf);
    exit(0);
    
  } else if (pid == 0) { // child process
    int n = read(p[0], buf, buffer_size);
    if (n != 4)
    {
      fprintf(2, "child: read error\n");
      exit(1);
    }
    fprintf(1, "child receive:: %d, %s\n", getpid(), buf);
    fprintf(1, "%d: received %s\n", getpid(), buf);
    n = write(p[1], str_pong, buffer_size);
    if (n != 4)
    {
      fprintf(2, "child: write error\n");
      exit(1);
    }
    exit(0);

  } else {
    printf("fork error\n");
    exit(0);
  }
}
