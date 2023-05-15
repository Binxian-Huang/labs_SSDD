struct client_data {
    char username[20];
    char alias[20];
    char birthday[20];
    unsigned int identifier;
    int online;
    char ip[20];
    int port;
};

struct message_data {
    char sender[20];
    unsigned int identifier;
    char message[100];
};

int sendMessage(int socket_fd, char *buffer, int size);

int register_user(struct client_data *client);

int unregister_user(char *username);

int connect_user(char *alias, char *ip, int port);

int disconnect_user(char *alias);

int save_message(char *receiver, struct message_data *message);

int send_message(char *alias);
