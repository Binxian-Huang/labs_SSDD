#define MAXSIZE 256

struct petition {
    int operation;
    int key;
    int key2;
    char value1[256];
    int value2;
    double value3;
    char q_name[MAXSIZE];
};

struct result {
    int result;
    char value1[256];
    int value2;
    double value3;
    char q_name[MAXSIZE];
};

int init();

int set_value(int key, char *value1, int value2, double value3);

int get_value(int key, char *value1, int *value2, double *value3);

int modify_value(int key, char *value1, int value2, double value3);

int delete_key(int key);

int exist(int key);

int copy_key(int key1, int key2);
