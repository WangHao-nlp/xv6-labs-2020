#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 带参数列表，执行某个程序
void run(char *program, char **args){
    if(fork()==0){
        //子进程
        exec(program, args);
        exit(0);
    }
    return; //父进程
}

int main(int argc, char *argv[]){
    char buf[2048]; //读入时使用的内存池
    char *p = buf, *last_p = buf;// 当前参数结束、开始指针
    char *argsbuf[128]; //全部参数列表
    char **args = argsbuf;
    for(int i=1; i<argc; i++){
        *args = argv[i];
        args++;
    }
    // 开始读入参数
    char **pa = args;
    while(read(0, p, 1)!=0){
        if(*p==' '||*p=='\n'){
            *p = '\0';
            *(pa++) = last_p;
            last_p = p+1;
            if(*p=='\n'){
                *pa = 0;
                run(argv[1], argsbuf);
                pa = args;
            }
        }
        p++;
    }
    if(pa!=args){
        *p = '\0';
        *(pa++) = last_p;
        *pa = 0;
        run(argv[1], argsbuf);
    }
    while(wait(0)!=-1){};
    exit(0);
}