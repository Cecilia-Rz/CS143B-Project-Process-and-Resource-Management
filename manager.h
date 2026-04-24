#ifndef MANAGER_H
#define MANAGER_H

#define FREE    0
#define READY   1
#define RUNNING 2
#define BLOCKED 3

#define MAX_PCB 16
#define MAX_RCB 4
#define PRIORITY_LEVELS 3

struct Node {
    int pid;
    int units;
    struct Node* next;
};

struct PCB {
    int state;
    int priority;
    int parent;
    struct Node* children;
    struct Node* resources;
};

struct RCB {
    int state;
    int total_units;
    struct Node* waitlist;
};

struct ReadyList {
    struct Node* levels[PRIORITY_LEVELS];
};

extern struct PCB pcb[MAX_PCB];
extern struct RCB rcb[MAX_RCB];
extern struct ReadyList rl;
extern int running_pid;

void init(void);
int create(int priority);
int destroy(int pid);
int request(int r, int k);
int release(int r, int k);
int timeout(void);
int scheduler(void);

struct Node* new_node(int val1, int val2);
void free_list(struct Node* head);
int find_free_pcb(void);
void remove_from_rl(int pid);
void add_to_rl(int pid, int priority);
int is_descendant(int parent, int target);
int holding_resource(int pid, int r, int k);

#endif