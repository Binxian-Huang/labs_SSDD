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
        exit(1);
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

        // printf("Operation code: ");
        // fflush(stdin);
        // fgets(input, sizeof(input), stdin);
        // if (sscanf(input, "%d %*s", &pet.operation) != 1 || sscanf(input, "%*s") > 0) {
        //     printf("You must introduce one operation code.\n");
        // } else if (pet.operation == 0) { // init
        //     char last_char = getchar();
        //     if (last_char != '\n') {  //after operation code there is other values
        //         perror("Wrong number of arguments on init.\n");
        //         break;
        //     } else {
        //         res = init();
        //         if (res == 0) {
        //             printf("Initilization correct.\n");
        //         } else {
        //             printf("An error ocurred while initilizing.\n");
        //         }
        //     }
        printf("Operation code: ");
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%1d%*c", &pet.operation) != 1 || pet.operation < 0 || pet.operation > 7 || (input[1] != '\0' && input[1] != '\n')) {
            printf("You must introduce one operation code.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }

        if (pet.operation == 0) { // init
            res = init();
            if (res == 0) {
                printf("Initilization correct.\n");
            } else {
                printf("An error ocurred while initilizing.\n");
            }
        } else if (pet.operation == 1) { // set_value
            printf("Arguments for set_value: ");
            fflush(stdin);
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d \"%[^\"]\" %d %lf %*s", &pet.key, pet.value1, &pet.value2, &pet.value3) != 4 || sscanf(input, "%*s") > 0) {  //  key value1 value2 value3
                perror("Wrong number of arguments on set_value.\n");
                break;
            } else {
                res = set_value(pet.key, pet.value1, pet.value2, pet.value3);
                if (res == 1) {
                    printf("The value with key %d set correctly.\n", pet.key);
                } else {
                    printf("An error ocurred while setting the value with key %d.\n", pet.key);
                }
            }
        } else if (pet.operation == 2) { // get_value
            printf("Arguments for get_value: ");
            fflush(stdin);
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d %*s", &pet.key) != 1 || sscanf(input, "%*s") > 0) {  // key 
                perror("Wrong number of arguments on get_value.\n");
                break;
            } else {
                res = get_value(pet.key, pet.value1, &pet.value2, &pet.value3);
                if (res == 1) {
                    printf("The value with key %d is: %s %d %f\n", pet.key, pet.value1, pet.value2, pet.value3);
                } else {
                    printf("An error ocurred while getting the value with key %d.\n", pet.key);
                }
            }
        } else if (pet.operation == 3) { // modify_value
            printf("Arguments for modify_value: ");
            fflush(stdin);
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d \"%[^\"]\" %d %lf %*s", &pet.key, pet.value1, &pet.value2, &pet.value3) != 4 || sscanf(input, "%*s") > 0) {  // key value1 value2 value3
                perror("Wrong number of arguments on modify_value.\n");
                break;
            } else {
                res = modify_value(pet.key, pet.value1, pet.value2, pet.value3);
                if (res == 1) {
                    printf("The value with key %d modified correctly.\n", pet.key);
                } else {
                    printf("An error ocurred while modifying the value with key %d.\n", pet.key);
                }
            }
        } else if (pet.operation == 4) { // delete_key
            printf("Arguments for delete_key: ");
            fflush(stdin);
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d %*s", &pet.key) != 1 || sscanf(input, "%*s") > 0) {  // key
                perror("Wrong number of arguments on delete_key.\n");
                break;
            } else {
                res = delete_key(pet.key);
                if (res == 1) {
                    printf("The value with key %d deleted correctly.\n", pet.key);
                } else {
                    printf("An error ocurred while deleting the value with key %d.\n", pet.key);
                }
            }
        } else if (pet.operation == 5) { // exist
            printf("Arguments for exist: ");
            fflush(stdin);
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d %*s", &pet.key) != 1 || sscanf(input, "%*s") > 0) {  // key
                perror("Wrong number of arguments on exist.\n");
                break;
            } else {
                res = exist(pet.key);
                if (res == 1) {
                    printf("The value with key %d exists.\n", pet.key);
                } else {
                    printf("The value with key %d does not exist.\n", pet.key);
                }
            }
        } else if (pet.operation == 6) { // copy_key
            printf("Arguments for copy_key: ");
            fflush(stdin);
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d %d %*s", &pet.key, &pet.key2) != 2 || sscanf(input, "%*s") > 0) {  // key1 key2
                perror("Wrong number of arguments on copy_key.\n");
                break;
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
        } else {
            printf("Wrong operation code.\n");
        }
    }

    disable_connection();
    printf("Connection disabled.\n");
    exit(0);
}