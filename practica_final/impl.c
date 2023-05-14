#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
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
    fprintf(stdout, "Data file: %s\n", datafilename);

    if (access(datafilename, F_OK) != -1) {
        FILE *file = fopen(datafilename, "r");
        char line[256];
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strstr(line, "Online: ") != NULL) {
                int current_online = atoi(line + strlen("Online: "));
                fprintf(stdout, "Current online: %d\n", current_online);
                if (current_online == 1) {
                    fprintf(stdout, "Client already online\n");
                    fclose(file);
                    free(datafilename);
                    free(userdir_path);
                    return 1;
                } else if (current_online == 0) {
                    fprintf(stdout, "Client not online\n");
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
}

int get_identifier (char *alias) {
    fprintf(stdout, "Alias: %s\n", alias);

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
    fprintf(stdout, "Data file: %s\n", datafilename);
    fprintf(stdout, "Temp file: %s\n", tempfilename);

    if (access(datafilename, F_OK) != -1) {
        FILE *file = fopen(datafilename, "r");
        FILE *temp = fopen(tempfilename, "w");
        char line[256];
        unsigned int identifier;
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strstr(line, "Identifier: ") != NULL) {
                identifier = atoi(line + strlen("Identifier: "));
                fprintf(stdout, "Current identifier:  %d\n", identifier);
                if (identifier == UINT_MAX) {
                    identifier = 1;
                } else {
                    identifier++;
                }
                fprintf(stdout, "New identifier:  %d\n", identifier);
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
}

int register_user (struct client_data *client) {
    fprintf(stdout, "Username: %s\n", client->username);
    fprintf(stdout, "Alias: %s\n", client->alias);
    fprintf(stdout, "Birthday: %s\n", client->birthday);
    fprintf(stdout, "Identifier: %d\n", client->identifier);
    fprintf(stdout, "Online: %d\n", client->online);
    fprintf(stdout, "IP: %s\n", client->ip);
    fprintf(stdout, "Port: %d\n", client->port);

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

        /*
        fscanf(file, "Username: %s\n", client->username);
        fscanf(file, "Alias: %s\n", client->alias);
        fscanf(file, "Birthday: %s\n", client->birthday);
        fscanf(file, "Online: %d\n", &client->online);
        fscanf(file, "IP: %s\n", client->ip);
        fscanf(file, "Port: %d\n", &client->port);
        */
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
    fprintf(stdout, "Data file: %s\n", datafilename);
    fprintf(stdout, "Messages file: %s\n", messagesfilename);

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
    fprintf(stdout, "Alias: %s\n", alias);
    fprintf(stdout, "IP: %s\n", ip);
    fprintf(stdout, "Port: %d\n", port);

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
    fprintf(stdout, "Data file: %s\n", datafilename);
    fprintf(stdout, "Temp file: %s\n", tempfilename);

    if (access(datafilename, F_OK) != -1) {
        FILE *file = fopen(datafilename, "r");
        FILE *temp = fopen(tempfilename, "w");
        char line[256];
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strstr(line, "Online: ") != NULL) {
                int current_online = atoi(line + strlen("Online: "));
                fprintf(stdout, "Current online: %d\n", current_online);
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
    fprintf(stdout, "Alias: %s\n", alias);

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
    fprintf(stdout, "Data file: %s\n", datafilename);
    fprintf(stdout, "Temp file: %s\n", tempfilename);

    if (access(datafilename, F_OK) != -1) {
        FILE *file = fopen(datafilename, "r");
        FILE *temp = fopen(tempfilename, "w");
        char line[256];
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strstr(line, "Online: ") != NULL) {
                int current_online = atoi(line + strlen("Online: "));
                fprintf(stdout, "Current online: %d\n", current_online);
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

int save_message(char *receiver, struct message_data *message) {
    fprintf(stdout, "Sender: %s\n", message->sender);
    fprintf(stdout, "Receiver: %s\n", receiver);
    fprintf(stdout, "Message: %s\n", message->message);

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
    fprintf(stdout, "Identifier: %d\n", message->identifier);

    char receivermessages_path[] = "/user_messages.txt";
    char *messagesfilename = malloc(strlen(receiverdir_path) + strlen(receivermessages_path) + 1);
    strcpy(messagesfilename, receiverdir_path);
    strcat(messagesfilename, receivermessages_path);
    fprintf(stdout, "Messages file: %s\n", messagesfilename);

    FILE *file = fopen(messagesfilename, "a");
    if (file != NULL) {
        fwrite(message, sizeof(struct message_data), 1, file);
        
        fclose(file);
        free(messagesfilename);
        free(senderdir_path);
        free(receiverdir_path);
        return 0;
    } else {
        fprintf(stderr, "Error opening message file of receiver on send server\n");
        free(messagesfilename);
        free(senderdir_path);
        free(receiverdir_path);
        return 2;
    }
};
