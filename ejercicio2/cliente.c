#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "claves.h"

/* env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente */
int main(int argc, char *argv[]) {
    struct petition pet;
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
        pet.operation = -1;

        printf("Operation code: ");
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%d", &pet.operation) == 0 || pet.operation < 0 || pet.operation > 7 || (strlen(input) > 2) || ((strlen(input) == 2) && (!isdigit(input[0]) || !isspace(input[1])))) {
            printf("You must introduce one operation code.\n");
            continue;
        }

        memset(input, 0, sizeof(input));
        if (pet.operation == 0) { // init
            res = init();
            if (res == 0) {
                printf("Initilization correct.\n");
            } else {
                printf("An error ocurred while initilizing.\n");
            }
        } else if (pet.operation == 1) { // set_value
            printf("Arguments for set_value: ");
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d \"%[^\"]\" %d %lf", &pet.key, pet.value1, &pet.value2, &pet.value3) != 4) {  //  key value1 value2 value3
                perror("Wrong number of arguments on set_value.\n");
                continue;
            } else {
                res = set_value(pet.key, pet.value1, pet.value2, pet.value3);
                if (res == 0) {
                    printf("The value with key %d set correctly.\n", pet.key);
                } else {
                    printf("An error ocurred while setting the value with key %d.\n", pet.key);
                }
            }
        } else if (pet.operation == 2) { // get_value
            printf("Arguments for get_value: ");
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d", &pet.key) != 1 || strlen(input) != 2) {  // key 
                perror("Wrong number of arguments on get_value.\n");
                continue;
            } else {
                res = get_value(pet.key, pet.value1, &pet.value2, &pet.value3);
                if (res == 0) {
                    printf("The value with key %d is: %s %d %f\n", pet.key, pet.value1, pet.value2, pet.value3);
                } else {
                    printf("An error ocurred while getting the value with key %d.\n", pet.key);
                }
            }
        } else if (pet.operation == 3) { // modify_value
            printf("Arguments for modify_value: ");
            if (scanf("%d \"%[^\"]\" %d %lf", &pet.key, pet.value1, &pet.value2, &pet.value3) != 4) {  // key value1 value2 value3
                perror("Wrong number of arguments on modify_value.\n");
                continue;
            } else {
                res = modify_value(pet.key, pet.value1, pet.value2, pet.value3);
                if (res == 0) {
                    printf("The value with key %d modified correctly.\n", pet.key);
                } else {
                    printf("An error ocurred while modifying the value with key %d.\n", pet.key);
                }
            }
        } else if (pet.operation == 4) { // delete_key
            printf("Arguments for delete_key: ");
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d", &pet.key) != 1 || strlen(input) != 2) {  // key
                perror("Wrong number of arguments on delete_key.\n");
                continue;
            } else {
                res = delete_key(pet.key);
                if (res == 0) {
                    printf("The value with key %d deleted correctly.\n", pet.key);
                } else {
                    printf("An error ocurred while deleting the value with key %d.\n", pet.key);
                }
            }
        } else if (pet.operation == 5) { // exist
            printf("Arguments for exist: ");
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d", &pet.key) != 1 || strlen(input) != 2) {  // key
                perror("Wrong number of arguments on exist.\n");
                continue;
            } else {
                res = exist(pet.key);
                if (res == 1) {
                    printf("The value with key %d exists.\n", pet.key);
                } else if (res == 0){
                    printf("The value with key %d does not exist.\n", pet.key);
                } else {
                    printf("An error ocurred while checking if the value with key %d exists.\n", pet.key);
                }
            }
        } else if (pet.operation == 6) { // copy_key
            printf("Arguments for copy_key: ");
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d %d", &pet.key, &pet.key2) != 2 || strlen(input) != 4) {  // key1 key2
                perror("Wrong number of arguments on copy_key.\n");
                continue;
            } else {
                res = copy_key(pet.key, pet.key2);
                if (res == 0) {
                    printf("The value with key %d copied correctly to key %d.\n", pet.key, pet.key2);
                } else {
                    printf("An error ocurred while copying the value with key %d to key %d.\n", pet.key, pet.key2);
                }
            }
        } else if (pet.operation == 7) { // EXIT
            printf("Exiting...\n");
            break;
        }
    }
    exit(0);
}