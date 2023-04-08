#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <pthread.h>
#include "servidor.h"

pthread_mutex_t mutex_mensaje = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_mensaje = PTHREAD_COND_INITIALIZER;
int mensaje_no_copiado = 1;

int main() {
    mqd_t q_server;
    struct petition pet;
    struct mq_attr attr;
    pthread_t t_id;
    pthread_attr_t t_attr;

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct petition);
    q_server = mq_open("/COLA_SERVIDOR", O_CREAT | O_RDONLY, 0700, &attr);
    if (q_server == -1) {
        perror("Error opening server queue on server.");
        exit(1);
    }

    while (1) {
        if (mq_receive(q_server, (char *) &pet, sizeof(struct petition), 0) == -1) {
            perror("Error receiving petition on server.");
            exit(1);
        }
        
        pthread_attr_init(&t_attr);
        pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
        if (pthread_create(&t_id, &t_attr, (void *)treat_message, (void *) &pet) == 0) {
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
}

void treat_message(void *mess) {
    struct petition message;
    struct result res;
    mqd_t q_client;

    pthread_mutex_lock(&mutex_mensaje);
    message = *((struct petition *) mess);
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

    q_client = mq_open(message.q_name, O_WRONLY);
    if (q_client == -1) {
        perror("Error opening client queue on server.");
        exit(1);
    }

    if (mq_send(q_client, (char *) &res, sizeof(struct result), 0) == -1) {
        perror("Error sending result from server.");
        exit(1);
    }
    mq_close(q_client);

    pthread_exit(0);
}