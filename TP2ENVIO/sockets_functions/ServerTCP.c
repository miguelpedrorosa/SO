#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "socketFunction.h"

#define PORT 8080

int main()
{
    int server_socket = tcp_serversocket_init(PORT);
    if (server_socket < 0)
    {
        printf("Failed to create server socket\n");
        return 1;
    }
    printf("Server socket created successfully\n");

    while (1)
    {
        int client_socket = tcp_serversocket_accept(server_socket);
        if (client_socket < 0)
        {
            printf("Failed to accept client connection\n");
        }
        printf("Client connection accepted. Client socket: %d\n", client_socket);

        pthread_t thread;

        int *pt = malloc(sizeof(int));
        if (pt == NULL)
        {
            printf("Failed to allocate memory\n");
        }

        *pt = client_socket;
        int result = pthread_create(&thread, NULL, thHandleClient, pt);
        if (result != 0)
        {
            printf("Failed to create thread\n");
            free(pt);
        }
        printf("Thread created successfully\n");
    }

    close(server_socket);
    return 0;
}