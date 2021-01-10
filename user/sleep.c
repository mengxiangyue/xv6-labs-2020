#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/syscall.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

  if(argc != 2){
    fprintf(2, "Usage: Sleep [x] \n");
    exit(1);
  }

  int second = atoi(argv[1]);
  if (second <= 0)
  {
    fprintf(2, "Second should greater than 0 \n");
    exit(1);
  }
  fprintf(1, "%d", second);
  int result = sleep(second);
  exit(result);
}
