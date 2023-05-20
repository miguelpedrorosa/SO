#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "socketFunction.h"

#define SERVER_ENDPOINT "SocketUNIX"
#define PORT 8080

int main()
{
    int server_socket_un = un_serversocket_init(SERVER_ENDPOINT);

    int server_socket_tcp = tcp_serversocket_init(PORT);

    pthread_t ThreadHandleClient;

    printf("Servidor em execução\n");

    while (1)
    {
        int client_socket = un_serversocket_accept(server_socket_un);

        int *pt = malloc(sizeof(int));
        *pt = client_socket;
        pthread_create(&ThreadHandleClient, NULL, thHandleClient, pt);

        pthread_detach(ThreadHandleClient);

        client_socket = tcp_serversocket_accept(server_socket_tcp);

        pthread_t thread;

        pt = malloc(sizeof(int));
        *pt = client_socket;
        pthread_create(&thread, NULL, thHandleClient, pt);

        pthread_detach(ThreadHandleClient);
    }
    //unlink("SocketUnix")
    close(server_socket_un);
    close(server_socket_tcp);
}