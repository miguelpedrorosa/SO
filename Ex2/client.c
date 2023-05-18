#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_ENDPOINT "/tmp/server_socket"

int un_socket_client_init(const char *serverEndPoint) {
    int clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Erro ao criar socket do cliente");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un serverAddr;
    memset(&serverAddr, 0, sizeof(struct sockaddr_un));
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, serverEndPoint, sizeof(serverAddr.sun_path) - 1);

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_un)) == -1) {
        perror("Erro ao conectar ao servidor");
        exit(EXIT_FAILURE);
    }

    return clientSocket;
}

void receive_response(int clientSocket) {
    char buffer[256];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == -1) {
        perror("Erro ao receber a resposta do servidor");
        exit(EXIT_FAILURE);
    }

    buffer[bytesRead] = '\0';
    printf("Resposta do servidor: %s", buffer);
}

int main() {
    int clientSocket = un_socket_client_init(SERVER_ENDPOINT);
    printf("Conexão estabelecida com o servidor.\n");

    char request[256];
    printf("Digite 'ps' para obter a lista de processos ou 'date' para obter a data do sistema: ");
    fgets(request, sizeof(request), stdin);

    send(clientSocket, request, strlen(request), 0);
    receive_response(clientSocket);

    close(clientSocket);

    return 0;
}