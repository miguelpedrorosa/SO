
int un_serversocket_init(const char *ServerEndPoint);
int un_clientsocket_init(const char *ServerEndPoint);
int un_serversocket_accept(int serverSocket);

int tcp_serversocket_init(int serverPort);
int tcp_clientsocket_init(const char *host, int port);
int tcp_serversocket_accept(int serverSocket);

void *thHandleClient(void *_args);
int readline(int socketfd, char *buffer, int len);
void* thHandleClient(void *_args);
void handle_date(int client_socket);
void handle_ps(int client_socket);
void *handle_client(void *arg);
int writen(int fd, const void *buf, size_t count);