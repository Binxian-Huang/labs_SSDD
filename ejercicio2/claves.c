#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include "claves.h"

int enable_connection() {
    int socket_fd;
    struct sockaddr_in server_addr;
    short server_port = 8081;
    struct hostent *server;
    char *server_name = "localhost";

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &server_addr, sizeof(server_addr));
    server = gethostbyname(server_name);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    memcpy(&(server_addr.sin_addr), server->h_addr, server->h_length);
    
    if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Error in connect on client.\n");
        return -1;
    } else {
        printf("Connection established.\n");
        return 0;
    }

    return socket_fd;
}

int sendMessage(int socket_fd, char *buffer, int size) {
    int bytes_sent;
    int bytes_left = size;

    printf("Valor a enviar: %s.\n", buffer);
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
    printf("Value: %s\n", buffer);
    return totRead;
}

int init() {            // function that sends the message of the init operation to the server
    struct result res;
    char buffer[256];
    int operation_code = 0;
    int sc = enable_connection();
    
    sprintf(buffer, "%d", operation_code);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending operation code of init in client.\n");
        return -1;
    }

    printf("init message sent\n");

    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading init response in client.\n");
        return -1;
    }
    res.result = atoi(buffer);
    close(sc);
    printf("Conection socket closed.\n");
    printf("init response received\n");
    
    if (res.result == 1) {
        return 0;           // init ok
    } else {
        return -1;           // init error
    }
}

int set_value(int key, char *value1, int value2, double value3) {           // function that sends the message of the set_value operation to the server
    struct result res;
    char buffer[256];
    int operation_code = 1;
    int sc = enable_connection();

    sprintf(buffer, "%d", operation_code);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending operation code of set_value in client.\n");
        return -1;
    }
    sprintf(buffer, "%d", key);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key of set_value in client.\n");
        return -1;
    }
    if (sendMessage(sc, value1, strlen(buffer)+1) == -1) {
        perror("Error sending value1 of set_value in client.\n");
        return -1;
    }
    sprintf(buffer, "%d", value2);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending value2 of set_value in client.\n");
        return -1;
    }
    sprintf(buffer, "%f", value3);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending value3 of set_value in client.\n");
        return -1;
    } 
    printf("set_value message sent\n");   
    fflush(stdout);                                           

    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading set_value response in client.\n");
        return -1;
    }
    res.result = atoi(buffer);
    close(sc);
    printf("set_value response received\n");

    if (res.result == 1) {
        return 0;           // set_value ok
    } else {
        return -1;          // set_value error
    }
}

int get_value(int key, char *value1, int *value2, double *value3) {         // function that sends the message of the get_value operation to the server
    struct result res;
    char buffer[256];
    int operation_code = 2;
    int sc = enable_connection();

    sprintf(buffer, "%d", operation_code);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending operation code of get_value in client.\n");
        return -1;
    }
    sprintf(buffer, "%d", key);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key of get_value in client.\n");
        return -1;
    }
    printf("get_value message sent\n");

    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading result code of get_value response in client.\n");
        return -1;
    }
    res.result = atoi(buffer);
    if (readLine(sc, value1, 256) == -1) {
        perror("Error reading value1 of get_value response in client.\n");
        return -1;
    }
    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading value2 of get_value response in client.\n");
        return -1;
    }
    *value2 = atoi(buffer);
    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading value3 of get_value response in client.\n");
        return -1;
    }
    *value3 = atof(buffer);
    close(sc);
    printf("get_value response received\n");

    if (res.result == 1) {
        return 0;           // get_value ok
    } else {
        return -1;          // get_value error
    }
}

int modify_value(int key, char *value1, int value2, double value3) {        // function that sends the message of the modify_value operation to the server
    struct result res;
    char buffer[256];
    int operation_code = 3;
    int sc = enable_connection();

    sprintf(buffer, "%d", operation_code);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending operation code of modify_value in client.\n");
        return -1;
    }
    sprintf(buffer, "%d", key);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key of modify_value in client.\n");
        return -1;
    }
    if (sendMessage(sc, value1, strlen(buffer)+1) == -1) {
        perror("Error sending value1 of modify_value in client.\n");
        return -1;
    }
    sprintf(buffer, "%d", value2);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending value2 of modify_value in client.\n");
        return -1;
    }
    sprintf(buffer, "%f", value3);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending of value3 of modify_value in client.\n");
        return -1;
    }
    printf("modify_value message sent\n");

    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading modify_value response in client.\n");
        return -1;
    }
    res.result = atoi(buffer);
    close(sc);
    printf("modify_value response received\n");

    if (res.result == 1) {
        return 0;           // modify_value ok
    } else {
        return -1;          // modify_value error
    }
}

int delete_key(int key) {                                        // function that sends the message of the delete_key operation to the server
    struct result res;
    char buffer[256];
    int operation_code = 4;
    int sc = enable_connection();

    sprintf(buffer, "%d", operation_code);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending operation code of delete_key in client.\n");
        return -1;
    }
    sprintf(buffer, "%d", key);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key of delete_key in client.\n");
        return -1;
    }
    printf("delete_key message sent\n");

    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading delete_key response in client.\n");
        return -1;
    }
    res.result = atoi(buffer);
    printf("delete_key response received\n");
    if (res.result == 1) {
        return 0;           // delete_key ok
    } else {
        return -1;          // delete_key error
    }
}

int exist(int key) {                                // function that sends the message of the exist operation to the server
    struct result res;
    char buffer[256];
    int operation_code = 5;
    int sc = enable_connection();

    sprintf(buffer, "%d", operation_code);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending operation code of exist in client.\n");
        return -1;
    }
    sprintf(buffer, "%d", key);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key of exist in client.\n");
        return -1;
    }
    printf("exist message sent\n");
    
    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading exist response in client.\n");
        return -1;
    }
    res.result = atoi(buffer);
    close(sc);
    printf("exist response received\n");
    
    if (res.result == 1) {
        return 1;           // exist
    } else if (res.result == 0) {
        return 0;           // not exist
    } else {
        return -1;          // exist error
    }
}

int copy_key(int key1, int key2) {                      // function that sends the message of the copy_key operation to the server
    struct result res;
    char buffer[256];
    int operation_code = 6;
    int sc = enable_connection();

    sprintf(buffer, "%d", operation_code);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending operation code of copy_key in client.\n");
        return -1;
    }
    sprintf(buffer, "%d", key1);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key1 of copy_key in client.\n");
        return -1;
    }
    sprintf(buffer, "%d", key2);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key2 of copy_key in client.\n");
        return -1;
    }
    printf("copy_key message sent\n");
    
    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading copy_key response in client.\n");
        return -1;
    }
    res.result = atoi(buffer);
    close(sc);
    printf("copy_key response received\n");

    if (res.result == 1) {
        return 0;           // copy_key ok
    } else {
        return -1;          // copy_key error
    }
}
