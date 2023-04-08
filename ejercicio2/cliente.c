#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"


int main(int argc, char *argv[]) {
    struct petition pet;
    int res;

    if (strcmp(argv[1], "0") == 0) { // init
        if (argc != 2) {  // ./cliente operation
            perror("Wrong number of arguments on init.");
            exit(-1);
        } else {
            res = init();
            if (res == 0) {
                printf("Initilization correct.\n");
            }
        }
    }

    if (strcmp(argv[1], "1") == 0) { // set_value
        if (argc != 6) {  // ./cliente operation key value1 value2 value3
            perror("Wrong number of arguments on set_value.");
            exit(-1);
        } else {
            res = set_value(atoi(argv[2]), argv[3], atoi(argv[4]), atof(argv[5]));
            if (res == 1) {
                printf("The value with key %d set correctly.\n", atoi(argv[2]));
            } else {
                printf("An error ocurred while setting the value with key %d.\n", atoi(argv[2]));
            }
        }
    }

    if (strcmp(argv[1], "2") == 0) { // get_value
        if (argc != 3) {  // ./cliente operation key 
            perror("Wrong number of arguments on get_value.");
            exit(-1);
        } else {
            res = get_value(atoi(argv[2]), pet.value1, &pet.value2, &pet.value3);
            if (res == 1) {
                printf("The value with key %d is: %s %d %f\n", atoi(argv[2]), pet.value1, pet.value2, pet.value3);
            } else {
                printf("An error ocurred while getting the value with key %d.\n", atoi(argv[2]));
            }
        }
    }

    if (strcmp(argv[1], "3") == 0) { // modify_value
        if (argc != 6) {  // ./cliente operation key value1 value2 value3
            perror("Wrong number of arguments on modify_value.");
            exit(-1);
        } else {
            res = modify_value(atoi(argv[2]), argv[3], atoi(argv[4]), atof(argv[5]));
            if (res == 1) {
                printf("The value with key %d modified correctly.\n", atoi(argv[2]));
            } else {
                printf("An error ocurred while modifying the value with key %d.\n", atoi(argv[2]));
            }
        }
    }

    if (strcmp(argv[1], "4") == 0) { // delete_key
        if (argc != 3) {  // ./cliente operation key
            perror("Wrong number of arguments on delete_key.");
            exit(-1);
        } else {
            res = delete_key(atoi(argv[2]));
            if (res == 1) {
                printf("The value with key %d deleted correctly.\n", atoi(argv[2]));
            } else {
                printf("An error ocurred while deleting the value with key %d.\n", atoi(argv[2]));
            }
        }
    }

    if (strcmp(argv[1], "5") == 0) { // exist
        if (argc != 3) {  // ./cliente operation key
            perror("Wrong number of arguments on exist.");
            exit(-1);
        } else {
            res = exist(atoi(argv[2]));
            if (res == 1) {
                printf("The value with key %d exists.\n", atoi(argv[2]));
            } else {
                printf("The value with key %d does not exist.\n", atoi(argv[2]));
            }
        }
    }

    if (strcmp(argv[1], "6") == 0) { // copy_key
        if (argc != 4) {  // ./cliente operation key1 key2
            perror("Wrong number of arguments on copy_key.");
            exit(-1);
        } else {
            res = copy_key(atoi(argv[2]), atoi(argv[3]));
            if (res == 0) {
                printf("The value with key %d copied correctly to key %d.\n", atoi(argv[2]), atoi(argv[3]));
            } else {
                printf("An error ocurred while copying the value with key %d to key %d.\n", atoi(argv[2]), atoi(argv[3]));
            }
        }
    }

    return 0;
}