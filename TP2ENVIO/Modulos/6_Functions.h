typedef struct{
    int client_socket;
    Matrix *m1;
    Matrix *m2;
}ThreadInformation;

void send_matrix(int socket, const Matrix *matrix );
Matrix *receive_matrix(int socket);
void *handle_client_Mul(void *args);