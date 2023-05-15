#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "server.h"

int client_existing (char *userdir_path) {
    DIR *dir;

    if ((dir = opendir(userdir_path)) == NULL) {
        return 0;                                               // If directory doesn't exist, client not registered
    }

    closedir(dir);
    return 1;                                                   // If directory exists, client registered
};

int client_connected (char *alias) {
    char *userdir_path = malloc(strlen(alias) + 1);
    strcpy(userdir_path, alias);

    char userdata_path[] = "/user_data.txt";
    char *datafilename = malloc(strlen(userdir_path) + strlen(userdata_path) + 1);
    strcpy(datafilename, userdir_path);
    strcat(datafilename, userdata_path);

    if (access(datafilename, F_OK) != -1) {
        FILE *file = fopen(datafilename, "r");
        char line[256];
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strstr(line, "Online: ") != NULL) {
                int current_online = atoi(line + strlen("Online: "));
                if (current_online == 1) {
                    fclose(file);
                    free(datafilename);
                    free(userdir_path);
                    return 1;
                } else if (current_online == 0) {
                    fclose(file);
                    free(datafilename);
                    free(userdir_path);
                    return 0;
                } else {
                    fprintf(stderr, "Error reading online status\n");
                    fclose(file);
                    free(datafilename);
                    free(userdir_path);
                    return -1;
                }
            }
        }
    }
    fprintf(stderr, "Error opening user_data file on connect_user\n");
    free(datafilename);
    free(userdir_path);
    return -1;
};

int register_user (struct client_data *client) {
    char *userdir_path = malloc(strlen(client->alias) + 1);
    strcpy(userdir_path, client->alias);

    if (client_existing(userdir_path) == 1) {
        fprintf(stdout, "Client already registered\n");
        free(userdir_path);
        return 1;
    }

    if (mkdir(userdir_path, 0777) != -1) {
        DIR *dir;
        FILE *file;
        char userdata_path[] = "/user_data.txt";
        char *filename = malloc(strlen(userdir_path) + strlen(userdata_path) + 1);
        strcpy(filename, userdir_path);
        strcat(filename, userdata_path);

        dir = opendir(userdir_path);
        file = fopen(filename, "w");
        if (file == NULL) {
            fprintf(stderr, "Error opening user_data file on register_user\n");
            closedir(dir);
            free(filename);
            free(userdir_path);
            return 2;
        }

        fprintf(file, "Username: %s\n", client->username);
        fprintf(file, "Alias: %s\n", client->alias);
        fprintf(file, "Birthday: %s\n", client->birthday);
        fprintf(file, "Identifier: %d\n", client->identifier);
        fprintf(file, "Online: %d\n", client->online);
        fprintf(file, "IP: %s\n", client->ip);
        fprintf(file, "Port: %d\n", client->port);

        fclose(file);
        closedir(dir);
        free(filename);
        free(userdir_path);
        return 0;
    } else {
        fprintf(stderr, "Error registering client\n");
        free(userdir_path);
        return 2;
    }
};

int unregister_user(char *alias) {
    char *userdir_path = malloc(strlen(alias) + 1);
    strcpy(userdir_path, alias);

    if (client_existing(userdir_path) == 0) {
        fprintf(stdout, "Client not registered\n");
        free(userdir_path);
        return 1;
    }

    char userdata_path[] = "/user_data.txt";
    char usermessages_path[] = "/user_messages.txt";
    char *datafilename = malloc(strlen(userdir_path) + strlen(userdata_path) + 1);
    char *messagesfilename = malloc(strlen(userdir_path) + strlen(usermessages_path) + 1);
    strcpy(datafilename, userdir_path);
    strcat(datafilename, userdata_path);
    strcpy(messagesfilename, userdir_path);
    strcat(messagesfilename, usermessages_path);

    if (access(datafilename, F_OK) != -1) {
        if (remove(datafilename) != 0) {
            fprintf(stderr, "Error deleting user_data file on unregister_user\n");
            free(datafilename);
            free(messagesfilename);
            free(userdir_path);
            return 2;
        } else {
            free(datafilename);
        }
    }

    if (access(messagesfilename, F_OK) != -1) {
        if (remove(messagesfilename) != 0) {
            fprintf(stderr, "Error deleting user_messages file on unregister_user\n");
            free(messagesfilename);
            free(userdir_path);
            return 2;
        } else {
            free(messagesfilename);
        }
    }

    if (rmdir(userdir_path) != -1) {
        free(userdir_path);
        return 0;
    } else {
        fprintf(stderr, "Error unregistering client\n");
        free(userdir_path);
        return 2;
    }
};

int connect_user(char *alias, char *ip, int port) {
    char *userdir_path = malloc(strlen(alias) + 1);
    strcpy(userdir_path, alias);

    if (client_existing(userdir_path) == 0) {
        fprintf(stdout, "Client not registered\n");
        free(userdir_path);
        return 1;
    }

    char userdata_path[] = "/user_data.txt";
    char temp_path[] = "/temp.txt";
    char *datafilename = malloc(strlen(userdir_path) + strlen(userdata_path) + 1);
    char *tempfilename = malloc(strlen(userdir_path) + strlen(temp_path) + 1);
    strcpy(datafilename, userdir_path);
    strcat(datafilename, userdata_path);
    strcpy(tempfilename, userdir_path);
    strcat(tempfilename, temp_path);

    if (access(datafilename, F_OK) != -1) {
        FILE *file = fopen(datafilename, "r");
        FILE *temp = fopen(tempfilename, "w");
        char line[256];
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strstr(line, "Online: ") != NULL) {
                int current_online = atoi(line + strlen("Online: "));
                if (current_online == 1) {
                    fprintf(stdout, "Client already online\n");
                    fclose(file);
                    fclose(temp);
                    remove(tempfilename);
                    free(datafilename);
                    free(tempfilename);
                    free(userdir_path);
                    return 2;
                } else {
                    fprintf(temp, "Online: %d\n", 1);
                }
            } else if (strstr(line, "IP: ") != NULL) {
                fprintf(temp, "IP: %s\n", ip);
            } else if (strstr(line, "Port: ") != NULL) {
                fprintf(temp, "Port: %d\n", port);
            } else {
                fprintf(temp, "%s", line);
            }
        }
        fclose(file);
        fclose(temp);
        remove(datafilename);
        rename(tempfilename, datafilename);
        free(datafilename);
        free(tempfilename);
        free(userdir_path);
        return 0;
    } else {
        fprintf(stderr, "Error opening user_data file on connect_user\n");
        free(datafilename);
        free(tempfilename);
        free(userdir_path);
        return 3;
    }
};

int disconnect_user(char *alias) {
    char *userdir_path = malloc(strlen(alias) + 1);
    strcpy(userdir_path, alias);

    if (client_existing(userdir_path) == 0) {
        fprintf(stdout, "Client not registered\n");
        free(userdir_path);
        return 1;
    }

    char userdata_path[] = "/user_data.txt";
    char temp_path[] = "/temp.txt";
    char *datafilename = malloc(strlen(userdir_path) + strlen(userdata_path) + 1);
    char *tempfilename = malloc(strlen(userdir_path) + strlen(temp_path) + 1);
    strcpy(datafilename, userdir_path);
    strcat(datafilename, userdata_path);
    strcpy(tempfilename, userdir_path);
    strcat(tempfilename, temp_path);

    if (access(datafilename, F_OK) != -1) {
        FILE *file = fopen(datafilename, "r");
        FILE *temp = fopen(tempfilename, "w");
        char line[256];
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strstr(line, "Online: ") != NULL) {
                int current_online = atoi(line + strlen("Online: "));
                if (current_online == 0) {
                    fprintf(stdout, "Client not online\n");
                    fclose(file);
                    fclose(temp);
                    remove(tempfilename);
                    free(datafilename);
                    free(tempfilename);
                    free(userdir_path);
                    return 2;
                } else {
                    fprintf(temp, "Online: %d\n", 0);
                }
            } else if (strstr(line, "IP: ") != NULL) {
                fprintf(temp, "IP: \n");
            } else if (strstr(line, "Port: ") != NULL) {
                fprintf(temp, "Port: 0\n");
            } else {
                fprintf(temp, "%s", line);
            }
        }
        fclose(file);
        fclose(temp);
        remove(datafilename);
        rename(tempfilename, datafilename);
        free(datafilename);
        free(tempfilename);
        free(userdir_path);
        return 0;
    } else {
        fprintf(stderr, "Error opening user_data file on connect_user\n");
        free(datafilename);
        free(tempfilename);
        free(userdir_path);
        return 3;
    }
};

int get_identifier (char *alias) {
    char *userdir_path = malloc(strlen(alias) + 1);
    strcpy(userdir_path, alias);

    char userdata_path[] = "/user_data.txt";
    char temp_path[] = "/temp.txt";
    char *datafilename = malloc(strlen(userdir_path) + strlen(userdata_path) + 1);
    char *tempfilename = malloc(strlen(userdir_path) + strlen(temp_path) + 1);
    strcpy(datafilename, userdir_path);
    strcat(datafilename, userdata_path);
    strcpy(tempfilename, userdir_path);
    strcat(tempfilename, temp_path);

    if (access(datafilename, F_OK) != -1) {
        FILE *file = fopen(datafilename, "r");
        FILE *temp = fopen(tempfilename, "w");
        char line[256];
        unsigned int identifier;
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strstr(line, "Identifier: ") != NULL) {
                identifier = atoi(line + strlen("Identifier: "));
                if (identifier == UINT_MAX) {
                    identifier = 1;
                } else {
                    identifier++;
                }
                fprintf(temp, "Identifier: %d\n", identifier);
            } else {
                fprintf(temp, "%s", line);
            }
        }
        fclose(file);
        fclose(temp);
        remove(datafilename);
        rename(tempfilename, datafilename);
        free(datafilename);
        free(tempfilename);
        free(userdir_path);
        return identifier;
    } else {
        fprintf(stderr, "Error opening user_data file on connect_user\n");
        free(datafilename);
        free(tempfilename);
        free(userdir_path);
        return -1;
    }
};

int save_message(char *receiver, struct message_data *message) {
    char *senderdir_path = malloc(strlen(message->sender) + 1);
    char *receiverdir_path = malloc(strlen(receiver) + 1);
    strcpy(senderdir_path, message->sender);
    strcpy(receiverdir_path, receiver);

    if (client_existing(senderdir_path) == 0) {
        fprintf(stdout, "Sender not registered\n");
        free(senderdir_path);
        free(receiverdir_path);
        return 1;
    }
    if (client_existing(receiverdir_path) == 0) {
        fprintf(stdout, "Receiver not registered\n");
        free(senderdir_path);
        free(receiverdir_path);
        return 1;
    }
    if (client_connected(message->sender) == 0) {
        fprintf(stdout, "Sender not online\n");
        free(senderdir_path);
        free(receiverdir_path);
        return 1;
    }

    message->identifier = get_identifier(receiver);
    char receivermessages_path[] = "/user_messages.txt";
    char *messagesfilename = malloc(strlen(receiverdir_path) + strlen(receivermessages_path) + 1);
    strcpy(messagesfilename, receiverdir_path);
    strcat(messagesfilename, receivermessages_path);

    FILE *file = fopen(messagesfilename, "a");
    if (file != NULL) {
        fwrite(message, sizeof(struct message_data), 1, file);
        
        fclose(file);
        free(messagesfilename);
        free(senderdir_path);
        free(receiverdir_path);
        return 0;
    } else {
        fprintf(stderr, "Error opening message file of receiver on save_message server\n");
        free(messagesfilename);
        free(senderdir_path);
        free(receiverdir_path);
        return 2;
    }
};

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
};

int get_ip_and_port(char *alias, char *ip, int *port) {
    char *userdir_path = malloc(strlen(alias) + 1);
    strcpy(userdir_path, alias);

    char userdata_path[] = "/user_data.txt";
    char *datafilename = malloc(strlen(userdir_path) + strlen(userdata_path) + 1);
    strcpy(datafilename, userdir_path);
    strcat(datafilename, userdata_path);

    if (access(datafilename, F_OK) != -1) {
        FILE *file = fopen(datafilename, "r");
        char line[256];
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strstr(line, "IP: ") != NULL) {
                strcpy(ip, line + strlen("IP: "));
            } else if (strstr(line, "Port: ") != NULL) {
                *port = atoi(line + strlen("Port: "));
            }
        }
        fclose(file);
        free(datafilename);
        free(userdir_path);
        return 1;
    }
    fprintf(stderr, "Error opening user_data file on get_ip_port\n");
    free(datafilename);
    free(userdir_path);
    return -1;
}

int enable_connection(char *ip, int port) {
    int socket_fd;
    struct sockaddr_in server_addr;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Error creating socket on server to send message\n");
        return -1;
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_aton(ip, &server_addr.sin_addr);
    
    if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        fprintf(stderr, "Error connecting send message socket to client\n");
        return -1;
    } else {
        return socket_fd;
    }
    return socket_fd;
};

int send_ack_to_sender(char *sender, unsigned int identifier) {
    char buffer[256];
    char sender_ip[20];
    int sender_port;
    int sender_socket_fd;

    if (get_ip_and_port(sender, sender_ip, &sender_port) == -1) {
        fprintf(stderr, "Error getting IP and port of sender for ack\n");
        return -1;
    }

    if ((sender_socket_fd = enable_connection(sender_ip, sender_port)) == -1) {
        fprintf(stderr, "Error enabling connection to sender for ack\n");
        return -1;
    }

    sprintf(buffer, "SEND_MESSAGE_ACK");
    if (sendMessage(sender_socket_fd, buffer, strlen(buffer)+1) == -1) {
        fprintf(stderr, "Error sending ack to sender\n");
        return -1;
    }
    sprintf(buffer, "%d", identifier);
    if (sendMessage(sender_socket_fd, buffer, strlen(buffer)+1) == -1) {
        fprintf(stderr, "Error sending identifier of ack to sender\n");
        return -1;
    }

    close(sender_socket_fd);
    return 1;
}

int send_message(char *alias) {
    char *aliasdir_path = malloc(strlen(alias) + 1);
    strcpy(aliasdir_path, alias);

    if (client_connected(alias) != 1) {
        fprintf(stdout, "Client not online to send messages\n");
        free(aliasdir_path);
        return -1;
    }

    char aliasmessages_path[] = "/user_messages.txt";
    char *messagesfilename = malloc(strlen(aliasdir_path) + strlen(aliasmessages_path) + 1);
    strcpy(messagesfilename, aliasdir_path);
    strcat(messagesfilename, aliasmessages_path);

    FILE *file = fopen(messagesfilename, "r");
    if (file != NULL) {
        char receiver_ip[20];
        int receiver_port;
        int receiver_socket_fd;

        if (get_ip_and_port(alias, receiver_ip, &receiver_port) == -1) {
            fprintf(stderr, "Error getting ip and port of receiver on send_message server\n");
            free(messagesfilename);
            free(aliasdir_path);
            return -1;
        }

        while (file != NULL) {
            char buffer[256];
            struct message_data *message = malloc(sizeof(struct message_data));
            if ((receiver_socket_fd = enable_connection(receiver_ip, receiver_port)) == -1) {
                fprintf(stderr, "Error enabling socket connection to receiver on send_message server\n");
                free(messagesfilename);
                free(aliasdir_path);
                return -1;
            }

            if (fread(message, sizeof(struct message_data), 1, file) == 0) {
                free(message);
                break;
            } else {
                sprintf(buffer, "SEND_MESSAGE");
                if (sendMessage(receiver_socket_fd, buffer, strlen(buffer)+1) == -1) {
                    fprintf(stderr, "Error sending SEND_MESSAGE to receiver on send_message server\n");
                    free(messagesfilename);
                    free(aliasdir_path);
                    return -1;
                }
                sprintf(buffer, "%s", message->sender);
                if (sendMessage(receiver_socket_fd, buffer, strlen(buffer)+1) == -1) {
                    fprintf(stderr, "Error sending sender to receiver on send_message server\n");
                    free(messagesfilename);
                    free(aliasdir_path);
                    return -1;
                }
                sprintf(buffer, "%d", message->identifier);
                if (sendMessage(receiver_socket_fd, buffer, strlen(buffer)+1) == -1) {
                    fprintf(stderr, "Error sending identifier to receiver on send_message server\n");
                    free(messagesfilename);
                    free(aliasdir_path);
                    return -1;
                }
                sprintf(buffer, "%s", message->message);
                if (sendMessage(receiver_socket_fd, buffer, strlen(buffer)+1) == -1) {
                    fprintf(stderr, "Error sending message to receiver on send_message server\n");
                    free(messagesfilename);
                    free(aliasdir_path);
                    return -1;
                }

                if (client_connected(message->sender) == 1) {
                    if (send_ack_to_sender(message->sender, message->identifier) == -1) {
                        fprintf(stderr, "Error sending ack to sender on send_message server\n");
                        free(messagesfilename);
                        free(aliasdir_path);
                        return -1;
                    }
                } 
                free(message);
            }
            close(receiver_socket_fd);
        }

        fclose(file);
        remove(messagesfilename);
        free(messagesfilename);
        free(aliasdir_path);
        return 1;
    } else {
        free(messagesfilename);
        free(aliasdir_path);
        return 0;
    }
}