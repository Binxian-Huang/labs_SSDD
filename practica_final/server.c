#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t mutex_mensaje = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_mensaje = PTHREAD_COND_INITIALIZER;
int mensaje_no_copiado = 1;

int sendMessage(int socket_fd, char *buffer, int size) {
    int bytes_sent;
    int bytes_left = size;

    do {
        bytes_sent = write(socket_fd, buffer, bytes_left);
        bytes_left = bytes_left - bytes_sent;
        buffer = buffer + bytes_sent;
    } while ((bytes_sent >=0) && (bytes_left > 0));

    if (bytes_sent < 0) {
        return -1;
    } else {
        return 0;
    }
}

ssize_t readLine(int socket_fd, char *buffer, size_t size) {
    ssize_t numRead;
    size_t totRead;
    char *buf;
    char ch;

    if (size <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;
    totRead = 0;

    for (;;) {
        numRead = read(socket_fd, &ch, 1);

        if (numRead == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                return -1;
            }
        } else if (numRead == 0) {
            if (totRead == 0) {
                return 0;
            } else {
                break;
            }
        } else {
            if (ch == '\n') {
                break;
            }
            if (ch == '\0') {
                break;
            }
            if (totRead < size - 1) {
                totRead++;
                *buf++ = ch;
            }
        }
    }
    *buf = '\0';
    return totRead;
}

void treat_message(void *new_socket_fd) {
    pthread_mutex_lock(&mutex_mensaje);
    int socket_fd = *((int *) new_socket_fd);
    pthread_cond_signal(&cond_mensaje);
    pthread_mutex_unlock(&mutex_mensaje);

    if (close(socket_fd) == -1) {
        perror("Error closing connection socket in server.\n");
    } else {
        printf("Connection socket closed in server.\n");
    }
    return;
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    pthread_t t_id;
    pthread_attr_t t_attr;
    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    int socket_fd, new_socket_fd;
    int val = 1;

    if (argc != 3) {
        printf("Usage: ./server -p <port>\n");
        exit(1);
    }

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket on server.");
        exit(1);
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&val, sizeof(val)) == -1) {
        perror("Error setting socket options on server.");
        exit(1);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    fprintf(stdout, "s> init server %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    fprintf(stdout, "s> \n");

    if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket on server.");
        exit(1);
    } 

    if (listen(socket_fd, SOMAXCONN) == -1) {
        perror("Error listening on server.");
        exit(1);
    } 
    
    size = sizeof(client_addr);
    while (1) {
        pthread_attr_init(&t_attr);
        pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

        if ((new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, (socklen_t *)&size)) == -1) {
            perror("Error accepting connection on server.");
            exit(1);
        } else {
            printf("Connection accepted.\n");
        }

        if (pthread_create(&t_id, &t_attr, (void *)treat_message, (void *)&new_socket_fd) == 0) {
            printf("Thread created for client_%d.\n", getpid());
            pthread_mutex_lock(&mutex_mensaje);
            while (mensaje_no_copiado) {
                pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
            }
            mensaje_no_copiado = 1;
            pthread_mutex_unlock(&mutex_mensaje);
        } else {
            perror("Error creating thread.");
            exit(1);
        }
    }

    close(socket_fd);
    printf("Server socket closed.\n");
    return 0;
}