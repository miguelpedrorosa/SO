#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "errorfunctions.h"

#define MAX_REQUEST_LEN 1024

int un_serversocket_init(const char *ServerEndPoint)
{
    int sockfd;

    struct sockaddr_un serveraddr;

    // criação de um socket UNIX stream

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        sys_check_and_exit(sockfd, "ERROR: criação de um socket");
    }

    // Registar endereco local de modo a que os clientes nos possam contactar
    memset((char *)&serveraddr, 0, sizeof(serveraddr));

    // definição das família de protocolos como AF_UNIX
    serveraddr.sun_family = AF_UNIX;

    // copia o nome do ServerEndPoint fornecido para a estrutura de endereço do servidor
    strcpy(serveraddr.sun_path, ServerEndPoint);

    // vincula-se o socket ao ServerEndPoint fornecido
    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("Erro ao vincular o socket ao endpoint");
        close(sockfd);
        exit(-1);
    }

    // Inicia a escuta por conexões
    if (listen(sockfd, 5) < 0)
    {
        perror("Erro ao iniciar a escuta por conexões");
        close(sockfd);
        exit(-1);
    }

    return sockfd;
}

int un_clientsocket_init(const char *ServerEndPoint)
{
    int sockfd;

    struct sockaddr_un serveraddr;

    // criação de um socket UNIX stream

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        sys_check_and_exit(sockfd, "ERROR: criação de um socket");
    }

    memset((char *)&serveraddr, 0, sizeof(serveraddr));

    // definição das família de protocolos como AF_UNIX
    serveraddr.sun_family = AF_UNIX;

    // copia o nome do ServerEndPoint fornecido para a estrutura de endereço do servidor
    strcpy(serveraddr.sun_path, ServerEndPoint);

    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("Erro ao vincular o socket ao endpoint");
        close(sockfd);
        exit(-1);
    }

    return sockfd;
}

int un_serversocket_accept(int serverSocket)
{
    int newSockFd;
    struct sockaddr clientaddr;
    socklen_t dim_client = sizeof(clientaddr);
    newSockFd = accept(serverSocket, (struct sockaddr *)(&clientaddr), &dim_client);
    if (newSockFd < 0)
    {
        sys_check_and_exit(newSockFd, "ERROR: Aceitação do socket");
    }
    return newSockFd;
}

// TCP funções

int tcp_serversocket_init(int serverPort)
{
    int sockfd;

    struct sockaddr_in serv_addr;

    // Cria um socket TCP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        sys_check_and_exit(sockfd, "ERROR: criação de um socket");
    }

    memset((char *)&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(serverPort);

    // vincula-se o socket ao ServerEndPoint fornecido
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Erro ao vincular o socket ao endpoint");
        close(sockfd);
        exit(-1);
    }

    // Inicia a escuta por conexões
    if (listen(sockfd, 5) < 0)
    {
        perror("Erro ao iniciar a escuta por conexões");
        close(sockfd);
        exit(-1);
    }

    return sockfd;
}

int tcp_serversocket_accept(int serverSocket)
{
    int newSockFd;
    struct sockaddr_in clientaddr;
    socklen_t dim_client = sizeof(clientaddr);
    newSockFd = accept(serverSocket, (struct sockaddr *)(&clientaddr), &dim_client);
    if (newSockFd < 0)
    {
        sys_check_and_exit(newSockFd, "ERROR: Aceitação do socket");
    }
    return newSockFd;
}

int tcp_clientsocket_init(const char *host, int port)
{
    int sockfd;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    long ServerAddress;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        sys_check_and_exit(sockfd, "ERROR: criação de um socket");
    }

    if ((server = gethostbyname(host)) != NULL)
    {
        memcpy(&ServerAddress, server->h_addr_list[0], server->h_length);
    }
    else if ((ServerAddress = inet_addr(host)) == -1)
    {
        perror("Impossível determinar endereço IP da máquina");
        exit(EXIT_FAILURE);
    }

    memset((char *)&serveraddr, 0, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;

    serveraddr.sin_addr.s_addr = ServerAddress;

    // Define a porta do servidor como port
    serveraddr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("Erro ao vincular o socket ao endpoint");
        close(sockfd);
        exit(-1);
    }

    return sockfd;
}

// Outras

int readline(int socketfd, char *buffer, int len)
{
    int i;
    char aux;
    for (i = 0; i < len; ++i)
    {
        int n = read(socketfd, &aux, sizeof(aux));
        if (n <= 0)
        {
            return n;
        }
        if (aux == '\n')
        {
            buffer[i] = '\0';
            return i;
        }
        buffer[i] = aux;
    }

    buffer[i] = '\0';

    return -2;
}

int handle_ps(int client_socket)
{
    pid_t retfork;
    int error;

    error = dup2(client_socket, STDOUT_FILENO);
    if (error < 0)
    {
        perror("ERROR: ");
        exit(EXIT_FAILURE);
    }

    retfork = fork();
    if (retfork < 0)
    {
        perror("ERROR: Criação de um processo filho");
        exit(EXIT_FAILURE);
    }

    if (retfork == 0)
    {
        execlp("/bin/ps", "ps", NULL);
    }

    wait(NULL);

    return 1;
}

int handle_date(int client_socket)
{
    pid_t retfork;
    int error;

    error = dup2(client_socket, STDOUT_FILENO);
    if (error < 0)
    {
        perror("ERROR: ");
        exit(EXIT_FAILURE);
    }

    retfork = fork();
    if (retfork < 0)
    {
        perror("ERROR: Criação de um processo filho");
        exit(EXIT_FAILURE);
    }

    if (retfork == 0)
    {
        execlp("/bin/date", "date", NULL);
    }

    wait(NULL);

    return 1;
}

void *handle_client(void *arg)
{
    int *pt = (int *)arg;
    int client_socket = *pt;

    char buffer[MAX_REQUEST_LEN];
    int bytes_read;

    while (1)
    {
        // Lê a mensagem enviada pelo cliente
        bytes_read = readline(client_socket, buffer, MAX_REQUEST_LEN - 1);
        if (bytes_read <= 0)
        {
            break;
        }
        buffer[bytes_read] = '\0'; // Caracter nulo no final da string
        // Verificação de qual foi o serviço solicitado pelo cliente
        if (strcmp(buffer, "ps") == 0)
        {
            // Lista de processos
            handle_ps(client_socket);
        }
        else if (strcmp(buffer, "date") == 0)
        {
            // a data atual do sistema
            handle_date(client_socket);
        }
        else
            // mensagem inválida
            write(client_socket, "Serviço inválido.\n", strlen("Serviço inválido.\n"));
    }

    // fecha o client socket
    close(client_socket);
    free(pt);

    return NULL;

}

void *thHandleClient(void *args)
{

    handle_client(args);
    
    return NULL;
}

int writen(int fd, const void *buf, size_t count)
{
    int nBytesWR;
    const char *ptbuf = buf;
    size_t bytesToSend = count;
    while (bytesToSend > 0)
    {
        nBytesWR = write(fd, ptbuf, bytesToSend);
        if (nBytesWR <= 0)
            return nBytesWR;
        ptbuf += nBytesWR;
        bytesToSend -= nBytesWR;
    }
    return count;
}

