#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "usage: xargs cmd [...]\n");
        exit(1);
    }
    int flag = 1;
    char *new_argv[MAXARG];
    for (int i = 1; i < argc; i++) // 拷贝剩余的的参数到新的数组
        new_argv[i - 1] = argv[i];
    while (flag)
    {
        // p 开始指向数组的起始位置
        // 下面每次读取一个字符到数组的对应位置，然后修改指针的位置，
        // 如果读取到了'\n',则退出循化，这时候把最后一个字符 设置成0
        char buf[512], *p = buf;
        while ((flag = read(0, p, 1)) && *p != '\n')
            p++;
        if (flag == 0) // 没有数据了
            exit(0);
        *p = 0; // 字符串结尾

        new_argv[argc - 1] = buf;
        if (fork() == 0)
        {
            exec(argv[1], new_argv);
            exit(0);
        }
        wait(0);
    }
    exit(0);
}