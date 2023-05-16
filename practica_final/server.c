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

/*Function that read a line form socket*/
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

/*Function that treat a message, called from thread*/
void treat_message(void *new_socket_fd) {
    pthread_mutex_lock(&mutex_mensaje);
    int socket_fd = *((int *) new_socket_fd);                                       
    mensaje_no_copiado = 0;
    pthread_cond_signal(&cond_mensaje);
    pthread_mutex_unlock(&mutex_mensaje);

    char buffer[256];                                                               // buffer for storing messages to send from socket                        
    char operation[256];                                                            // buffer to store operation message
    int result;
    struct client_data client_data;                                                 // struct to store client data       

    // read operation message from socket
    if (readLine(socket_fd, operation, 256) == -1) {
        perror("Error reading operation code from client\n");
    } else{
        printf("Operation received on server: %s\n", operation);
    }
    
    //depending of operation, read data from socket and call function
    if (strcmp(operation, "REGISTER") == 0) {
        //read username to register and store in struct
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading username of register in server\n");
        } else {
            strcpy(client_data.username, buffer);
        }
        //read client alias to register and store in struct
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading alias of register in server\n");
        } else {
            strcpy(client_data.alias, buffer);
        }
        //read client birthday to register and store in struct
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading birthday of register in server\n");
        } else {
            strcpy(client_data.birthday, buffer);
        }
        //call function to register client, returned 0 if OK, 1 if already registered, 2 if error
        result = register_user(&client_data);
        sprintf(buffer, "%d", result);
        if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
            fprintf(stderr, "Error sending REGISTER result in server\n");
        }
    } else if (strcmp(operation, "UNREGISTER") == 0) {
        //read client alias to unregister
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading alias of unregister in server\n");
        } else {
            strcpy(client_data.alias, buffer);
        }
        //call function to unregister client, returned 0 if OK, 1 if not registered, 2 if error
        result = unregister_user(client_data.alias);
        sprintf(buffer, "%d", result);
        if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
            fprintf(stderr, "Error sending UNREGISTER result in server\n");
        }
    } else if (strcmp(operation, "CONNECT") == 0) {
        //read client alias to connect
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading alias of connect in server\n");
        } else {
            strcpy(client_data.alias, buffer);
        }
        //read client listen socket port to save
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading port of connect in server\n");
        } else {
            client_data.port = atoi(buffer);
        }
        //get client ip through socket
        char client_ip[INET_ADDRSTRLEN];
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        if (getpeername(socket_fd, (struct sockaddr *) &client_addr, &client_addr_len) == -1) {
            fprintf(stderr, "Error getting client IP in connect server\n");
        } else {
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
            strcpy(client_data.ip, client_ip);
        }
        //call function to connect client, returned 0 if OK, 1 if not registered, 2 if already online, 3 if error
        result = connect_user(client_data.alias, client_data.ip, client_data.port);
        sprintf(buffer, "%d", result);
        if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
            fprintf(stderr, "Error sending CONNECT result in server\n");
        }
        
        if (result == 0) {
            //Every time a client connects, send all messages to him (the function will check if he has messages pending)
            int sending = send_message(client_data.alias);
            if (sending == -1) {
                fprintf(stderr, "Error sending message in server\n");
            } else if (sending == 0) {
                fprintf(stdout, "No messages to send to client in server\n");
            } else {
                fprintf(stdout, "All messages sent to client\n");
            }
        }
    } else if (strcmp(operation, "DISCONNECT") == 0) {
        //read client alias to disconnect
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading alias of disconnect in server\n");
        } else {
            strcpy(client_data.alias, buffer);
        }
        //call function to disconnect client, returned 0 if OK, 1 if not registered, 2 if not online, 3 if error
        result = disconnect_user(client_data.alias);
        sprintf(buffer, "%d", result);
        if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
            fprintf(stderr, "Error sending DISCONNECT result in server\n");
        }
    } else if (strcmp(operation, "SEND") == 0) {
        struct message_data message_data;
        char receiver_alias[20];
        //read sender alias
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading alias of send in server\n");
        } else {
            strcpy(message_data.sender, buffer);
        }
        //read receiver alias
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading receiver of send in server\n");
        } else {
            strcpy(receiver_alias, buffer);
        }
        //read message
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading message of send in server\n");
        } else {
            strcpy(message_data.message, buffer);
        }
        //calls function to save message, returned 0 if OK, 1 if not registered, 2 if not online, 3 if error
        result = save_message(receiver_alias, &message_data);
        sprintf(buffer, "%d", result);
        if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
            fprintf(stderr, "Error sending SEND result in server\n");
        }
        //if message was saved, send message identifier to client
        if (result == 0) {
            sprintf(buffer, "%d", message_data.identifier);
            if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                fprintf(stderr, "Error sending SEND message identifier in server\n");
            }

            //Every time saved a message, try to send it to the receiver (the function will check if he is online)
            int sending = send_message(receiver_alias);
            if (sending == -1) {
                fprintf(stderr, "Error sending message in server\n");
            } else if (sending == 0) {
                fprintf(stdout, "No messages to send to client in server\n");
            } else {
                fprintf(stdout, "All messages sent to client\n");
            }
        }
    } else if (strcmp(operation, "CONNECTEDUSERS") == 0) {
        char alias[20];
        char user_names[20][20];
        int number_connected_users = 0;
        //read client alias
        if (readLine(socket_fd, buffer, 256) == -1) {
            fprintf(stderr, "Error reading alias of send in server\n");
        } else {
            strcpy(alias, buffer);
        }
        //calls function to get connected users, returned 0 if OK, 1 if current client not online, 2 if current client not registered or other case
        result = connected_users(alias, user_names, &number_connected_users);
        sprintf(buffer, "%d", result);
        if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
            fprintf(stderr, "Error sending SEND result in server\n");
        }
        //if result is 0, send number of connected users and their alias
        if (result == 0) {
            sprintf(buffer, "%d", number_connected_users);
            if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                fprintf(stderr, "Error sending number of connected users in server\n");
            }
            for (int i = 0; i < number_connected_users; i++) {
                sprintf(buffer, "%s", user_names[i]);
                if (sendMessage(socket_fd, buffer, strlen(buffer)+1) == -1) {
                    fprintf(stderr, "Error sending connected user alias in server\n");
                }
            }
        }
    } else {
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

    char hostname[256];
    char *ip;
    struct hostent *host_entry;

    if (gethostname(hostname, sizeof(hostname)) == -1) {
        fprintf(stderr, "Error getting hostname on server\n");
        exit(1);
    }

    host_entry = gethostbyname(hostname);
    if (host_entry == NULL) {
        fprintf(stderr, "Error getting host information on server\n");
        exit(1);
    }

    ip = inet_ntoa(*(struct in_addr*) host_entry->h_addr);
    fprintf(stdout, "s> init server %s:%d\n", ip, ntohs(server_addr.sin_port));
    fprintf(stdout, "s> \n");
    
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