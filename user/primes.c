// #include "kernel/types.h"
// #include "user/user.h"

// int main(int argc, char *argv[])
// {
//   int p[2][2];
//   pipe(p[0]);
//   for (int i = 2; i <= 35; i++)
//     write(p[0][1], &i, 1);
//   close(p[0][1]);
//   int idx = 0, sieve, num;
//   while (fork() == 0)
//   {
//     if (read(p[idx][0], &sieve, 1))
//     {
//       fprintf(1, "prime %d\n", sieve);
//       pipe(p[1 ^ idx]);
//       while (read(p[idx][0], &num, 1))
//       {
//         if (num % sieve != 0)
//           write(p[idx ^ 1][1], &num, 1);
//       }
//       close(p[1 ^ idx][1]);
//       idx ^= 1;
//     }
//     else
//     {
//       exit(0);
//     }
//   }
//   wait(0);
//   exit(0);
// }

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define R 0
#define W 1

int main1(int argc, char *argv[])
{
  int numbers[100], cnt = 0;
  int fd[2];
  for (int i = 2; i <= 35; i++)
  {
    numbers[cnt++] = i;
  }
  // 注意fork是在这个循环内进行的
  while (cnt > 0)
  {
    pipe(fd);
    if (fork() == 0)
    {
      int prime, this_prime = 0;
      // 关闭写的一端
      close(fd[W]);
      cnt = -1;
      // 读的时候，如果父亲还没写，就会block
      while (read(fd[R], &prime, sizeof(prime)) != 0)
      {
        // 设置当前进程代表的素数，然后筛掉能被当前素数整除的数
        if (cnt == -1)
        {
          this_prime = prime;
          cnt = 0;
        }
        else
        {
          // 把筛出来的接着放在number数组里？不对，这里cnt是重新从0开始计数的
          if (prime % this_prime != 0)
            numbers[cnt++] = prime;
        }
      }
      // printf("pid %d ,prime %d\n",getpid(),this_prime);
      printf("prime %d\n", this_prime);
      // 关闭读
      close(fd[R]);
      // WARN 注意！这里子进程并没有结束！子进程接下来继续执行while循环（cnt>0
      // 然后接着fork，注意此时子进程的子进程会获得和子进程一样的cnt和numbers
      // 也就是筛过的，而不是原始的
    }
    else
    {
      // 父进程里
      close(fd[R]);
      for (int i = 0; i < cnt; i++)
      {
        write(fd[W], &numbers[i], sizeof(numbers[0]));
      }
      close(fd[W]);
      wait(0);
      // 这个break，让父进程直接退出循环，从而结束了
      // 即父进程只是起了往第一个子进程传原始数据的作用

      break;
    }
  }
  exit(0);
}

// method 2
/**
 * After fork, both parent and child have file descriptors referring to the pipe.
 * 每个 pipe 有两个文件描述符，而且子进程和父进程都会分别创建两个对应的文件描述符，所以要分别都关闭。
 */
int waitNumber(int p_fd[2])
{
  int fd[2];
  int prime = -1;
  int number;
  int hasChild = -1;
  close(p_fd[W]);
  while (read(p_fd[R], &number, sizeof(number)) != 0)
  {
    if (prime == -1)
    {
      prime = number;
      fprintf(1, "prime %d\n", prime);
      // fprintf(1, "prime %d %d\n", prime, getpid());
    }
    else
    {
      if (number % prime != 0)
      {
        if (hasChild == -1)
        {
          hasChild = 1;
          pipe(fd);
          if (fork() == 0)
          {
            waitNumber(fd);
          }
          else
          {
            close(fd[R]);
          }
        }
        write(fd[W], &number, sizeof(number));
      }
    }
  }
  close(p_fd[R]);
  close(fd[W]);
  wait(0); // 如果不等待，会提前调用 exit 虽然看着结果是对的 但是有进程不会完全退出
  exit(0);
}

int main(int argc, char *argv[])
{
  int fd[2];
  pipe(fd);

  if (fork() == 0) // child
  {
    waitNumber(fd);
  }
  else
  {
    close(fd[R]);
    for (int i = 2; i < 36; i++)
    {
      write(fd[W], &i, sizeof(i));
    }
    close(fd[W]);
    wait(0);
  }
  exit(0);
}