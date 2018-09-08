#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int BUF_SIZE = 2000;
const int BUF_INC = 1000;
const char* SPACE = " ";

void getLine(char* userIn);
char** parseString(char* str, const char* delim);


int main() {
    char* userIn = malloc(sizeof(char)*BUF_SIZE);
    char** parsedIn;
    while (1){
        printf("$ ");
        getLine(userIn);
        parsedIn = parseString(userIn, SPACE);
        int i = 0;
        while (*(parsedIn + i) != NULL){
            printf("%s\n", *(parsedIn + i));
            i++;
        }
        if (strcmp(parsedIn[0],"exit") == 0)
            break;
    }
    free(userIn);
    for (int i = 0; i < sizeof(parsedIn)/sizeof(char**); i++){
        free(*(parsedIn + i));
    }
    free(parsedIn);
    exit (0);
}


//get a line of input from the user. Store the line in the userIn parameter. Will automatically resize the userIn
//memory if input becomes too large
void getLine (char* userIn){
    int bufferSize = BUF_SIZE;
    userIn = realloc(userIn, sizeof(char) * bufferSize);
    int c;
    int i = 0;
    while (1){
        c = getchar();
        if (c == '\n' || c == EOF){
            *(userIn + i) = '\0';
            break;
        }
        else{
            *(userIn + i) = c;
        }
        i++;
        if (i >= BUF_SIZE){
            bufferSize = bufferSize + BUF_INC;
            userIn = realloc(userIn, sizeof(char) * (bufferSize));
            if (userIn == NULL){
                printf("Error resizing user input buffer\n");
                exit(-1);
            }
        }
    }
    return;
}



//Parses the string in str into an array of strings separated by the delimiter delim
char** parseString(char* str, const char* delim){
    char* tokenized = strtok(str, delim);
    char** parsed = malloc(sizeof(char*));
    int parsedPtr = 0;
    int stringNum = 1;
    while (tokenized != NULL) {
        *(parsed + parsedPtr) = malloc(sizeof(char) * strlen(tokenized));
        strcpy(*(parsed + parsedPtr), tokenized);
        parsedPtr++;
        stringNum++;
        tokenized = strtok(NULL, delim);
        parsed = realloc(parsed, stringNum*sizeof(char *));
    }
    *(parsed + parsedPtr) = NULL;
    return parsed;
}