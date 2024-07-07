#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int arc,char *argv[]){
    int pipefd[2];
    char buf;
    int cpid;

    if(pipe(pipefd) == -1){
        fprintf(2,"create pipe fail \n");
        exit(1);
    }

    cpid = fork();
    if(cpid == -1){
        fprintf(2,"fork() fail \n");
        exit(1);
    }

    //write byte to pipe
    if(cpid == 0){// this runs by child, child write to pipe
        while(read(pipefd[0], &buf, 1) <=0 ){
        }
        fprintf(1, "%d: received ping \n",buf);
        int pid = getpid();
        write(pipefd[1], &pid,1);
        close(pipefd[0]);
        close(pipefd[1]);
    }
    else{ //parent write to pipe
        int pid = getpid();
        write(pipefd[1], &pid,1);
        wait(0);//wait child finish its process
        read(pipefd[0], &buf, 1);
        fprintf(1, "%d: received pong \n",buf);
        close(pipefd[0]);
        close(pipefd[1]);
    }

    exit(0);
}