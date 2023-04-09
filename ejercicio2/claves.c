#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "claves.h"


struct petition pet;
struct result res;
int socket_fd;


int enable_connection() {
    struct sockaddr_in server_addr;
    short server_port = 8080;
    struct hostent *server;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.port = htons(server_port);
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

int init() {            // function that sends the message of the init operation to the server

}

int set_value(int key, char *value1, int value2, double value3) {           // function that sends the message of the set_value operation to the server
    pet.operation = 1;                                                      
    pet.key = key;                                                          // set values of the tuple to save in the struct pet
    strcpy(pet.value1, value1);                                             
    pet.value2 = value2;                                                    
    pet.value3 = value3;                                                   

}

int get_value(int key, char *value1, int *value2, double *value3) {         // function that sends the message of the get_value operation to the server
    pet.operation = 2;
    pet.key = key;                                                          // set values of the tuple to save in the struct pet



    strcpy(value1, res.value1);                                            // set values of the tuple to save in the struct res
    *value2 = res.value2;
    *value3 = res.value3;
    
}

int modify_value(int key, char *value1, int value2, double value3) {        // function that sends the message of the modify_value operation to the server
    pet.operation = 3;
    pet.key = key;
    strcpy(pet.value1, value1);
    pet.value2 = value2;
    pet.value3 = value3;

}

int delete_key(int key) {                                        // function that sends the message of the delete_key operation to the server
    pet.operation = 4;
    pet.key = key;

}

int exist(int key) {                                // function that sends the message of the exist operation to the server
    pet.operation = 5;
    pet.key = key;

}

int copy_key(int key1, int key2) {                      // function that sends the message of the copy_key operation to the server
    pet.operation = 6;
    pet.key = key1;
    pet.key2 = key2;

}

