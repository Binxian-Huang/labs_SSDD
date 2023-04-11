#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include "claves.h"


struct petition pet;
struct result res;
int socket_fd;

int enable_connection() {
    struct sockaddr_in server_addr;
    short server_port = 8080;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_aton("127.0.0.1", &server_addr.sin_addr) == 0) {
        perror("Error in inet_aton on client.\n");
        return -1;
    }
    
    if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Error in connect on client.\n");
        return -1;
    }

    return 0;
}

int disable_connection() {
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
    *buf = '\0';
    return totRead;
}

int init() {            // function that sends the message of the init operation to the server
    char buffer[256];
    int operation_code = 0;
    sprintf(buffer, "%d", operation_code);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    printf("init message sent\n");

    readLine(socket_fd, buffer, 256);
    res.result = atoi(buffer);
    printf("init message received\n");
    return 0;
}

int set_value(int key, char *value1, int value2, double value3) {           // function that sends the message of the set_value operation to the server
    char buffer[256];
    int operation_code = 1;    
    sprintf(buffer, "%d", operation_code);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    sprintf(buffer, "%d", key);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    sendMessage(socket_fd, value1, strlen(value1)+1);
    sprintf(buffer, "%d", value2);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    sprintf(buffer, "%f", value3);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);                                                  

    readLine(socket_fd, buffer, 256);
    res.result = atoi(buffer);
    if (res.result == 1) {
        return 1;
    } else {
        return 0;
    }
}

int get_value(int key, char *value1, int *value2, double *value3) {         // function that sends the message of the get_value operation to the server
    char buffer[256];
    int operation_code = 2;    
    sprintf(buffer, "%d", operation_code);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    sprintf(buffer, "%d", key);                                                       

    readLine(socket_fd, buffer, 256);
    res.result = atoi(buffer);
    readLine(socket_fd, value1, 256);
    readLine(socket_fd, buffer, 256);
    *value2 = atoi(buffer);
    readLine(socket_fd, buffer, 256);
    *value3 = atof(buffer);
    if (res.result == 1) {
        return 1;
    } else {
        return 0;
    }
}

int modify_value(int key, char *value1, int value2, double value3) {        // function that sends the message of the modify_value operation to the server
    char buffer[256];
    int operation_code = 3;    
    sprintf(buffer, "%d", operation_code);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    sprintf(buffer, "%d", key);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    sendMessage(socket_fd, value1, strlen(value1)+1);
    sprintf(buffer, "%d", value2);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    sprintf(buffer, "%f", value3);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);                                                  

    readLine(socket_fd, buffer, 256);
    res.result = atoi(buffer);
    if (res.result == 1) {
        return 1;
    } else {
        return 0;
    }
}

int delete_key(int key) {                                        // function that sends the message of the delete_key operation to the server
    char buffer[256];
    int operation_code = 4;    
    sprintf(buffer, "%d", operation_code);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    sprintf(buffer, "%d", key);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);                                             

    readLine(socket_fd, buffer, 256);
    res.result = atoi(buffer);
    if (res.result == 1) {
        return 1;
    } else {
        return 0;
    }
}

int exist(int key) {                                // function that sends the message of the exist operation to the server
    char buffer[256];
    int operation_code = 5;    
    sprintf(buffer, "%d", operation_code);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    sprintf(buffer, "%d", key);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    
    readLine(socket_fd, buffer, 256);
    res.result = atoi(buffer);
    if (res.result == 1) {
        return 1;
    } else {
        return 0;
    }
}

int copy_key(int key1, int key2) {                      // function that sends the message of the copy_key operation to the server
    char buffer[256];
    int operation_code = 6;    
    sprintf(buffer, "%d", operation_code);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    sprintf(buffer, "%d", key1);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    sprintf(buffer, "%d", key2);
    sendMessage(socket_fd, buffer, strlen(buffer)+1);
    
    readLine(socket_fd, buffer, 256);
    res.result = atoi(buffer);
    if (res.result == 1) {
        return 1;
    } else {
        return 0;
    }
}
