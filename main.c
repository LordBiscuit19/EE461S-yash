#include <stdio.h>
#include <stdlib.h>

const int BUF_SIZE = 2000;
const int BUF_INC = 1000;

void getLine(char* userIn);
int strCmp(char* str1, char* str2);
char** parseString(char* str, char delim);


int main() {
    char* userIn = malloc(sizeof(char)*BUF_SIZE);
    while (1){
        printf("$ ");
        getLine(userIn);
        if (strCmp(userIn,"exit") == 1)
            break;
    }
    free(userIn);
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


//compares 2 strings. If they are the same return 1 and if they are not return -1.
int strCmp(char* str1, char* str2){
    int i = 0;
    int j = 0;
    while (*(str1 + i) != '\0' && *(str2 + j) != '\0'){
        if (*(str1 + i) != *(str2 + j)){
            return (-1);
        }
        i++;
        j++;
    }
    if (*(str1 + i) == *(str2 + j)){
        return (1);
    }
    else
        return (-1);
}



char** parseString(char* str, char delim){
    int begin = 0;
    int end = 0;
    int  parsedPtr = 0;
    char** parsed = malloc(sizeof(char*));
    while (*(str + begin) != '\0'){
        while (*(str + end) != delim && *(str + end) != '\0'){
            end++;
        }
        *(parsed + parsedPtr) = malloc(sizeof(char) * (1 + end - begin));
        int i = 0;
        while (end < begin-1) {
            *(*(parsed + parsedPtr) + i) = *(str + end);
            end++;
            i++;
        }

    }
}