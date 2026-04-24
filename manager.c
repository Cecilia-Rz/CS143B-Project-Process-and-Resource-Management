#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manager.h"

struct PCB pcb[MAX_PCB];
struct RCB rcb[MAX_RCB];
struct ReadyList rl;
int running_pid = -1;

struct Node* new_node(int val1, int val2) {
    struct Node* n = malloc(sizeof(struct Node));
    if (!n) return NULL;
    n->pid = val1;
    n->units = val2;
    n->next = NULL;
    return n;
}

void free_list(struct Node* head) {
    struct Node* curr = head;
    while (curr) {
        struct Node* next = curr->next;
        free(curr);
        curr = next;
    }
}

void init() {
    // Clear all data structures
    memset(pcb, 0, sizeof(pcb));
    memset(rcb, 0, sizeof(rcb));
    memset(&rl, 0, sizeof(rl));
    running_pid = -1;
    
    // Initialize RCBs with correct units
    rcb[0].state = rcb[0].total_units = 1;
    rcb[1].state = rcb[1].total_units = 1;
    rcb[2].state = rcb[2].total_units = 2;
    rcb[3].state = rcb[3].total_units = 3;
    
    // Initialize process 0
    pcb[0].state = RUNNING;
    pcb[0].priority = 0;
    pcb[0].parent = -1;
    pcb[0].children = NULL;
    pcb[0].resources = NULL;
    
    // Add process 0 to RL level 0
    rl.levels[0] = new_node(0, 0);
    running_pid = 0;
    
}

int scheduler() {
    // Find highest-priority non-empty ready list
    for (int pri = PRIORITY_LEVELS - 1; pri >= 0; pri--) {
        struct Node* head = rl.levels[pri];
        if (head != NULL) {
            int pid = head->pid;
            running_pid = pid;
            pcb[pid].state = RUNNING;
            return pid;
        }
    }

    // No runnable process
    running_pid = -1;
    return -1;
}

int timeout() {
    if (running_pid < 0) return -2;

    int pid = running_pid;
    int pri = pcb[pid].priority;

    struct Node* head = rl.levels[pri];
    if (head == NULL || head->pid != pid) {
        // Invariant broken; no valid rotation possible
        return -1;
    }

    // If only one process at this priority, nothing changes
    if (head->next == NULL) {
        return running_pid;
    }

    // Move head to tail
    rl.levels[pri] = head->next;
    head->next = NULL;

    struct Node* tail = rl.levels[pri];
    while (tail->next != NULL) {
        tail = tail->next;
    }
    tail->next = head;

    // Old running process becomes READY
    pcb[pid].state = READY;

    // Pick new head of highest-priority non-empty list
    return scheduler();
}