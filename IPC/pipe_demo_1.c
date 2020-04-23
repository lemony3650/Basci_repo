#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


//创建管道并使用管道与标准输入输出  pipe use : 1 ->>>>>> 0
//生成的pipe[2]感觉像两个文件一样可以暂存，用File *描述
int main()
{
    int len,i,pipe_fd[2];
    char buf[BUFSIZ];

    if( pipe(pipe_fd) == -1 ){
        perror("could not make pipe");
        exit(1);
    }
    printf("Got a pipe! the file descripors:{%d %d}\n",pipe_fd[0],pipe_fd[1]);

    //read from stdin and write into pipe[1] 
    //read from pipe[1] and print(stdout)
    while( fgets(buf,BUFSIZ,stdin) ){           //stdin
        len = strlen(buf);                      //get string adn to write pipe
        if(write(pipe_fd[1],buf,len) != len){     
            perror("writing to pipe");
            break;
        }

        for(i = 0; i < len; i++)                //buf 赋值X to diff pipe read buf value
            buf[i] = 'X';

        len = read(pipe_fd[0],buf,BUFSIZ);        //read pipe to watch buf value
        if( len == -1 ){
            perror("reading to pipe");
            break;
        }
        if(write(1,buf,len) != len){            //buf to stdout view
            perror("writing to pipe");
            break;
        }
    }
}