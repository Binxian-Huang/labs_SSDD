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

void treat_message(void *new_socket_fd) {
    pthread_mutex_lock(&mutex_mensaje);
    int socket_fd = *((int *) new_socket_fd);
    mensaje_no_copiado = 0;
    pthread_cond_signal(&cond_mensaje);
    pthread_mutex_unlock(&mutex_mensaje);

    int result;
    switch (message.operation) {
        case 0:                                     // if operation == 0 call init()
            result = init();
            if (result == 0) {
                res.result = 1;                     // if init ended correctly set result to 1
            }
            break;
        case 1:                                     // if operation == 1 call set_value()
            result = set_value(message.key, message.value1, message.value2, message.value3);
            if (result == 0) {
                res.result = 1;                     // if set_value ended correctly set result to 1
            } else {
                res.result = 0;
            }
            break;
        case 2:                                     // if operation == 2 call get_value()
            result = get_value(message.key, res.value1, &res.value2, &res.value3);
            if (result == 0) {
                res.result = 1;                     // if get_value ended correctly set result to 1
            } else {
                res.result = 0;
            }
            break;
        case 3:                                     // if operation == 3 call modify_value()
            result = modify_value(message.key, message.value1, message.value2, message.value3);
            if (result == 0) {
                res.result = 1;                     // if modify_value ended correctly set result to 1
            } else {
                res.result = 0;
            }
            break;
        case 4:                                     // if operation == 4 call delete_key()     
            result = delete_key(message.key);
            if (result == 0) {                      // if delete_key ended correctly set result to 1
                res.result = 1;
            } else {
                res.result = 0;
            }
            break;
        case 5:                                     // if operation == 5 call exist()
            result = exist(message.key);
            if (result == 1) {                      // if exist ended correctly set result to 1
                res.result = 1;
            } else {
                res.result = 0;
            }
            break;
        case 6:                                     // if operation == 6 call copy_key()
            result = copy_key(message.key, message.key2);
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