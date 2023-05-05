#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "server.h"

int client_existing (char *userdir_path) {
    DIR *dir;

    if ((dir = opendir(userdir_path)) == NULL) {
        return -1;                                              // If directory doesn't exist, client not registered
    } else {
    }

    closedir(dir);
    return 0;
};

int register_user (struct client_data *client) {
    fprintf(stdout, "Username: %s\n", client->username);
    fprintf(stdout, "Alias: %s\n", client->alias);
    fprintf(stdout, "Birthday: %s\n", client->birthday);
    fprintf(stdout, "Online: %d\n", client->online);
    fprintf(stdout, "IP: %s\n", client->ip);
    fprintf(stdout, "Port: %d\n", client->port);

    char *userdir_path = malloc(strlen(client->username) + 1);
    strcpy(userdir_path, client->username);

    if (client_existing(userdir_path) == 0) {
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
