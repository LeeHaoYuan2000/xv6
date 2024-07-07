#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int arc,char *argv[]){

    if(arc > 2){
        fprintf(2,"Only allow to input 1 argument in Sleep!! \n");
        exit(1);// the input argument numebr is wrong
    }


    if(strlen(argv[1]) > 256){
        fprintf(2,"Argument Too Long, Please input arg less than 256 Bytes!  \n");
        exit(1);
    }

    //if the argument is not a number
    for(int i =0 ; i < strlen(argv[1]) -1 ; i++){ //the last charcter is enter so "strlen(argv[1]) -1" 

        if( !((argv[1][i] <= '9')&&(argv[1][i] >= '0')) ){ //if the char is not a number
            fprintf(2,"Argument only can input the numbers! \n");
            exit(0);
        }

    }

    int sleep_time = atoi(argv[1]);

    sleep(sleep_time);

    exit(0); //exit successfully
}