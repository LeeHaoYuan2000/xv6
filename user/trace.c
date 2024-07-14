#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("useage:trace <sys_call bits_32bits> <Commond> \n");
    }

    char *tarce_mask_bits = argv[1];

    for(int i = 0; i < strlen(tarce_mask_bits) ; i++){

        if( !((tarce_mask_bits[i] > '0') && (tarce_mask_bits[i] < '9')) ){ //if the trace bit is not a number
            fprintf(2,"Please trace_mask_bits can only be numbers \n");
            exit(1);
        }
    }

    unsigned int mask_bits_32 = atoi(tarce_mask_bits);

    char *v[argc - 2];

    for(int i = 0; i < (argc-2); i++){
        v[i] = argv[i + 2];
    }

    trace(mask_bits_32);

    int pid = fork();

    if(pid == 0){ //the child process
        exec(v[0], v);
        exit(0);
    } 


    wait(0);
    exit(0);
}