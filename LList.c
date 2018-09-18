#include "LList.h"
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>


struct node* createLList(){
    struct node* newNode = malloc(sizeof(struct node));
    newNode->next = NULL;
    newNode->name = NULL;
    return newNode;
}

struct node* addNode(struct node* head, struct node* new){
    if (head == NULL){
        head = new;
        printf("added node: %d\n", new->pid);
        return head;
    }
    struct node* cur = head;
    while (cur->next != NULL){
        cur = cur->next;
    }
    cur->next = new;
    new->next = NULL;
    printf("added node: %d, to node: %d\n", new->pid, cur->pid);
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
                kill (cur->pid, SIGINT);
                free(cur);
            }
            else{   //if there is only one node
                printf("removed node: %d\n", cur->pid);
                kill (cur->pid, SIGINT);
                free(cur);
                head = NULL;
            }
        }
        if (prev != NULL){//in the middle or at end
            printf("in middle\n");
            printf("removed node: %d\n", cur->pid);
            prev->next = cur->next;
            kill (cur->pid, SIGINT);
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
                kill (cur->pid, SIGINT);
                free(cur);
            }
            else{ //if there is only one node
                kill (cur->pid, SIGINT);
                free(cur);
                head = NULL;
            }
        }
        if (prev != NULL){//in the middle or at end
            prev->next = cur->next;
            kill (cur->pid, SIGINT);
            free(cur);
        }
    }
    return head;
}

void freeLL(struct node* head){
    printf("test\n");
    if (head == NULL){
        return;
    }
    struct node* cur = head;
    struct node* prev = NULL;
    while (cur != NULL){
        printf("removed node: %d\n", cur->pid);
        kill (cur->pid, SIGINT);
        prev = cur;
        cur = cur->next;
        free(prev);
    }
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
        printf("\n"); 
        cur = cur->next;
    }
    return;
}