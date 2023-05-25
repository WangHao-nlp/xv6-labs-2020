#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// c    matches any literal character c
// .    matches any single character
// ^    matches the beginning of the input string
// $    matches the end of the input string
// *    matches zero or more occurrences of the previous character

    /* match: search for regexp anywhere in text */
    // 匹配：在文本中的任何位置搜索正则表达式
    // int match(char *regexp, char *text)
    // {
    //     if (regexp[0] == '^')
    //         return matchhere(regexp+1, text);
    //     do {    /* must look even if string is empty */
    //         if (matchhere(regexp, text))
    //             return 1;
    //     } while (*text++ != '\0');
    //     return 0;
    // }

    // /* matchhere: search for regexp at beginning of text */
    // matchhere：在文本开头搜索regexp
    // int matchhere(char *regexp, char *text)
    // {
    //     if (regexp[0] == '\0')
    //         return 1;
    //     if (regexp[1] == '*')
    //         return matchstar(regexp[0], regexp+2, text);
    //     if (regexp[0] == '$' && regexp[1] == '\0')
    //         return *text == '\0';
    //     if (*text!='\0' && (regexp[0]=='.' || regexp[0]==*text))
    //         return matchhere(regexp+1, text+1);
    //     return 0;
    // }

    // /* matchstar: search for c*regexp at beginning of text */
    // matchstar：在文本开头搜索c*regexp
    // int matchstar(int c, char *regexp, char *text)
    // {
    //     do {    /* a * matches zero or more instances */
    //         if (matchhere(regexp, text))
    //             return 1;
    //     } while (*text != '\0' && (*text++ == c || c == '.'));
    //     return 0;
    // }

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int match(char *re, char *text){
    if(re[0]=='^'){
        return matchhere(re+1, text);
    }
    do{
        if(matchhere(re, text)){
            return 1;
        }
    }while(*text++!='\0');
    return 0;
}

int matchhere(char *re, char *text){
    if(re[0]=='\0'){
        return 1;
    }
    if(re[1]=='*'){
        return matchstar(re[0], re+2, text);
    }
    if(re[0]=='$'&&re[1]=='\0'){
        return *text == '\0';
    }
    if(*text!='\0'&&(re[0]=='.'||re[0]==*text)){
        return matchhere(re+1, text+1);
    }
    return 0;
}

int matchstar(int c, char *re, char *text){
    do{
        if(matchhere(re, text)){
            return 1;
        }
    }while(*text!='\0'&&(*text++==c||c=='.'));
    return 0;
}


void find(char *path, char *target){
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd=open(path, 0))<0){
        fprintf(2, "find: cannot open %s\n", path); // 2指定输出的文件描述符
        return;
    }

    if(fstat(fd, &st)<0){
        fprintf(2, "find: cannot stat %s\n", path); // 2指定输出的文件描述符
        close(fd);
        return;
    }

    switch(st.type){
        case T_FILE:
            // if(strcmp(path+strlen(path)-strlen(target), target)==0){
            //     printf("%s\n", path);
            // }
            if(match(target, path)){
                printf("%s\n", path);
            }
            break;
        case T_DIR:
            if (strlen(path)+1+DIRSIZ+1>sizeof buf){
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ ='/';
            while(read(fd, &de, sizeof(de))==sizeof(de)){
                if (de.inum==0){
                    continue;
                }
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st)<0){
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                if(strcmp(buf+strlen(buf)-2, "/.")!=0&&strcmp(buf+strlen(buf)-3, "/..")!=0){
                    find(buf, target); // 递归查找
                }
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]){
    if(argc<3){
        exit(0);
    }
    char target[512];
    target[0] = '/';
    strcpy(target+1, argv[2]);
    find(argv[1], target);
    exit(0);
}