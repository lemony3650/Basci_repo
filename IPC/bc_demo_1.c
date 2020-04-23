#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define oops(m,x)   {perror(m);exit(x);}
int fatal(char *mess[]);

// read from stdin and convert into to RPN,send down pipe
// then read from other pipe and print to user 
// Uses fdopen() to convert a file descriptor to a stream
// parent : bc
void be_bc(int todc[2],int fromdc[2])
{
    int num1,num2;
    char operation[BUFSIZ], message[BUFSIZ], *fgets();
    FILE *fpout,*fpin,*fdopen();

    //setup bc wirte to dc
    close(todc[0]);     //close parent read to write
    close(fromdc[1]);   //close child write

    //file describptor as para
    fpout = fdopen(todc[1],"w");        //not fopen throw filename to open
    fpin  = fdopen(fromdc[0],"r");
    if(fpout == NULL || fpin == NULL)
        fatal("Error convering pipes to strams");

    //stdin read string to message
    while(printf("tinybc: "),fgets(message,BUFSIZ,stdin) != NULL){
        //sscanf分析输入 传值
        if(sscanf(message,"%d %[- + * / ^]%d",&num1,operation,&num2) != 3){
            printf("syntax error\n");
            continue;   
        }
        if (fprintf(fpout,"%d\n %d\n %c\np\n",num1,num2,*operation) == EOF)
            fatal("Error writing");

        fflush(fpout);
        if (fgets(message,BUFSIZ,fpin) == NULL)     //get result from fpin(pipe bc)
            break;
        printf("%d %c %d = %s",num1,*operation,num2,message);
    }
    fclose(fpout);
    fclose(fpin);
}

void be_dc(int in[2],int out[2])
//              tobc     formdc
{
    if(dup2(in[0],0) == -1)         //bc read cp to stdin
        oops("dc:cannot redirect stdin",3);
    close(in[0]);
    close(in[1]);

    if(dup2(out[1],1) == -1)        //dc wirte cp to stdout
        oops("dc:cannot redirect stdout",4);
    close(out[1]);
    close(out[0]);

    execlp("dc","dc","-",NULL);
    oops("cannot run dc",5);
}

int fatal(char *mess[])
{
    fprintf(stderr,"Error: %s\n",mess);
    exit(1);
}

int main()
{
    pid_t pid;
    int todc[2],fromdc[2];

    //create two pipe
    if (pipe(todc) == -1 || pipe(fromdc) == -1)
        oops("pipe failed",1);

    if ((pid = fork()) == -1)
        oops("cannot fork",2);

    //parent bc    child dc
    if (pid == 0)               
        be_dc(todc,fromdc);
    else{
        be_bc(todc,fromdc);
        wait(NULL);
    }
}