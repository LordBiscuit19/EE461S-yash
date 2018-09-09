#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

const int BUF_SIZE = 2000;
const int BUF_INC = 1000;


void getLine(char* userIn);
char** parseString(char* str, const char* delim);
int exCmd(char** args);
int lnchPrg(char** args);
int yashCd(char** args);
int yashHelp(char** args);
int yashExit(char** args);

const int (*SHELL_FNCTS[])(char**) = {&yashCd};
const char* SPACE = " ";
const char* SHELL_CMDS[] = { "cd", "help", "exit"};
const int NUM_SHELL_CMDS = 3;


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
        if (exCmd(parsedIn) == 42){
            i = 0;
            while (*(parsedIn + i) != NULL){
                free(*(parsedIn + i));
                i++;
            }
            free(userIn);
            free(parsedIn);
            exit (0);
        }
        i = 0;
        while (*(parsedIn + i) != NULL){
            free(*(parsedIn + i));
            i++;
        }
    }

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


//executes the user command. First it checks to see if the command is one of the built in commands in the shell
int exCmd(char** args){
    for (int i = 0; i < NUM_SHELL_CMDS; i++){
        if (strcmp(args[0], SHELL_CMDS[i]) == 0){
            return (*SHELL_FNCTS[i])(args);
        }
    }
    return lnchPrg(args);
}


//executes all user commands that require an external program
int lnchPrg(char** args){
    pid_t pid;
    pid = fork();
    int* waitStat;
    if (pid == 0){
        execvp(args[0], args);
        printf("execvp error in lnchPrg");
        return(-1);
    }
    if (pid == -1){
        printf("error forking from lnchPrg");
        return(-2);
    }
    waitpid(pid, &waitStat, 0);
    return (0);
}

int yashCd(char** args){
    printf("testing cd");
    return 1;
}

int yashExit(char** args){
    return 42;
}