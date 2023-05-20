#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "socketFunction.h"

#define MAX_REQUEST_LEN 1024

#define SERVER_ENDPOINT "SocketUNIX"

int main()
{
    int client_socket, nbytes;
    char buffer[MAX_REQUEST_LEN];
    char command[10];

    // iniciação do socket do client
    client_socket = un_clientsocket_init(SERVER_ENDPOINT);

    // Comando desejado
    printf("Escolha o comando desejado (ps/date):");
    fgets(command, sizeof(command), stdin);
    // printf("%s",command);

    if (strcmp(command, "ps\n") == 0 || strcmp(command, "date\n") == 0)
    {
        strcpy(buffer, command);
    }
    else
    {
        perror("Comando ínvalido");
        exit(EXIT_FAILURE);
    }

    if ((nbytes = writen(client_socket, buffer, strlen(buffer))) < 0)
    {
        perror("ERROR: função writen");
        exit(EXIT_FAILURE);
    }

    if ((nbytes = recv(client_socket, buffer,sizeof(buffer)-1,0)) < 0)
    {
        // printf("%d\n",nbytes);
        perror("ERROR: função read");
        exit(EXIT_FAILURE);
    }

    buffer[nbytes] = '\0';

    // exibir a resposta do servidor
    printf("Resposta do servidor: \n %s\n", buffer);

    close(client_socket);

    return 0;
}
