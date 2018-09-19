#include "LList.h"
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>


struct node* createLList(){
    struct node* newNode = malloc(sizeof(struct node));
    newNode->next = NULL;
    newNode->name = NULL;
    return newNode;
}

struct node* addNode(struct node* head, int pid, int jobNum, enum status state, const char* name){
    struct node* newNode = malloc(sizeof(struct node));
    newNode->pid = pid;
    newNode->jobNum = jobNum;
    newNode->state = state;
    newNode->name = strdup(name);
    newNode->next = NULL;
    if (head == NULL){
        head = newNode;
        printf("added node: %d\n", newNode->pid);
        return head;
    }
    struct node* cur = head;
    while (cur->next != NULL){
        cur = cur->next;
    }
    cur->next = newNode;
    printf("added node: %d, to node: %d\n", newNode->pid, cur->pid);
    return head;
}

struct node* removeNodePid(struct node* head, int pid){
     if (head == NULL){
        return head;
    }
    printf("start remove\n");
    struct node* cur = head;
    struct node* prev = NULL;
    while (cur->next != NULL && cur->pid != pid){
        printf("node: %d\n", cur->pid);
        prev = cur;
        cur = cur->next;
    }
    if (cur->pid == pid){ //if the node with the desired pid was found
        printf("Node found\n");
        if (prev == NULL){ //at beginning
            printf("at beginning\n");
            if (cur->next != NULL){ //if there are more than 1 node
                head->next = cur->next;
                printf("removed node: %d\n", cur->pid);
                kill ((-1)*cur->pid, SIGINT);
                free (cur->name);
                free(cur);
            }
            else{   //if there is only one node
                printf("removed node: %d\n", cur->pid);
                kill ((-1)*cur->pid, SIGINT);
                free (cur->name);
                free(cur);
                head = NULL;
            }
        }
        else{//in the middle or at end
            printf("in middle\n");
            printf("removed node: %d\n", cur->pid);
            prev->next = cur->next;
            kill ((-1)*cur->pid, SIGINT);
            free (cur->name);
            free(cur);
        }
    }
    return head;
}

struct node* removeNodeJobNum(struct node* head, int jobNum){
     if (head == NULL){
        return head;
    }
    struct node* cur = head;
    struct node* prev = NULL;
    while (cur->next != NULL && cur->jobNum != jobNum){
        prev = cur;
        cur = cur->next;
    }
    if (cur->jobNum == jobNum){ //if the node with the desired jobNum was found
        if (prev == NULL){ //at beginning
            if (cur->next != NULL){ //if there are more than one node
                head->next = cur->next;
                kill ((-1)*cur->pid, SIGINT);
                free (cur->name);
                free(cur);
            }
            else{ //if there is only one node
                kill ((-1)*cur->pid, SIGINT);
                free (cur->name);
                free(cur);
                head = NULL;
            }
        }
        if (prev != NULL){//in the middle or at end
            prev->next = cur->next;
            kill ((-1)*cur->pid, SIGINT);
            free (cur->name);
            free(cur);
        }
    }
    return head;
}

void freeLL(struct node* head){
    if (head == NULL){
        return;
    }
    struct node* cur = head;
    struct node* prev = NULL;
    while (cur != NULL){
        printf("removed node: %d\n", cur->pid);
        kill ((-1)*cur->pid, SIGINT);
        prev = cur;
        cur = cur->next;
        free (prev->name);
        free(prev);
    }
    head = NULL;
    return;
}

void LLPrintJobs(struct node* head, int mostRecentJob){
    if (head == NULL){
        return;
    }
    struct node* cur = head;
    while (cur != NULL){
        printf("[%d] ", cur->jobNum);
        if (cur->jobNum == mostRecentJob){
            printf("+    ");
        }
        else{
            printf("-    ");
        }
        switch(cur->state){
            case 1 :
                printf("Running    ");
                break;
            case 2 :
                printf("Stopped    ");
                break;
            case 3 :
                printf("Done    ");
                break;
        }
        printf("%s\n", cur->name); 
        cur = cur->next;
    }
    return;
}