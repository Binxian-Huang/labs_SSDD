#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "claves.h"

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

int init() {            // function that sends the message of the init operation to the server
    CLIENT *clnt;
    enum clnt_stat retval;
    int res;
    char *server = "localhost";

    if (clnt = clnt_create(server, SERVICES, SERVICESVER, "tcp") == NULL) {
        clnt_pcreateerror(server);
        perror("Error binding in client init.\n");
        return -1;
    } else {
        printf("Client init binded correctly.\n");
    }

    if (retval = init_1(&res, clnt) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error rpc in client.\n");
        return -1;
    } else {
        printf("Init rpc correctly in client.\n");
    }

    clnt_destroy(clnt);
    if (res == 1) {
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
    } else {
        printf("Operation code %d sent correctly.\n", operation_code);
    }

    sprintf(buffer, "%d", key);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key of set_value in client.\n");
        return -1;
    } else {
        printf("Key %d sent correctly.\n", key);
    }

    if (sendMessage(sc, value1, strlen(value1)+1) == -1) {
        perror("Error sending value1 of set_value in client.\n");
        return -1;
    } else {
        printf("Value1 %s sent correctly.\n", value1);
    }

    sprintf(buffer, "%d", value2);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending value2 of set_value in client.\n");
        return -1;
    } else {
        printf("Value2 %d sent correctly.\n", value2);
    }

    sprintf(buffer, "%f", value3);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending value3 of set_value in client.\n");
        return -1;
    } else {
        printf("Value3 %f sent correctly.\n", value3);
    }

    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading set_value response in client.\n");
        return -1;
    } else {
        res.result = atoi(buffer);
        printf("Set_value response result received correctly with value %d.\n", res.result);
    }

    if (close(sc) == -1) {
        perror("Error closing socket in client set_value.\n");
        return -1;
    } else {
        printf("Conection socket closed of client set_value.\n");
    }

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
    } else {
        printf("Operation code %d sent correctly.\n", operation_code);
    }

    sprintf(buffer, "%d", key);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key of get_value in client.\n");
        return -1;
    } else {
        printf("Key %d sent correctly.\n", key);
    }

    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading result code of get_value response in client.\n");
        return -1;
    } else {
        res.result = atoi(buffer);
        printf("Get_value response result received correctly with value %d.\n", res.result);
    }

    if (res.result == 0) {
        if (close(sc) == -1) {
            perror("Error closing socket in client get_value.\n");
            return -1;
        } else {
            printf("Conection socket closed in client get_value.\n");
        }
        return -1;          // get_value error
    } else {
        if (readLine(sc, value1, 256) == -1) {
            perror("Error reading value1 of get_value response in client.\n");
            return -1;
        } else {
            printf("Value1 received correctly with value %s.\n", value1);
        }

        if (readLine(sc, buffer, 256) == -1) {
            perror("Error reading value2 of get_value response in client.\n");
            return -1;
        } else {
            *value2 = atoi(buffer);
            printf("Value2 received correctly with value %d.\n", *value2);
        }

        if (readLine(sc, buffer, 256) == -1) {
            perror("Error reading value3 of get_value response in client.\n");
            return -1;
        } else {
            *value3 = atof(buffer);
            printf("Value3 received correctly with value %f.\n", *value3);
        }

        if (close(sc) == -1) {
            perror("Error closing socket in client get_value.\n");
            return -1;
        } else {
            printf("Conection socket closed in client get_value.\n");
        }

        return 0;           // get_value ok
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
    } else {
        printf("Operation code %d sent correctly.\n", operation_code);
    }

    sprintf(buffer, "%d", key);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key of modify_value in client.\n");
        return -1;
    } else {
        printf("Key %d sent correctly.\n", key);
    }

    if (sendMessage(sc, value1, strlen(value1)+1) == -1) {
        perror("Error sending value1 of modify_value in client.\n");
        return -1;
    } else {
        printf("Value1 %s sent correctly.\n", value1);
    }

    sprintf(buffer, "%d", value2);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending value2 of modify_value in client.\n");
        return -1;
    } else {
        printf("Value2 %d sent correctly.\n", value2);
    }

    sprintf(buffer, "%f", value3);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending of value3 of modify_value in client.\n");
        return -1;
    } else {
        printf("Value3 %f sent correctly.\n", value3);
    }

    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading modify_value response in client.\n");
        return -1;
    } else {
        res.result = atoi(buffer);
        printf("Modify_value response result received correctly with value %d.\n", res.result);
    }

    if (close(sc) == -1) {
        perror("Error closing socket in client modify_value.\n");
        return -1;
    } else {
        printf("Conection socket closed in client modify_value.\n");
    }

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
    } else {
        printf("Operation code %d sent correctly.\n", operation_code);
    }

    sprintf(buffer, "%d", key);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key of delete_key in client.\n");
        return -1;
    } else {
        printf("Key %d sent correctly.\n", key);
    }

    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading delete_key response in client.\n");
        return -1;
    } else {
        res.result = atoi(buffer);
        printf("Delete_key response result received correctly with value %d.\n", res.result);
    }

    if (close(sc) == -1) {
        perror("Error closing socket in client delete_key.\n");
        return -1;
    } else {
        printf("Conection socket closed in client delete_key.\n");
    }

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
    } else {
        printf("Operation code %d sent correctly.\n", operation_code);
    }

    sprintf(buffer, "%d", key);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key of exist in client.\n");
        return -1;
    } else {
        printf("Key %d sent correctly.\n", key);
    }
    
    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading exist response in client.\n");
        return -1;
    } else {
        res.result = atoi(buffer);
        printf("Exist response result received correctly with value %d.\n", res.result);
    }

    if (close(sc) == -1) {
        perror("Error closing socket in client exist.\n");
        return -1;
    } else {
        printf("Conection socket closed in client exist.\n");
    }
    
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
    } else {
        printf("Operation code %d sent correctly.\n", operation_code);
    }

    sprintf(buffer, "%d", key1);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key1 of copy_key in client.\n");
        return -1;
    } else {
        printf("Key1 %d sent correctly.\n", key1);
    }

    sprintf(buffer, "%d", key2);
    if (sendMessage(sc, buffer, strlen(buffer)+1) == -1) {
        perror("Error sending key2 of copy_key in client.\n");
        return -1;
    } else {
        printf("Key2 %d sent correctly.\n", key2);
    }
    
    if (readLine(sc, buffer, 256) == -1) {
        perror("Error reading copy_key response in client.\n");
        return -1;
    } else {
        res.result = atoi(buffer);
        printf("Copy_key response result received correctly with value %d.\n", res.result);
    }

    if (close(sc) == -1) {
        perror("Error closing socket in client copy_key.\n");
        return -1;
    } else {
        printf("Conection socket closed in client copy_key.\n");
    }

    if (res.result == 1) {
        return 0;           // copy_key ok
    } else {
        return -1;          // copy_key error
    }
}
