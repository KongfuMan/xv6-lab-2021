#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFFER_SIZE 16
#define INDEX_READ 0
#define INDEX_WRITE 1

int
main(int argc, char *argv[]){
    int p2c[2]; // child read-end <--- parent write-end
    int c2p[2]; // parent read-end <--- child write-end

    // pipe(int pipefd[2]) creates a pipe, a unidirectional data channel that can be used
    // for interprocess communication. 
    // - pipefd[0] refersto the read end of the pipe.
    // - pipefd[1] refers to the write end of  the pipe.
    pipe(p2c);
    pipe(c2p); 
    char buf[BUFFER_SIZE];
    
    int child = fork();
    if (child < 0){
        fprintf(2, "fork a child process error.\n");
        exit(1);
    }
    else if (child == 0){
        // current is the context of child process. 
        close(p2c[INDEX_WRITE]);
        close(c2p[INDEX_READ]);
        
        int n = read(p2c[INDEX_READ], buf, BUFFER_SIZE); //blocking until there are data in the pipe buffer
        close(p2c[INDEX_READ]);
        if (n < 0){
            fprintf(2, "Child read error.\n");
            exit(1);
        }
        fprintf(2, "%d: received %s\n", getpid(), buf);

        char msg[] = "pong";
        n = write(c2p[INDEX_WRITE], msg, sizeof(msg));
        close(c2p[INDEX_WRITE]);
        if (n < 0){
            fprintf(2, "Child write error.\n");
            exit(1);
        }
    }
    else{
        // current is the context of parent process
        close(p2c[INDEX_READ]);
        close(c2p[INDEX_WRITE]);

        char msg[] = "ping";
        int n = write(p2c[INDEX_WRITE], msg, sizeof(msg));
        close(p2c[INDEX_WRITE]);
        if (n < 0){
            fprintf(2, "Parent write error.\n");
            exit(1);
        }
        n = read(c2p[INDEX_READ], buf, BUFFER_SIZE);
        close(c2p[INDEX_READ]);
        if (n < 0){
            fprintf(2, "Parent read error.\n");
            exit(1);
        }
        fprintf(2, "%d: received %s\n", getpid(), buf);
    }

    exit(0);
}