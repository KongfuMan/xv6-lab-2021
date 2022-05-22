#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MIN 2
#define MAX 35

// param:   `rdf` is read file descriptor for current process.
void
find_primes(int rfd){
    // fprintf(2, "P(id=%d) read fd is: %d\n", getpid(), rfd);
    int init;
    if (read(rfd, &init, 4) == 0){
        close(rfd);
        exit(0);
    }
    fprintf(2, "prime %d\n", init);

    int num;
    int p[2];
    if(pipe(p) == -1){
        close(p[0]);
        close(p[1]);
        close(rfd);
        fprintf(2, "create pipe error");
        exit(0);
    }
    int child = fork();
    if (child < 0){
        fprintf(2, "For child process error.\n");
        close(p[0]);
        close(p[1]);
        close(rfd);
        exit(1);
    }
    else if (child > 0){
        //parent: continue handler oter numbers
        close(p[0]);
        while(read(rfd, &num, 4) == 4){
            if (num % init == 0){
                continue;
            }
            write(p[1], &num, 4);
        }
        close(rfd);
        close(p[1]);
        wait(0); //wait until child process 
    }
    else{
        // child: recursively handle the next level
        close(p[1]);
        find_primes(p[0]);
        close(p[0]);
    }
    exit(0);
}

int
main(int argc, char *argv[]){
    int num = MIN;
    int p[2];  // p[1] write ---> p[0]
    pipe(p);
    // fprintf(2, "P(id=%d) read fd is: %d\n", getpid(), p[0]);
    int cid = fork();
    if (cid < 0){
        fprintf(2, "Fork child process error");
        exit(1);
    }
    else if (cid == 0){
        // child
        close(p[1]); //To this step, the write operation of parent has been done???
        find_primes(p[0]);
        close(p[0]);
    }
    else{
        close(p[0]);
        while(num <= MAX){
            write(p[1], &num, 4);
            num++;
        }
        close(p[1]);
        wait(0);
    }
    exit(0);
}