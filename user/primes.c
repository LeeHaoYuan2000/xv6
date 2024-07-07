#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX 36
#define FIRST_PRIME 2
int primes_sieve(int last_pipe_read_fd);

int main(int argc,char *argv[]){
    int pipefd[2];

    if(pipe(pipefd) == -1){
        fprintf(2,"Create Pipe Fail \n");
        exit(1);
    }

    int cpid = fork();

    if(cpid == 0){
        close(pipefd[1]);
        primes_sieve(pipefd[0]);
        exit(0);
    }
    else{

		close(pipefd[0]);

        for(int i=2 ; i < 36; i++){
            write(pipefd[1], &i, sizeof(int));
        }
      close(pipefd[1]);
    }

    wait(0);//wait for all child process done.
    exit(0);
}

int primes_sieve(int last_pipe_read_fd){

   // printf("last_pipe_read_fd %d \n", last_pipe_read_fd);

    int pipefd[2];
    int buf;
    
    int       first_prime ; //the first number always the prime
    int res = read(last_pipe_read_fd, &first_prime, sizeof(int));
    printf("res1 == %d \n",res);
    if(res < 1){ // make sure the recursion will return
        close(last_pipe_read_fd);
        printf("Done \n");
        return 0;
    }

    fprintf(1,"primes %d \n", first_prime);

    if(pipe(pipefd) == -1){
        fprintf(2,"Create Pipe Faile \n");
        exit(1);
    }

    int cpid = fork();

    if(!cpid){
        close(last_pipe_read_fd);
		close(pipefd[1]);
        primes_sieve(pipefd[0]);
        return 0;
    }
    else{
		close(pipefd[0]);
         do {
            res = read(last_pipe_read_fd, &buf, sizeof(int)); 
            if(buf % first_prime != 0){
                write(pipefd[1], &buf, sizeof(int));
            }
        }while(res > 0);
        close(pipefd[1]);
    }

    wait(0);
   return 0;
}

// int generate_natural();  // -> out_fd
// int prime_filter(int in_fd, int prime);  // -> out_fd

// int
// main(int argc, char *argv[])
// {
// 	int prime; 
	
// 	int in = generate_natural();
// 	while (read(in, &prime, sizeof(int))) {
// 		// printf("prime %d: in_fd: %d\n", prime, in);  // debug
// 		printf("prime %d\n", prime); 
// 		in = prime_filter(in, prime);
// 	}

// 	exit(0);
// }

// // 生成自然数: 2, 3, 4, ..< MAX
// int
// generate_natural() {
// 	int out_pipe[2];
	
// 	pipe(out_pipe);

// 	if (!fork()) {
// 		for (int i = FIRST_PRIME; i < MAX; i++) {
// 			write(out_pipe[1], &i, sizeof(int));
// 		}
// 		close(out_pipe[1]);

// 		exit(0);
// 	}

// 	close(out_pipe[1]);

// 	return out_pipe[0];
// }

// // 素数筛
// int 
// prime_filter(int in_fd, int prime) 
// {
// 	int num;
// 	int out_pipe[2];

// 	pipe(out_pipe);

// 	if (!fork()) {
// 		while (read(in_fd, &num, sizeof(int))) {
// 			if (num % prime) {
// 				write(out_pipe[1], &num, sizeof(int));
// 			}
// 		}
// 		close(in_fd);
// 		close(out_pipe[1]);
		
// 		exit(0);
// 	}

// 	close(in_fd);
// 	close(out_pipe[1]);

// 	return out_pipe[0];
// }