#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>
#include "LList.h"

const int BUF_SIZE = 2000;
const int BUF_INC = 1000;


_Bool getLine(char* userIn);
char** parseString(char* str, const char* delim);
int exCmd1(char** args);
int exCmd2(char** args1, char** arg2);
int lnchPrg1(char** args);
int lnchPrg2 (char** args1, char** args2);
int yashHelp(char** args);
int yashExit(char** args);
int yashJobs(char**args);
int lookForPipe(char** args);
int lookForRedir(char** args, int* returns);
_Bool lookForBGProcess(char** args, _Bool removeSymbol);
void setUpRedir (char** args, _Bool output, _Bool input, _Bool error);

int (*SHELL_FNCTS[])(char**) = {&yashExit, &yashJobs};
const char* SPACE = " ";
const char* SHELL_CMDS[] = {"exit", "jobs"};
const int NUM_SHELL_CMDS = 2;
int topPid;
int fgPid = -1;
int numJobs = 0;
struct node* head = NULL;

void handle_SIGINT(){
    if (fgPid != -1)
        head = removeNodePid(head, fgPid);
    //kill((-1)*topPid, SIGINT);
}

void handle_SIGTSTP(){
    if (fgPid != -1){
        stopJob(head, fgPid);
        fgPid = -1;
    }
}


void handle_SIGCHLD(){
    int pid, status;
    pid = waitpid(-1, &status, WNOHANG);
    //printf("SIGCHLD pid: %d\n", pid);
    if (pid != fgPid)
        head = removeNodePid(head, pid);
}


int main() {
    char* userIn = malloc(sizeof(char)*BUF_SIZE);
    char** parsedIn;
    char** parsedIn2 = NULL;
    signal(SIGINT, handle_SIGINT);
    signal(SIGTSTP, handle_SIGTSTP);
    signal(SIGCHLD, handle_SIGCHLD);
    while (1){
        printf("# ");
        if(getLine(userIn)){
            exit(0);
        }
        parsedIn = parseString(userIn, SPACE);
        int i = 0;
        int pipeOffset = lookForPipe(parsedIn);
        if (pipeOffset != -1){
            parsedIn2 = parsedIn + pipeOffset + 1;    //parsedIn2 is the command after the pipe
            *(parsedIn + pipeOffset) = NULL; //split the user input string into the command before and after the pipe
            if (exCmd2(parsedIn, parsedIn2) == 42) {
                i = 0;
                while (*(parsedIn + i) != NULL) {
                    free(*(parsedIn + i));
                    i++;
                }
                i = 0;
                while (*(parsedIn2 + i) != NULL) {
                    free(*(parsedIn2 + i));
                    i++;
                }
                free(userIn);
                free(parsedIn);
                //free(parsedIn2);
                exit(0);
            }
        }
        else if (exCmd1(parsedIn) == 42){
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
        free(parsedIn);
        if (parsedIn2 != NULL) {
            while (*(parsedIn2 + i) != NULL) {
                printf("%s\n", *(parsedIn2+i));
                free(*(parsedIn2 + i));
                i++;
            }
            //free(parsedIn2);
            parsedIn2 = NULL;
        }
    }

}


//get a line of input from the user. Store the line in the userIn parameter. Will automatically resize the userIn
//memory if input becomes too large
_Bool getLine (char* userIn){
    int bufferSize = BUF_SIZE;
    userIn = realloc(userIn, sizeof(char) * bufferSize);
    int c;
    int i = 0;
    while (1){
        c = getchar();
        if (c == '\n'){
            *(userIn + i) = '\0';
            return 0;
        }
        if (c == EOF){
            *(userIn + i) = '\0';
            return 1;
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
    return 0;
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


int lookForPipe (char** args){
    int i = 0;
    while(*(args + i) != NULL){
        if (strcmp(*(args+i), "|") == 0) {
            return (i); //if a pipe is found return it's offset
        }
        i++;
    }
    return (-1); //-1 will be the signal that a pipe was not found.
}


//***************************************
//Returns: -1 if no redirect found, 1 if redirect is found.
//searches args for a redirect symbol. The returns variable is used to point to the return values of the
//function. lookForRedir places the offset of the redirection symbol in args in the first index of the
//returns array. It also places 1 in the second index of returns to signify output redirection and -1 to
//signify input redirection.
int lookForRedir(char** args, int* returns){
    //******
    //to-do: handle the case of multiple file redirect symbols (need an index for each symbol)
    int i = 0;
    int foundRedir = -1;
    while (*(args + i) != NULL){
        if (strcmp(*(args+i), ">") == 0){
            *(returns + 3) = i; //puts the offset of the redirection symbol in the fourth spot of returns
            *(returns + 0) = 1; //puts 1 in the first spot of returns to signify output redirection
	        foundRedir = 1;
        }
        if (strcmp(*(args+i), "<") == 0){
            *(returns + 4) = i; //puts the offset of the redirection symbol in the fifth spot of returns
            *(returns + 1) = 1; //puts 1 in the second spot of returns to signify input redirection
            foundRedir = 1;
	    }
        if (strcmp(*(args+i), "2>") == 0){
            *(returns + 5) = i; //puts the offset of the redirection symbol in the sixth spot of returns
            *(returns + 2) = 1; //puts 1 in the third spot of returns to signify error redirection
            foundRedir = 1;
	    }
        i++;
    }
    return (foundRedir);
}


//Looks for the & symbol in args. Returns 1 if it is found and -1 if it is not. Sets args at the position of the & to be NULL
_Bool lookForBGProcess(char** args, _Bool removeSymbol){
    int i = 0;
    while (*(args + i) != NULL){
        if (strcmp(*(args + i), "&") == 0){
            if(removeSymbol){
                
                *(args + i) = NULL;
            }
            return 1;
        }
        i++;
    }
    return 0;
}


//executes the user command. First it checks to see if the command is one of the built in commands in the shell
int exCmd1(char** args){
    for (int i = 0; i < NUM_SHELL_CMDS; i++){
        if (strcmp(args[0], SHELL_CMDS[i]) == 0){
            return (*SHELL_FNCTS[i])(args);
        }
    }
    return lnchPrg1(args);
}


int exCmd2(char** args1, char** args2){
    for (int i = 0; i < NUM_SHELL_CMDS; i++){
        if (strcmp(args1[0], SHELL_CMDS[i]) == 0){
            return (*SHELL_FNCTS[i])(args1);
        }
    }
    for (int i = 0; i < NUM_SHELL_CMDS; i++){
        if (strcmp(args2[0], SHELL_CMDS[i]) == 0){
            return (*SHELL_FNCTS[i])(args2);
        }
    }
    return lnchPrg2(args1, args2);
}


void setUpRedir (char** args, _Bool output, _Bool input, _Bool error){
    int* redirOffsetAndType = malloc(sizeof(int) * 6);
    for (int i = 0; i < 6; i++){    //initialize array of data used for file redirection detection
        *(redirOffsetAndType + i) = -1;
    }

    if(lookForRedir(args, redirOffsetAndType) == 1){    //there is a redirect
        //printf ("filename : %s\n", args[redirOffsetAndType[3] + 1]);
        char* fileName;
        //printf("args[0]: %s\n", args[0]);
        
        if (output){
            if(redirOffsetAndType[0] == 1){
                fileName = *(args + redirOffsetAndType[3] + 1);
                *(args + redirOffsetAndType[3]) = NULL;
                int fd = open(fileName, O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU);
                //printf ("fd: %d\n", fd);
                dup2(fd, STDOUT_FILENO);
            }
        }

        if (input){
            if (redirOffsetAndType[1] == 1){
                fileName = *(args + redirOffsetAndType[4] + 1);
                *(args + redirOffsetAndType[4]) = NULL;
                int fd = open(fileName, O_RDONLY);
                //printf ("fd: %d\n", fd);
                dup2(fd, STDIN_FILENO);
            }
        }

        if (error){
            if(redirOffsetAndType[2] == 1){
                fileName = *(args + redirOffsetAndType[5] + 1);
                *(args + redirOffsetAndType[5]) = NULL;
                int fd = open(fileName, O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU);
                //printf ("fd: %d\n", fd);
                dup2(fd, STDERR_FILENO);
            }
        }

        free(redirOffsetAndType);
    }
    return;
}


//executes all user commands that require an external program
int lnchPrg1(char** args){
    pid_t pid;
    int* waitStat;
    _Bool BGFlag = lookForBGProcess(args, 0);
    pid = fork();
    if (!BGFlag)
        fgPid = pid;
    //printf("child pid: %d\n", pid);

    if (pid == 0){ //child
        setUpRedir(args, 1, 1, 1);
        if(BGFlag)
            lookForBGProcess(args,1);
        execvp(args[0], args);
        //printf("execvp error in lnchPrg1");
        return(-1);
    }

    else if (pid == -1){
        //printf("error forking from lnchPrg1");
        return(-2);
    }

    else { //parent
        setpgid(pid,pid);
        topPid = pid;
        printf("pid: %d", pid);
        if(BGFlag){//BG process
            ++numJobs;
            head = addNode(head, pid, numJobs, 1, args[0]);
            return (0);
        }
        else{//FG process
            ++numJobs;
            head = addNode(head, pid, numJobs, 1, args[0]);
            waitpid(pid, waitStat, 0);
            if (fgPid != -1)
                head = removeNodePid(head, pid);
            fgPid = -1;
            return (0);
        }
    }
}


int lnchPrg2(char** args1, char** args2){
    pid_t pid1, pid2;
    int pipeFd[2]; //pipeFd[0] read end pipeFd[1] write end
    pipe(pipeFd);
    int* waitStat;
     _Bool BGFlag = lookForBGProcess(args2, 0);
    pid1 = fork();
    if (pid1 != 0){ //parent
        pid2 = fork();
        if (!BGFlag)
            fgPid = pid2;
        if (pid2 != 0){//parent
            close(pipeFd[0]);
            close(pipeFd[1]);
            setpgid(pid1, pid1);
            setpgid(pid2, pid1);
            topPid = pid1;
            if(BGFlag){
                ++numJobs;
                head = addNode(head, pid1, numJobs, 1, args2[0]);
                return (0);
            }
            else{
                ++numJobs;
                head = addNode(head, pid1, numJobs, 1, args2[0]);
                waitpid(pid2, waitStat, 0);
                head = removeNodePid(head, pid1);
                fgPid = -1;
                return (0);
            }
        }
        else{ //child 2 read end of pipe
            if(BGFlag)
                lookForBGProcess(args2,1);
            close(pipeFd[1]);
            dup2(pipeFd[0], STDIN_FILENO);
            setUpRedir(args2, 1, 1, 1);
            execvp(args2[0], args2);
        }
    }
    else{ //child 1 write end of pipe
        close(pipeFd[0]);
        dup2(pipeFd[1], STDOUT_FILENO);
        setUpRedir(args1, 1, 1, 1); 
        execvp(args1[0], args1);
    }

}


int yashExit(char** args){
    freeLL(head);
    return 42;
}

int yashJobs(char** args){
    LLPrintJobs(head, numJobs);    
}