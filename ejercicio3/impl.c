#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "servidor.h"

FILE *file;
int array_key, array_value2;
double array_value3;
char array_value1[256];

int init() {
    FILE *file;
    file = fopen("arrays.txt", "w");
    if (file == NULL) {
        perror("Error opening file on init.");
        return -1;
    }
    fclose(file);
    return 0;
}

int set_value(int key, char *value1, int value2, double value3) {
    FILE *file;
    int array_key, array_value2;
    char array_value1[256];
    double array_value3;
    

    if (exist(key)) {                                           // if key already exists, return error 
        perror("Key already exists on set_value.");
        return -1;
    }
    
    array_key = key;                                            // copy values to set in the variables
    strcpy(array_value1, value1);
    array_value2 = value2;
    array_value3 = value3;
    file = fopen("arrays.txt", "a");                            // open file to write at the end
    if (file == NULL) {
        perror("Error opening file on set_value.");
        return -1;
    }

    fprintf(file, "%d %s %d %f\n", array_key, array_value1, array_value2, array_value3);            // write the tuple in the position that was opened, a line for each tuple

    printf("Value set correctly: %d %s %d %f\n", array_key, array_value1, array_value2, array_value3);

    fclose(file);
    return 0;
}

int get_value(int key, char *value1, int *value2, double *value3) {
    FILE *file;
    int array_key, array_value2;
    char array_value1[256];
    double array_value3;

    if (!exist(key)) {                                        // if key does not exist, return error
        perror("Key does not exist on get_value.");
        return -1;
    }

    file = fopen("arrays.txt", "r");                          // open file to read
    if (file == NULL) {
        perror("Error opening file on get_value.");
        return -1;
    }

    char line[256];                                         // set size of line
    while (fgets(line, sizeof(line), file) != NULL) {       // read line by line
        sscanf(line, "%d %s %d %lf", &array_key, array_value1, &array_value2, &array_value3);      // copy values of the line to the variables 
        if (array_key == key) {                             // if key of the line is key we are looking for copy values
            strcpy(value1, array_value1);                   
            *value2 = array_value2;                         
            *value3 = array_value3;
            break;
        }
    }

    fclose(file);
    return 0;
}

int modify_value(int key, char *value1, int value2, double value3) {
    FILE *file;
    int array_key, array_value2;
    char array_value1[256];
    double array_value3;

    if (!exist(key)) {                                      // if key does not exist, return error                
        perror("Key does not exist on modify_value.");
        return -1;
    }

    file = fopen("arrays.txt", "r");                        // open file to read
    FILE *temp = fopen("temp.txt", "w");                    // create temporal file to write
    if (file == NULL || temp == NULL) {
        perror("Error opening file on delete_key.");
        return -1;
    }

    char line[256];                                         // set size of line                      
    while (fgets(line, sizeof(line), file) != NULL) {       // read line by line
        sscanf(line, "%d %s %d %lf", &array_key, array_value1, &array_value2, &array_value3);            // copy values of the line to the variables
        if (array_key == key) {                             // if key of the line is key we are looking for
            fprintf(temp, "%d %s %d %lf\n", key, value1, value2, value3);        // write the new values in the line
        } else {
            fprintf(temp, "%d %s %d %lf\n", array_key, array_value1, array_value2, array_value3);      // write the old values
        }
    }

    fclose(file);
    fclose(temp);
    remove("arrays.txt");                                   // remove the original file
    rename("temp.txt", "arrays.txt");                       // rename the temporal file to the original file
    return 0;
}

int delete_key(int key) {    
    FILE *file;
    int array_key, array_value2;
    char array_value1[256];
    double array_value3;

    if (!exist(key)) {                                      // if key does not exist, return error
        perror("Key does not exist on delete_key.");
        return -1;
    }

    file = fopen("arrays.txt", "r");                        // open file to read
    FILE *temp = fopen("temp.txt", "w");                    // create temporal file to write
    if (file == NULL || temp == NULL) {
        perror("Error opening file on delete_key.");
        return -1;
    }

    char line[256];                                         // set size of line
    while (fgets(line, sizeof(line), file) != NULL) {       // read line by line
        sscanf(line, "%d %s %d %lf", &array_key, array_value1, &array_value2, &array_value3);            // copy values of the line to the variables
        if (array_key != key) {                             // if key of the line is not key we are looking for write the line in the temporal file
            fprintf(temp, "%d %s %d %lf\n", array_key, array_value1, array_value2, array_value3);
        }
    }

    fclose(file);
    fclose(temp);
    remove("arrays.txt");                                   // remove the original file
    rename("temp.txt", "arrays.txt");                       // rename the temporal file to the original file
    return 0;
}

int exist(int key) {
    FILE *file;
    
    file = fopen("arrays.txt", "r");                        // open file to read
    if (file == NULL) {
        perror("Error opening file on exist.");
        return -1;
    }

    char line[256];                                         // set size of line
    int found = 0;                                          // set found to 0
    while (fgets(line, sizeof(line), file) != NULL) {       // read line by line
        int k;                                              // set a variable where we copy the keys we read
        sscanf(line, "%d", &k);
        if (k == key) {                                     // if key of the line is key we are looking for set found to 1
            found = 1;
            break;
        }
    }

    fclose(file);
    if (found) {
        return 1;                                           // return 1 if found
    } else {
        return 0;                                           // return 0 if not found
    }
}

int copy_key(int key1, int key2) {
    char array_value1[256];
    int array_value2;
    double array_value3;

    if (!exist(key1)) {                                     // if key1 does not exist, return error
        perror("Key1 does not exist on copy_key.");
        return -1;
    }

    get_value(key1, array_value1, &array_value2, &array_value3);            // get the values of key1

    if (!exist(key2)) {                                                     // if key2 does not exist, call set value with key2 and the values of key1
        set_value(key2, array_value1, array_value2, array_value3);
    } else {                                                                // if key2 exists, call modify value with key2 and the values of key1
        modify_value(key2, array_value1, array_value2, array_value3);       
    }

    return 0;
}