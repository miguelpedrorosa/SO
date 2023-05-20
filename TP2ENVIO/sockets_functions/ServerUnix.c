#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "socketFunction.h"

#define SERVER_ENDPOINT "SocketUNIX"

int main(){
    int server_socket = un_serversocket_init(SERVER_ENDPOINT);
    
    while(1){
        int client_socket = un_serversocket_accept(server_socket);

        pthread_t thread;

         int *pt = malloc(sizeof(int));
        *pt = client_socket;
        pthread_create(&thread, NULL, thHandleClient, pt);
    }

    return 0;
}