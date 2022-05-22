#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(2, "Usage: sleep time...\n");
        exit(1);
    }

    int time = atoi(argv[1]);
    int ret = sleep(time);
    if (ret < 0){
        fprintf(2, "Sleep error");
        exit(1);
    }
    exit(0);
}