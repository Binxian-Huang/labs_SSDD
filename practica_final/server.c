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
#include "server.h"

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
    mensaje_no_copiado = 0;
    pthread_cond_signal(&cond_mensaje);
    pthread_mutex_unlock(&mutex_mensaje);

    char buffer[256];
    char operation[256];                            // buffer to store operation code
    int result;
    struct client_data client_data;
    if (readLine(socket_fd, operation, 256) == -1) {
        perror("Error reading operation code from client\n");
    } else{
        printf("Operation received on server: %s\n", operation);
    }

    if (strcmp(operation, "REGISTER") == 0) {
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading username of register in server\n");
        } else {
            strcpy(client_data.username, buffer);
        }
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading alias of register in server\n");
        } else {
            strcpy(client_data.alias, buffer);
        }
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading birthday of register in server\n");
        } else {
            strcpy(client_data.birthday, buffer);
        }
        
        result = register_user(&client_data);
        sprintf(buffer, "%d", result);
        if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
            fprintf(stderr, "Error sending REGISTER result in server\n");
        } else {
            fprintf(stdout, "REGISTER result sent correctly with value: %d\n", result);
        }
    } else if (strcmp(operation, "UNREGISTER") == 0) {
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading alias of unregister in server\n");
        } else {
            strcpy(client_data.alias, buffer);
        }

        result = unregister_user(client_data.alias);
        sprintf(buffer, "%d", result);
        if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
            fprintf(stderr, "Error sending UNREGISTER result in server\n");
        } else {
            fprintf(stdout, "UNREGISTER result sent correctly with value: %d\n", result);
        }
    } else if (strcmp(operation, "CONNECT") == 0) {
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading alias of connect in server\n");
        } else {
            strcpy(client_data.alias, buffer);
        }
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading port of connect in server\n");
        } else {
            client_data.port = atoi(buffer);
        }

        char client_ip[INET_ADDRSTRLEN];
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        if (getpeername(socket_fd, (struct sockaddr *) &client_addr, &client_addr_len) == -1) {
            fprintf(stderr, "Error getting client IP in connect server\n");
        } else {
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
            strcpy(client_data.ip, client_ip);
        }

        result = connect_user(client_data.alias, client_data.ip, client_data.port);
        sprintf(buffer, "%d", result);
        if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
            fprintf(stderr, "Error sending CONNECT result in server\n");
        } else {
            fprintf(stdout, "CONNECT result sent correctly with value: %d\n", result);
        }
    } else if (strcmp(operation, "DISCONNECT") == 0) {
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading alias of disconnect in server\n");
        } else {
            strcpy(client_data.alias, buffer);
        }

        result = disconnect_user(client_data.alias);
        sprintf(buffer, "%d", result);
        if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
            fprintf(stderr, "Error sending DISCONNECT result in server\n");
        } else {
            fprintf(stdout, "DISCONNECT result sent correctly with value: %d\n", result);
        }
    } else if (strcmp(operation, "SEND") == 0) {
        struct message_data message_data;
        char receiver_alias[20];
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading alias of send in server\n");
        } else {
            strcpy(message_data.sender, buffer);
        }
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading receiver of send in server\n");
        } else {
            strcpy(receiver_alias, buffer);
        }
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading message of send in server\n");
        } else {
            strcpy(message_data.message, buffer);
        }

        result = save_message(receiver_alias, &message_data);
        sprintf(buffer, "%d", result);
        if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
            fprintf(stderr, "Error sending SEND result in server\n");
        } else {
            fprintf(stdout, "SEND result sent correctly with value: %d\n", result);
        }
        if (result == 0) {
            sprintf(buffer, "%d", message_data.identifier);
            if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                fprintf(stderr, "Error sending SEND message identifier in server\n");
            } else {
                fprintf(stdout, "SEND message identifier sent correctly with value: %d\n", message_data.identifier);
            }
        }
    } 
   
        /*
        case "UNREGISTER":                                     // if operation == 1 call set_value()
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
        case "CONNECT":                                     // if operation == 2 call get_value()
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
        case "DISCONNECT":                                     // if operation == 3 call modify_value()
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
        case "CONNECTED USERS":                                     // if operation == 4 call delete_key()
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
        case "SEND":                                     // if operation == 5 call exist()
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
        case "SENDATTACH":                                     // if operation == 6 call copy_key()
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
        */
    else {
        fprintf(stderr, "Operation not recognized\n");
    }

    if (close(socket_fd) == -1) {
        fprintf(stderr, "Error closing connection socket in server\n");
    } else {
        fprintf(stdout, "Connection socket closed in server\n");
    }
    pthread_exit(0);
    return;
}

int main(int argc, char *argv[]) {
    pthread_t t_id;
    pthread_attr_t t_attr;
    struct sockaddr_in server_addr, client_addr;
    struct addrinfo hints, *res;
    struct sockaddr_in *ipv4;
    socklen_t size;
    int socket_fd, new_socket_fd;
    int val = 1;

    if (argc != 3) {
        fprintf(stdout, "Usage: ./server -p <port>\n");
        exit(1);
    }

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Error creating socket on server\n");
        exit(1);
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&val, sizeof(val)) == -1) {
        fprintf(stderr, "Error setting socket options on server\n");
        exit(1);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        fprintf(stderr, "Error binding socket on server\n");
        exit(1);
    } 

    if (listen(socket_fd, SOMAXCONN) == -1) {
        fprintf(stderr, "Error listening on server\n");
        exit(1);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    char *host = "localhost";

    if (getaddrinfo(host, NULL, &hints, &res) != 0) {
        fprintf(stderr, "Error getting address info on server\n");
        exit(1);
    }

    for (struct addrinfo *addr = res; addr != NULL; addr = addr->ai_next) {
        ipv4 = (struct sockaddr_in *)addr->ai_addr;
        char ip[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &ipv4->sin_addr, ip, sizeof(ip)) != NULL) {
            fprintf(stdout, "s> init server %s:%d\n", ip, ntohs(server_addr.sin_port));
            fprintf(stdout, "s> \n");
            freeaddrinfo(res);
            break;
        }
    }
    /*
    fprintf(stdout, "s> init server %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    fprintf(stdout, "s> \n");
    */
    size = sizeof(client_addr);
    while (1) {
        pthread_attr_init(&t_attr);
        pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

        if ((new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, (socklen_t *)&size)) == -1) {
            fprintf(stderr, "Error accepting connection on server\n");
            exit(1);
        }

        if (pthread_create(&t_id, &t_attr, (void *)treat_message, (void *)&new_socket_fd) == 0) {
            fprintf(stdout, "Thread created for client_%d\n", getpid());
            pthread_mutex_lock(&mutex_mensaje);
            while (mensaje_no_copiado) {
                pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
            }
            mensaje_no_copiado = 1;
            pthread_mutex_unlock(&mutex_mensaje);
        } else {
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }

    close(socket_fd);
    fprintf(stdout, "Server socket closed\n");
    return 0;
}