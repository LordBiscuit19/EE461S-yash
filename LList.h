#ifndef LList_H
#define LList_H

enum status {Running, Stopped, Done};

typedef struct node {
    int pid;
    int jobNum;
    enum status state;
    char* name;
    struct node* next;
} node;

struct node* addNode(struct node* head, int pid, int jobNum, enum status state, const char* name);
struct node* removeNodePid(struct node* head, int pid);
struct node* removeNodeJobNum(struct node* head, int jobNum);
void freeLL(struct node* head);
struct node* createLList();
void LLPrintJobs(struct node* head, int mostRecentJob);

#endif