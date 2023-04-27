#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "claves.h"

/* env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente */
int main(int argc, char *argv[]) {
    int operation;
    int key;
    int key2;
    char value1[256];
    int value2;
    double value3;
    int res;

    if (argc != 1){
        printf("Usage: ./cliente\n");
    }

    printf("These are the operations you can use:\n");
    printf("init - 0\n");
    printf("set_value - 1 <key> <value1> <value2> <value3>\n");
    printf("get_value - 2 <key>\n");
    printf("modify_value - 3 <key> <value1> <value2> <value3>\n");
    printf("delete_key - 4 <key>\n");
    printf("exist - 5 <key>\n");
    printf("copy_key - 6 <key1> <key2>\n");
    printf("EXIT - 7\n");

    while (1)
    {
        char input[512];
        operation = -1;

        printf("Operation code: ");
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%d", &operation) == 0 || operation < 0 || operation > 7 || (strlen(input) > 2) || ((strlen(input) == 2) && (!isdigit(input[0]) || !isspace(input[1])))) {
            printf("You must introduce one operation code.\n");
            continue;
        }

        memset(input, 0, sizeof(input));
        if (operation == 0) { // init
            res = init();
            if (res == 0) {
                printf("Initilization correct.\n");
            } else {
                printf("An error ocurred while initilizing.\n");
            }
        } else if (operation == 1) { // set_value
            printf("Arguments for set_value: ");
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d \"%[^\"]\" %d %lf", &key, value1, &value2, &value3) != 4) {  //  key value1 value2 value3
                perror("Wrong number of arguments on set_value.\n");
                continue;
            } else {
                res = set_value(key, value1, value2, value3);
                if (res == 0) {
                    printf("The value with key %d set correctly.\n", key);
                } else {
                    printf("An error ocurred while setting the value with key %d.\n", key);
                }
            }
        } else if (operation == 2) { // get_value
            printf("Arguments for get_value: ");
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d", &key) != 1 || strlen(input) != 2) {  // key 
                perror("Wrong number of arguments on get_value.\n");
                continue;
            } else {
                res = get_value(key, value1, &value2, &value3);
                if (res == 0) {
                    printf("The value with key %d is: %s %d %f\n", key, value1, value2, value3);
                } else {
                    printf("An error ocurred while getting the value with key %d.\n", key);
                }
            }
        } else if (operation == 3) { // modify_value
            printf("Arguments for modify_value: ");
            if (scanf("%d \"%[^\"]\" %d %lf", &key, value1, &value2, &value3) != 4) {  // key value1 value2 value3
                perror("Wrong number of arguments on modify_value.\n");
                continue;
            } else {
                res = modify_value(key, value1, value2, value3);
                if (res == 0) {
                    printf("The value with key %d modified correctly.\n", key);
                } else {
                    printf("An error ocurred while modifying the value with key %d.\n", key);
                }
            }
        } else if (operation == 4) { // delete_key
            printf("Arguments for delete_key: ");
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d", &key) != 1 || strlen(input) != 2) {  // key
                perror("Wrong number of arguments on delete_key.\n");
                continue;
            } else {
                res = delete_key(key);
                if (res == 0) {
                    printf("The value with key %d deleted correctly.\n", key);
                } else {
                    printf("An error ocurred while deleting the value with key %d.\n", key);
                }
            }
        } else if (operation == 5) { // exist
            printf("Arguments for exist: ");
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d", &key) != 1 || strlen(input) != 2) {  // key
                perror("Wrong number of arguments on exist.\n");
                continue;
            } else {
                res = exist(key);
                if (res == 1) {
                    printf("The value with key %d exists.\n", key);
                } else if (res == 0){
                    printf("The value with key %d does not exist.\n", key);
                } else {
                    printf("An error ocurred while checking if the value with key %d exists.\n", key);
                }
            }
        } else if (operation == 6) { // copy_key
            printf("Arguments for copy_key: ");
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d %d", &key, &key2) != 2 || strlen(input) != 4) {  // key1 key2
                perror("Wrong number of arguments on copy_key.\n");
                continue;
            } else {
                res = copy_key(key, key2);
                if (res == 0) {
                    printf("The value with key %d copied correctly to key %d.\n", key, key2);
                } else {
                    printf("An error ocurred while copying the value with key %d to key %d.\n", key, key2);
                }
            }
        } else if (operation == 7) { // EXIT
            printf("Exiting...\n");
            break;
        }
    }
    exit(0);
}