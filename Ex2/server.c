#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#define SERVER_ENDPOINT "/tmp/server_socket"

int un_socket_server_init(const char *serverEndPoint) {
    int serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Erro ao criar socket do servidor");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un serverAddr;
    memset(&serverAddr, 0, sizeof(struct sockaddr_un));
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, serverEndPoint, sizeof(serverAddr.sun_path) - 1);

    unlink(serverEndPoint);  // Remover arquivo do socket, se já existir

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_un)) == -1) {
        perror("Erro ao vincular o socket do servidor");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 1) == -1) {
        perror("Erro ao ouvir conexões no socket do servidor");
        exit(EXIT_FAILURE);
    }

    return serverSocket;
}

int un_socket_server_accept(int serverSocket) {
    int clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == -1) {
        perror("Erro ao aceitar a conexão do cliente");
        exit(EXIT_FAILURE);
    }

    return clientSocket;
}

void send_process_list(int clientSocket) {
    FILE *processList = popen("/bin/ps", "r");
    if (processList == NULL) {
        perror("Erro ao executar o comando ps");
        exit(EXIT_FAILURE);
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), processList) != NULL) {
        send(clientSocket, buffer, strlen(buffer), 0);
    }

    pclose(processList);
}

void send_system_date(int clientSocket) {
    FILE *systemDate = popen("/bin/date", "r");
    if (systemDate == NULL) {
        perror("Erro ao executar o comando date");
        exit(EXIT_FAILURE);
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), systemDate);
    send(clientSocket, buffer, strlen(buffer), 0);

    pclose(systemDate);
}

int main() {
    int serverSocket = un_socket_server_init(SERVER_ENDPOINT);
    printf("Servidor iniciado. Aguardando conexões...\n");

    while (1) {
        int clientSocket = un_socket_server_accept(serverSocket);
        printf("Cliente conectado. Aguardando requisição...\n");

        char request[256];
        int bytesRead = recv(clientSocket, request, sizeof(request) - 1, 0);
        if (bytesRead == -1) {
            perror("Erro ao receber a requisição do cliente");
            exit(EXIT_FAILURE);
        }

        request[bytesRead] = '\0';

        if (strcmp(request, "ps\n") == 0) {
            send_process_list(clientSocket);
        } else if (strcmp(request, "date\n") == 0) {
            send_system_date(clientSocket);
        } else {
            send(clientSocket, "Comando inválido\n", 17, 0);
        }

        close(clientSocket);
        printf("Requisição concluída. Cliente desconectado.\n");
    }

    close(serverSocket);
    unlink(SERVER_ENDPOINT);

    return 0;
}