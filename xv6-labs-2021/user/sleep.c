#include "kernel/types.h" // 一些基本的类型定义
#include "kernel/stat.h" // 文件状态
#include "user/user.h"  // System call 和 utility functions

int main (int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(2, "Usage: sleep <number>\n");
        exit(1);
    }

    int time = atoi(argv[1]);
    if (time <= 0) {
        fprintf(2, "Invalid sleep time: %s\n", argv[1]);
        exit(1);
    }
    
    if (sleep(time) != 0) {
        fprintf(2, "sleep failed\n");
        exit(1);
    }
    exit(0);
}