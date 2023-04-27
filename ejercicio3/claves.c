#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "claves.h"
#include "servicios.h"

int init() {            // function that sends the message of the init operation to the server
    CLIENT *clnt;
    enum clnt_stat retval;
    int res;
    char *server = "localhost";

    if ((clnt = clnt_create(server, SERVICES, SERVICESVER, "tcp")) == NULL) {
        clnt_pcreateerror(server);
        fprintf(stderr, "Error binding in client init\n");
        return -1;
    } else {
        fprintf(stdout, "Client init binded correctly\n");
    }

    if ((retval = init_1(&res, clnt)) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error rpc in client init\n");
        return -1;
    } else {
        fprintf(stdout, "Init rpc correct in client\n");
    }

    clnt_destroy(clnt);
    if (res == 0) {
        return 0;           // init ok
    } else {
        return -1;           // init error
    } 
}

int set_value(int key, char *value1, int value2, double value3) {           // function that sends the message of the set_value operation to the server
    CLIENT *clnt;
    enum clnt_stat retval;
    int res;
    char *server = "localhost";

    if ((clnt = clnt_create(server, SERVICES, SERVICESVER, "tcp")) == NULL) {
        clnt_pcreateerror(server);
        fprintf(stderr, "Error binding in client set_value\n");
        return -1;
    } else {
        fprintf(stdout, "Client set_value binded correctly\n");
    }

    if ((retval = set_1(key, value1, value2, value3, &res, clnt)) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error rpc in client set_value\n");
        return -1;
    } else {
        fprintf(stdout, "Set_value rpc correct in client\n");
    }

    clnt_destroy(clnt);
    if (res == 0) {
        return 0;           // set_value ok
    } else {
        return -1;          // set_value error
    }
}

int get_value(int key, char *value1, int *value2, double *value3) {         // function that sends the message of the get_value operation to the server
    CLIENT *clnt;
    enum clnt_stat retval;
    struct result_values res;
    char *server = "localhost";

    if ((clnt = clnt_create(server, SERVICES, SERVICESVER, "tcp")) == NULL) {
        clnt_pcreateerror(server);
        fprintf(stderr, "Error binding in client get_value\n");
        return -1;
    } else {
        fprintf(stdout, "Client get_value binded correctly\n");
    }

    if ((retval = get_1(key, &res, clnt)) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error rpc in client get_value\n");
        return -1;
    } else {
        fprintf(stdout, "Get_value rpc correct in client\n");
        strcpy(value1, res.value1);
        *value2 = res.value2;
        *value3 = res.value3;
    }

    clnt_destroy(clnt);
    if (res.operation_result == 0) {
        return 0;          // get_value ok
    } else {
        return -1;           // get_value error
    }
}

int modify_value(int key, char *value1, int value2, double value3) {        // function that sends the message of the modify_value operation to the server
    CLIENT *clnt;
    enum clnt_stat retval;
    int res;
    char *server = "localhost";

    if ((clnt = clnt_create(server, SERVICES, SERVICESVER, "tcp")) == NULL) {
        clnt_pcreateerror(server);
        fprintf(stderr, "Error binding in client modify_value\n");
        return -1;
    } else {
        fprintf(stdout, "Client modify_value binded correctly\n");
    }

    if ((retval = modify_1(key, value1, value2, value3, &res, clnt)) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error rpc in client modify_value\n");
        return -1;
    } else {
        fprintf(stdout, "Modify_value rpc correct in client\n");
    }

    clnt_destroy(clnt);
    if (res == 0) {
        return 0;           // modify_value ok
    } else {
        return -1;          // modify_value error
    }
}

int delete_key(int key) {                                        // function that sends the message of the delete_key operation to the server
    CLIENT *clnt;
    enum clnt_stat retval;
    int res;
    char *server = "localhost";

    if ((clnt = clnt_create(server, SERVICES, SERVICESVER, "tcp")) == NULL) {
        clnt_pcreateerror(server);
        fprintf(stderr, "Error binding in client delete_key\n");
        return -1;
    } else {
        fprintf(stdout, "Client delete_key binded correctly\n");
    }

    if ((retval = delete_1(key, &res, clnt)) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error rpc in client delete_key\n");
        return -1;
    } else {
        fprintf(stdout, "Delete_key rpc correct in client\n");
    }

    clnt_destroy(clnt);
    if (res == 0) {
        return 0;           // delete_key ok
    } else {
        return -1;          // delete_key error
    }
}

int exist(int key) {                                // function that sends the message of the exist operation to the server
    CLIENT *clnt;
    enum clnt_stat retval;
    int res;
    char *server = "localhost";

    if ((clnt = clnt_create(server, SERVICES, SERVICESVER, "tcp")) == NULL) {
        clnt_pcreateerror(server);
        fprintf(stderr, "Error binding in client exist\n");
        return -1;
    } else {
        fprintf(stdout, "Client exist binded correctly\n");
    }

    if ((retval = exist_1(key, &res, clnt)) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error rpc in client exist\n");
        return -1;
    } else {
        fprintf(stdout, "Exist rpc correct in client\n");
    }

    clnt_destroy(clnt);
    if (res == 1) {
        return 1;           // exist
    } else if (res == 0) {
        return 0;           // not exist
    } else {
        return -1;          // exist error
    }
}

int copy_key(int key1, int key2) {                      // function that sends the message of the copy_key operation to the server
    CLIENT *clnt;
    enum clnt_stat retval;
    int res;
    char *server = "localhost";

    if ((clnt = clnt_create(server, SERVICES, SERVICESVER, "tcp")) == NULL) {
        clnt_pcreateerror(server);
        fprintf(stderr, "Error binding in client copy_key\n");
        return -1;
    } else {
        fprintf(stdout, "Client copy_key binded correctly\n");
    }

    if ((retval = copy_1(key1, key2, &res, clnt)) != RPC_SUCCESS) {
        clnt_perror(clnt, "Error rpc in client copy_key\n");
        return -1;
    } else {
        fprintf(stdout, "Copy_key rpc correct in client\n");
    }

    clnt_destroy(clnt);
    if (res == 0) {
        return 0;           // copy_key ok
    } else {
        return -1;          // copy_key error
    }
}
