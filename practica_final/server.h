struct client_data {
    char username[20];
    char alias[20];
    char birthday[20];
    int online;
    char ip[20];
    int port;
};

int register_user(struct client_data *client);

int unregister_user(char *username);

int connect_user(char *alias, char *ip, int port);

int disconnect_user(char *alias);

