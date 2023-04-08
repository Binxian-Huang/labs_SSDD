#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include "claves.h"

mqd_t q_server;
mqd_t q_client;
struct petition pet;
struct result res;
struct mq_attr attr;
char queuename[255];

int init() {            // function that sends the message of the init operation to the server
    pet.operation = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct result);

    sprintf(queuename, "/COLA_CLIENTE_%d", getpid());
    q_client = mq_open(queuename, O_CREAT | O_RDONLY , 0700, &attr);
    if (q_client == -1) {
        perror("Error opening client queue on client.");
        exit(1);
    }
    q_server = mq_open("/COLA_SERVIDOR", O_WRONLY);
    if (q_server == -1) {
        perror("Error opening server queue on client .");
        exit(1);
    }

    strcpy(pet.q_name, queuename);
    if (mq_send(q_server, (char *) &pet, sizeof(struct petition), 0) == -1) {
        perror("Error sending petition from client.");
        exit(1);
    }
    if (mq_receive(q_client, (char *) &res, sizeof(struct result), 0) == -1) {
        perror("Error receiving result on client.");
        exit(1);
    }

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(queuename);
    return 0;
}

int set_value(int key, char *value1, int value2, double value3) {           // function that sends the message of the set_value operation to the server
    pet.operation = 1;                                                      
    pet.key = key;                                                          // set values of the tuple to save in the struct pet
    strcpy(pet.value1, value1);                                             
    pet.value2 = value2;                                                    
    pet.value3 = value3;                                                   
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct result);

    sprintf(queuename, "/COLA_CLIENTE_%d", getpid());
    q_client = mq_open(queuename, O_CREAT | O_RDONLY , 0700, &attr);
    if (q_client == -1) {
        perror("Error opening client queue on client.");
        exit(1);
    }
    q_server = mq_open("/COLA_SERVIDOR", O_WRONLY);
    if (q_server == -1) {
        perror("Error opening server queue on client .");
        exit(1);
    }

    strcpy(pet.q_name, queuename);
    if (mq_send(q_server, (char *) &pet, sizeof(struct petition), 0) == -1) {
        perror("Error sending petition from client.");
        exit(1);
    }
    if (mq_receive(q_client, (char *) &res, sizeof(struct result), 0) == -1) {
        perror("Error receiving result on client.");
        exit(1);
    }

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(queuename);
    if (res.result == 1) {
        return 1;
    } else {
        return 0;
    }
}

int get_value(int key, char *value1, int *value2, double *value3) {         // function that sends the message of the get_value operation to the server
    pet.operation = 2;
    pet.key = key;                                                          // set values of the tuple to save in the struct pet
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct result);

    sprintf(queuename, "/COLA_CLIENTE_%d", getpid());
    q_client = mq_open(queuename, O_CREAT | O_RDONLY , 0700, &attr);
    if (q_client == -1) {
        perror("Error opening client queue on client.");
        exit(1);
    }
    q_server = mq_open("/COLA_SERVIDOR", O_WRONLY);
    if (q_server == -1) {
        perror("Error opening server queue on client .");
        exit(1);
    }

    strcpy(pet.q_name, queuename);
    if (mq_send(q_server, (char *) &pet, sizeof(struct petition), 0) == -1) {
        perror("Error sending petition from client.");
        exit(1);
    }
    if (mq_receive(q_client, (char *) &res, sizeof(struct result), 0) == -1) {
        perror("Error receiving result on client.");
        exit(1);
    }
    
    strcpy(value1, res.value1);                                            // set values of the tuple to save in the struct res
    *value2 = res.value2;
    *value3 = res.value3;
    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(queuename);
    if (res.result == 1) {
        return 1;
    } else {
        return 0;
    }
}

int modify_value(int key, char *value1, int value2, double value3) {        // function that sends the message of the modify_value operation to the server
    pet.operation = 3;
    pet.key = key;
    strcpy(pet.value1, value1);
    pet.value2 = value2;
    pet.value3 = value3;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct result);

    sprintf(queuename, "/COLA_CLIENTE_%d", getpid());
    q_client = mq_open(queuename, O_CREAT | O_RDONLY , 0700, &attr);
    if (q_client == -1) {
        perror("Error opening client queue on client.");
        exit(1);
    }
    q_server = mq_open("/COLA_SERVIDOR", O_WRONLY);
    if (q_server == -1) {
        perror("Error opening server queue on client .");
        exit(1);
    }

    strcpy(pet.q_name, queuename);
    if (mq_send(q_server, (char *) &pet, sizeof(struct petition), 0) == -1) {
        perror("Error sending petition from client.");
        exit(1);
    }
    if (mq_receive(q_client, (char *) &res, sizeof(struct result), 0) == -1) {
        perror("Error receiving result on client.");
        exit(1);
    }

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(queuename);
    if (res.result == 1) {
        return 1;
    } else {
        return 0;
    }
}

int delete_key(int key) {                                        // function that sends the message of the delete_key operation to the server
    pet.operation = 4;
    pet.key = key;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct result);

    sprintf(queuename, "/COLA_CLIENTE_%d", getpid());
    q_client = mq_open(queuename, O_CREAT | O_RDONLY , 0700, &attr);
    if (q_client == -1) {
        perror("Error opening client queue on client.");
        exit(1);
    }
    q_server = mq_open("/COLA_SERVIDOR", O_WRONLY);
    if (q_server == -1) {
        perror("Error opening server queue on client .");
        exit(1);
    }

    strcpy(pet.q_name, queuename);
    if (mq_send(q_server, (char *) &pet, sizeof(struct petition), 0) == -1) {
        perror("Error sending petition from client.");
        exit(1);
    }
    if (mq_receive(q_client, (char *) &res, sizeof(struct result), 0) == -1) {
        perror("Error receiving result on client.");
        exit(1);
    }

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(queuename);
    if (res.result == 1) {
        return 1;
    } else {
        return 0;
    }
}

int exist(int key) {                                // function that sends the message of the exist operation to the server
    pet.operation = 5;
    pet.key = key;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct result);

    sprintf(queuename, "/COLA_CLIENTE_%d", getpid());
    q_client = mq_open(queuename, O_CREAT | O_RDONLY , 0700, &attr);
    if (q_client == -1) {
        perror("Error opening client queue on client.");
        exit(1);
    }
    q_server = mq_open("/COLA_SERVIDOR", O_WRONLY);
    if (q_server == -1) {
        perror("Error opening server queue on client .");
        exit(1);
    }

    strcpy(pet.q_name, queuename);
    if (mq_send(q_server, (char *) &pet, sizeof(struct petition), 0) == -1) {
        perror("Error sending petition from client.");
        exit(1);
    }
    if (mq_receive(q_client, (char *) &res, sizeof(struct result), 0) == -1) {
        perror("Error receiving result on client.");
        exit(1);
    }

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(queuename);
    if (res.result == 1) {
        return 1;
    } else {
        return 0;
    }
}

int copy_key(int key1, int key2) {                      // function that sends the message of the copy_key operation to the server
    pet.operation = 6;
    pet.key = key1;
    pet.key2 = key2;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct result);

    sprintf(queuename, "/COLA_CLIENTE_%d", getpid());
    q_client = mq_open(queuename, O_CREAT | O_RDONLY , 0700, &attr);
    if (q_client == -1) {
        perror("Error opening client queue on client.");
        exit(1);
    }
    q_server = mq_open("/COLA_SERVIDOR", O_WRONLY);
    if (q_server == -1) {
        perror("Error opening server queue on client .");
        exit(1);
    }

    strcpy(pet.q_name, queuename);
    if (mq_send(q_server, (char *) &pet, sizeof(struct petition), 0) == -1) {
        perror("Error sending petition from client.");
        exit(1);
    }
    if (mq_receive(q_client, (char *) &res, sizeof(struct result), 0) == -1) {
        perror("Error receiving result on client.");
        exit(1);
    }

    mq_close(q_server);
    mq_close(q_client);
    mq_unlink(queuename);
    if (res.result == 1) {
        return 1;
    } else {
        return 0;
    }
}

