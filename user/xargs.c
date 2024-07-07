#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int main(int argc,char *argv[]){
    unsigned int p = 0;
    char *v[MAXARG];
    int cnt;
    char ch;
    int Blank_Cnt = 0;
    char *string = malloc(sizeof(char)*1024);

    if(argc < 2){
        fprintf(2,"Usage: xargs <Options> \n");
        exit(1);
    }

    for(cnt = 1 ; cnt < argc ; cnt++){  // xargs grep help
        v[cnt - 1] = argv[cnt];    //input "grep help"
    }

    cnt -= 1;
    v[cnt] = string;

    while( read(0, &ch, sizeof(char)) > 0 ){

        if(ch == ' ' || ch == '\t'){
            Blank_Cnt += 1;
            continue;
        }

        

        if(Blank_Cnt == 0){
            if(p >= 2048){
                fprintf(2,"the argument is too long");
            }

            if(ch == '\n'){
                continue;
            }

            *(string + p) = ch;
            p += 1;
        }
         else{ // Blank_Cnt != 0 -> there is a now argument
            p = 0;
            Blank_Cnt = 0;
            cnt++;

            //WRITE char
            string = malloc(sizeof(char)*1024);
            v[cnt] = string;
            *(string + p) = ch;
            p++;
        }

    }


    if(fork() == 0){ // runs the child thread
        exec(v[0], (char**)v);
        exit(0);
    }

    wait(0);
    exit(0);
}

// #define is_blank(chr) (chr == ' ' || chr == '\t') 

// int
// main(int argc, char *argv[])
// {
// 	char buf[2048], ch;
// 	char *p = buf;
// 	char *v[MAXARG];
// 	int c;
// 	int blanks = 0;
// 	int offset = 0;

// 	if(argc <= 1){
// 		fprintf(2, "usage: xargs <command> [argv...]\n");
// 		exit(1);
// 	}

// 	for (c = 1; c < argc; c++) {
// 		v[c-1] = argv[c];
// 	}
// 	--c;

// 	while (read(0, &ch, 1) > 0) {
// 		if (is_blank(ch)) {
// 			blanks++;
// 			continue;
// 		}

// 		if (blanks) {  // 之前有过空格
// 			buf[offset++] = 0;

// 			v[c++] = p;
// 			p = buf + offset;

// 			blanks = 0;
// 		}

// 		if (ch != '\n') {
// 			buf[offset++] = ch;
// 		} else {
// 			v[c++] = p;
// 			p = buf + offset;

// 			if (!fork()) {
// 				exit(exec(v[0], v));
// 			}
// 			wait(0);
			
// 			c = argc - 1;
// 		}
// 	}

// 	exit(0);
// }