#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <pthread.h>
#include "servidor.h"

pthread_mutex_t mutex_mensaje = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_mensaje = PTHREAD_COND_INITIALIZER;
int mensaje_no_copiado = 1;

int main() {
    pthread_t t_id;
    pthread_attr_t t_attr;
    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    int socket_fd, new_socket_fd;
    int val = 1;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket on server.");
        exit(1);
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int)) == -1) {
        perror("Error setting socket options on server.");
        exit(1);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

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
        }
        
        if (pthread_create(&t_id, &t_attr, (void *)treat_message, (void *)&new_socket_fd) == 0) {
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
    return 0;
}

int sendMessage(int socket_fd, char *buffer, int size) {
    int bytes_sent;
    int bytes_left = size;

    while ((bytes_sent >=0) && (bytes_left > 0)) {
        bytes_sent = write(socket_fd, buffer, bytes_left);
        bytes_left -= bytes_sent;
        buffer += bytes_sent;
    }

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
}

void treat_message(void *new_socket_fd) {
    char buffer[256];

    pthread_mutex_lock(&mutex_mensaje);
    int socket_fd = *((int *) new_socket_fd);
    mensaje_no_copiado = 0;
    pthread_cond_signal(&cond_mensaje);
    pthread_mutex_unlock(&mutex_mensaje);

    int result;
    struct petition pet;
    struct result res;

    readLine(socket_fd, buffer, 256);
    pet.operation = atoi(buffer);
    switch (pet.operation) {
        case 0:                                     // if operation == 0 call init()
            result = init();
            if (result == 0) {
                res.result = 1;                     // if init ended correctly set result to 1
                sprintf(buffer, "%d", res.result);
                sendMessage(socket_fd, buffer, strlen(buffer)+1);
            }
            break;
        case 1:                                     // if operation == 1 call set_value()
            readLine(socket_fd, buffer, 256);
            pet.key = atoi(buffer);
            readLine(socket_fd, &pet.value1, 256);
            readLine(socket_fd, buffer, 256);
            pet.value2 = atoi(buffer);
            readLine(socket_fd, buffer, 256);
            pet.value3 = atof(buffer);

            result = set_value(pet.key, pet.value1, pet.value2, pet.value3);
            if (result == 0) {
                res.result = 1;                     // if set_value ended correctly set result to 1
                sprintf(buffer, "%d", res.result);
                sendMessage(socket_fd, buffer, strlen(buffer)+1);
            } else {
                res.result = 0;
            }
            break;
        case 2:                                     // if operation == 2 call get_value()
            readLine(socket_fd, buffer, 256);
            pet.key = atoi(buffer);

            result = get_value(pet.key, res.value1, &res.value2, &res.value3);
            if (result == 0) {
                res.result = 1;                     // if get_value ended correctly set result to 1
                sprintf(buffer, "%d", res.result);
                sendMessage(socket_fd, buffer, strlen(buffer)+1);
                sendMessage(socket_fd, res.value1, strlen(res.value1)+1);
                sprintf(buffer, "%d", res.value2);
                sendMessage(socket_fd, buffer, strlen(buffer)+1);
                sprintf(buffer, "%f", res.value3);
                sendMessage(socket_fd, buffer, strlen(buffer)+1);
            } else {
                res.result = 0;
            }
            break;
        case 3:                                     // if operation == 3 call modify_value()
            readLine(socket_fd, buffer, 256);
            pet.key = atoi(buffer);
            readLine(socket_fd, &pet.value1, 256);
            readLine(socket_fd, buffer, 256);
            pet.value2 = atoi(buffer);
            readLine(socket_fd, buffer, 256);
            pet.value3 = atof(buffer);

            result = modify_value(pet.key, pet.value1, pet.value2, pet.value3);
            if (result == 0) {
                res.result = 1;                     // if modify_value ended correctly set result to 1
                sprintf(buffer, "%d", res.result);
                sendMessage(socket_fd, buffer, strlen(buffer)+1);
            } else {
                res.result = 0;
            }
            break;
        case 4:                                     // if operation == 4 call delete_key()
            readLine(socket_fd, buffer, 256);
            pet.key = atoi(buffer);

            result = delete_key(pet.key);
            if (result == 0) {                      // if delete_key ended correctly set result to 1
                res.result = 1;
                sprintf(buffer, "%d", res.result);
                sendMessage(socket_fd, buffer, strlen(buffer)+1);
            } else {
                res.result = 0;
            }
            break;
        case 5:                                     // if operation == 5 call exist()
            readLine(socket_fd, buffer, 256);
            pet.key = atoi(buffer);

            result = exist(pet.key);
            if (result == 1) {                      // if exist ended correctly set result to 1
                res.result = 1;
                sprintf(buffer, "%d", res.result);
                sendMessage(socket_fd, buffer, strlen(buffer)+1);
            } else {
                res.result = 0;
            }
            break;
        case 6:                                     // if operation == 6 call copy_key()
            readLine(socket_fd, buffer, 256);
            pet.key = atoi(buffer);
            readLine(socket_fd, buffer, 256);
            pet.key2 = atoi(buffer);

            result = copy_key(pet.key, pet.key2);
            if (result == 0) {                      // if copy_key ended correctly set result to 1
                res.result = 1;
            } else {
                res.result = 0;
            }
            break;
        default:
            break;
    }

    close(socket_fd);
    pthread_exit(0);
}