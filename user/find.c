#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *
fmtname(char *path)
{
  // static char buf[DIRSIZ + 1];
  char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--) // 移动到 path 最最末尾， 然后找到第一个 / 后面的字符 如果没有 / 则就会移动到开头
    ;
  p++;
  return p;
}

void find(char *path, char *pattern)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0)
  {
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type)
  {
  case T_FILE:
    if (strcmp(pattern, fmtname(path)) == 0)
    {
      printf("%s\n", path);
    }
    break;

  case T_DIR:
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
      if (de.inum == 0)
        continue;
      // *memmove(void *dest, const void *src, unsigned int count);
      // 由src所指内存区域复制count个字节到dest所指内存区域。
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if (stat(buf, &st) < 0)
      {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      if (strcmp(".", de.name) == 0 || strcmp("..", de.name) == 0)
      {
        continue;
      }
      find(buf, pattern);
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(2, "params should be two!\n");
    exit(1);
  }

  char *path = argv[1];
  char *pattern = argv[2];
  find(path, pattern);
  exit(0);
}
