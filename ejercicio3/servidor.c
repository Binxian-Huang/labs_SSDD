#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <rpc/rpc.h>
#include "servidor.h"

pthread_mutex_t mutex_mensaje = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_mensaje = PTHREAD_COND_INITIALIZER;
int mensaje_no_copiado = 1;

int main(int argc, char *argv[]) {
    pthread_t t_id;
    pthread_attr_t t_attr;
    int val = 1;

    if (argc != 1) {
        printf("Usage: ./servidor\n");
        exit(1);
    }

    while (1) {
        pthread_attr_init(&t_attr);
        pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

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
    char buffer[256];
    pthread_mutex_lock(&mutex_mensaje);
    int socket_fd = *((int *) new_socket_fd);
    mensaje_no_copiado = 0;
    pthread_cond_signal(&cond_mensaje);
    pthread_mutex_unlock(&mutex_mensaje);

    int result;
    struct petition pet;
    struct result res;
    if (readLine(socket_fd, buffer, 256) == -1) {
        perror("Error reading operation code from client.\n");
    } else{
        pet.operation = atoi(buffer);
        printf("Operation code received on server: %d\n", pet.operation);
    }

    switch (pet.operation) {
        case 0:                                     // if operation == 0 call init()
            result = init();
            if (result == 0) {
                res.result = 1;                     // if init ended correctly set result to 1
            } else {
                res.result = 0;
            }

            sprintf(buffer, "%d", res.result);
            if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                perror("Error sending init response result in server.\n");
            } else {
                printf("Init response result sent correctly with value: %d\n", res.result);
            }
            break;
        case 1:                                     // if operation == 1 call set_value()
            if (readLine(socket_fd, buffer, 256) == -1) {
                perror("Error reading key of set_value petition in server.\n");
            } else {
                pet.key = atoi(buffer);
                printf("Key of set_value petition received on server with value: %d\n", pet.key);
            }

            if (readLine(socket_fd, pet.value1, 256) == -1) {
                perror("Error reading value1 of set_value petition in server.\n");
            } else {
                printf("Value1 of set_value petition received on server with value: %s\n", pet.value1);
            }

            if (readLine(socket_fd, buffer, 256) == -1) {
                perror("Error reading value2 of set_value petition in server.\n");
            } else {
                pet.value2 = atoi(buffer);
                printf("Value2 of set_value petition received on server with value: %d\n", pet.value2);
            }

            if (readLine(socket_fd, buffer, 256) == -1) {
                perror("Error reading value3 of set_value petition in server.\n");
            } else {
                pet.value3 = atof(buffer);
                printf("Value3 of set_value petition received on server with value: %lf\n", pet.value3);
            }

            printf("Key to set: %d, Value1: %s, Value2: %d, Value3: %lf\n", pet.key, pet.value1, pet.value2, pet.value3);
            result = set_value(pet.key, pet.value1, pet.value2, pet.value3);
            if (result == 0) {
                res.result = 1;                     // if set_value ended correctly set result to 1
            } else {
                res.result = 0;
            }

            sprintf(buffer, "%d", res.result);
            if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                perror("Error sending result code of set_value in server.\n");
            } else {
                printf("Set_value response result sent correctly with value: %d\n", res.result);
            }
            break;
        case 2:                                     // if operation == 2 call get_value()
            if (readLine(socket_fd, buffer, 256) == -1) {
                perror("Error reading key of get_value petition in server.\n");
            } else {
                pet.key = atoi(buffer);
                printf("Key of get_value petition received on server with value: %d\n", pet.key);
            }

            printf("Key to get: %d\n", pet.key);
            result = get_value(pet.key, res.value1, &res.value2, &res.value3);
            if (result == 0) {
                res.result = 1;                     // if get_value ended correctly set result to 1
                sprintf(buffer, "%d", res.result);
                if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                    perror("Error sending result code of get_value in server.\n");
                } else {
                    printf("Get_value response result sent correctly with value: %d\n", res.result);
                }

                if (sendMessage(socket_fd, res.value1, strlen(res.value1)+1) == -1) {
                    perror("Error sending value1 of get_value in server.\n");
                } else {
                    printf("Get_value response value1 sent correctly with value: %s\n", res.value1);
                }

                sprintf(buffer, "%d", res.value2);
                if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                    perror("Error sending value2 of get_value in server.\n");
                } else {
                    printf("Get_value response value2 sent correctly with value: %d\n", res.value2);
                }

                sprintf(buffer, "%f", res.value3);
                if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                    perror("Error sending value3 of get_value in server.\n");
                } else {
                    printf("Get_value response value3 sent correctly with value: %f\n", res.value3);
                }
            } else {
                res.result = 0;
                sprintf(buffer, "%d", res.result);
                if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                    perror("Error sending result code of get_value in server.\n");
                } else {
                    printf("Get_value response result sent correctly with value: %d\n", res.result);
                }
            }
            break;
        case 3:                                     // if operation == 3 call modify_value()
            if (readLine(socket_fd, buffer, 256) == -1) {
                perror("Error reading key of modify_value petition in server.\n");
            } else {
                pet.key = atoi(buffer);
                printf("Key of modify_value petition received on server with value: %d\n", pet.key);
            }
            
            if (readLine(socket_fd, pet.value1, 256) == -1) {
                perror("Error reading value1 of modify_value petition in server.\n");
            } else {
                printf("Value1 of modify_value petition received on server with value: %s\n", pet.value1);
            }

            if (readLine(socket_fd, buffer, 256) == -1) {
                perror("Error reading value2 of modify_value petition in server.\n");
            } else {
                pet.value2 = atoi(buffer);
                printf("Value2 of modify_value petition received on server with value: %d\n", pet.value2);
            }

            if (readLine(socket_fd, buffer, 256) == -1) {
                perror("Error reading value3 of modify_value petition in server.\n");
            } else {
                pet.value3 = atof(buffer);
                printf("Value3 of modify_value petition received on server with value: %lf\n", pet.value3);
            }

            printf("Key to modify: %d, Value1: %s, Value2: %d, Value3: %lf\n", pet.key, pet.value1, pet.value2, pet.value3);
            result = modify_value(pet.key, pet.value1, pet.value2, pet.value3);
            if (result == 0) {
                res.result = 1;                     // if modify_value ended correctly set result to 1
            } else {
                res.result = 0;
            }

            sprintf(buffer, "%d", res.result);
            if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                perror("Error sending result code of init in server.\n");
            } else {
                printf("Modify_value response result sent correctly with value: %d\n", res.result);
            }
            break;
        case 4:                                     // if operation == 4 call delete_key()
            if (readLine(socket_fd, buffer, 256) == -1) {
                perror("Error reading key of delete_key petition in server.\n");
            } else {
                pet.key = atoi(buffer);
                printf("Key of delete_key petition received on server with value: %d\n", pet.key);
            }

            printf("Key to delete: %d\n", pet.key);
            result = delete_key(pet.key);
            if (result == 0) {                      // if delete_key ended correctly set result to 1
                res.result = 1;
            } else {
                res.result = 0;
            }

            sprintf(buffer, "%d", res.result);
            if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                perror("Error sending result code of delete_key in server.\n");
            } else {
                printf("Delete_key response result sent correctly with value: %d\n", res.result);
            }
            break;
        case 5:                                     // if operation == 5 call exist()
            if (readLine(socket_fd, buffer, 256) == -1) {
                perror("Error reading key of exist petition in server.\n");
            } else {
                pet.key = atoi(buffer);
                printf("Key of exist petition received on server with value: %d\n", pet.key);
            }

            printf("Key to verify if exist: %d\n", pet.key);
            result = exist(pet.key);
            if (result == 1) {                      // if exist ended correctly set result to 1
                res.result = 1;
            } else if (result == 0){
                res.result = 0;
            } else {
                res.result = -1;
            }

            sprintf(buffer, "%d", res.result);
            if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                perror("Error sending result code of exist in server.\n");
            } else {
                printf("Exist response result sent correctly with value: %d\n", res.result);
            }
            break;
        case 6:                                     // if operation == 6 call copy_key()
            if (readLine(socket_fd, buffer, 256) == -1) {
                perror("Error reading key1 of copy_key petition in server.\n");
            } else {
                pet.key = atoi(buffer);
                printf("Key1 of copy_key petition received on server with value: %d\n", pet.key);
            }

            if (readLine(socket_fd, buffer, 256) == -1) {
                perror("Error reading key2 of copy_key petition in server.\n");
            } else {
                pet.key2 = atoi(buffer);
                printf("Key2 of copy_key petition received on server with value: %d\n", pet.key2);
            }

            printf("Key to copy for: %d. Key to copy at: %d.\n", pet.key, pet.key2);
            result = copy_key(pet.key, pet.key2);
            if (result == 0) {                      // if copy_key ended correctly set result to 1
                res.result = 1;
            } else {
                res.result = 0;
            }

            sprintf(buffer, "%d", res.result);
            if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                perror("Error sending result code of init in server.\n");
            } else {
                printf("Copy_key response result sent correctly with value: %d\n", res.result);
            }
            break;
        default:
            break;
    }

    if (close(socket_fd) == -1) {
        perror("Error closing connection socket in server.\n");
    } else {
        printf("Connection socket closed in server.\n");
    }
    return;
    pthread_exit(0);
}