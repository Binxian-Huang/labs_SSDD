#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"


int main(int argc, char *argv[]) {
    struct petition pet;
    int res;

    if (argc != 1){
        printf("Usage: ./cliente. IP address 127.0.0.1 and port 8080 already set.");
    }

    res = enable_connection();

    if (res == 0) {
        printf("Connection enabled.\n");
    } else {
        printf("Connection not enabled.\n");
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
        printf("Operation code and arguments: ", 0, 1, 2, 4);
        scanf("%d", &pet.operation);

        if (strcmp(pet.operation, "0") == 0) { // init
            if (getchar() != "\n") {  //after operation code there is other values
                perror("Wrong number of arguments on init.\n");
            } else {
                res = init();
                if (res == 0) {
                    printf("Initilization correct.\n");
                }
            }
        }

        if (strcmp(pet.operation, "1") == 0) { // set_value
            if (scanf("%d %d %c %lf", &pet.key, pet.value1, &pet.value2, &pet.value3) != 4) {  //  key value1 value2 value3
                perror("Wrong number of arguments on set_value.\n");
            } else {
                res = set_value(pet.key, &pet.value1, pet.value2, pet.value3)
                if (res == 1) {
                    printf("The value with key %d set correctly.\n", pet.key);
                } else {
                    printf("An error ocurred while setting the value with key %d.\n", pet.key);
                }
            }
        }

        if (strcmp(pet.operation, "2") == 0) { // get_value
            if (scanf("%d", &pet.key) != 1) {  // key 
                perror("Wrong number of arguments on get_value.\n");
            } else {
                res = get_value(pet.key, pet.value1, &pet.value2, &pet.value3);
                if (res == 1) {
                    printf("The value with key %d is: %s %d %f\n", pet.key, pet.value1, pet.value2, pet.value3);
                } else {
                    printf("An error ocurred while getting the value with key %d.\n", pet.key);
                }
            }
        }

        if (strcmp(pet.operation, "3") == 0) { // modify_value
            if (scanf("%d %d %c %lf", &pet.key, pet.value1, &pet.value2, &pet.value3) != 4) {  // key value1 value2 value3
                perror("Wrong number of arguments on modify_value.\n");
            } else {
                res = modify_value(pet.key, &pet.value1, pet.value2, pet.value3);
                if (res == 1) {
                    printf("The value with key %d modified correctly.\n", pet.key);
                } else {
                    printf("An error ocurred while modifying the value with key %d.\n", pet.key);
                }
            }
        }

        if (strcmp(pet.operation, "4") == 0) { // delete_key
            if (scanf("%d", &pet.key) != 1) {  // key
                perror("Wrong number of arguments on delete_key.\n");
            } else {
                res = delete_key(pet.key);
                if (res == 1) {
                    printf("The value with key %d deleted correctly.\n", pet.key);
                } else {
                    printf("An error ocurred while deleting the value with key %d.\n", pet.key);
                }
            }
        }

        if (strcmp(pet.operation, "5") == 0) { // exist
            if (scanf("%d", &pet.key) != 1) {  // key
                perror("Wrong number of arguments on exist.\n");
            } else {
                res = exist(pet.key);
                if (res == 1) {
                    printf("The value with key %d exists.\n", pet.key);
                } else {
                    printf("The value with key %d does not exist.\n", pet.key);
                }
            }
        }

        if (strcmp(pet.operation, "6") == 0) { // copy_key
            if (scanf("%d %d", &pet.key, &pet.key2) != 2) {  // key1 key2
                perror("Wrong number of arguments on copy_key.\n");
            } else {
                res = copy_key(pet.key, pet.key2);
                if (res == 0) {
                    printf("The value with key %d copied correctly to key %d.\n", pet.key, pet.key2);
                } else {
                    printf("An error ocurred while copying the value with key %d to key %d.\n", pet.key, pet.key2);
                }
            }
        }

        if (strcmp(pet.operation, "7") == 0) { // EXIT
            break;
        }
    }

    disable_connection();
    return 0;
}