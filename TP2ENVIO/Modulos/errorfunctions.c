#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>


int sys_check_and_exit(int code, char *msg){
    if(code == -1){
        perror(msg);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}