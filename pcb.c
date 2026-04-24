#include <stdlib.h>
#include <string.h>
#include "manager.h"

int find_free_pcb() {
    for (int i = 1; i < MAX_PCB; i++) {
        if (pcb[i].state == 0) return i;
    }
    return -1;
}

int create(int priority) {
    if (priority < 1 || priority > 2)
        return -2;

    int parent = running_pid;
    if (parent < 0)
        return -2;

    int new_pid = find_free_pcb();
    if (new_pid < 0)
        return -2;

    // Initialize PCB
    memset(&pcb[new_pid], 0, sizeof(struct PCB));
    pcb[new_pid].state = READY;
    pcb[new_pid].priority = priority;
    pcb[new_pid].parent = parent;

    // Add to parent's children list
    struct Node* child = new_node(new_pid, 0);
    child->next = pcb[parent].children;
    pcb[parent].children = child;

    // Insert new process at TAIL of its priority queue
    add_to_rl(new_pid, priority);

    // Preemption rule:
    // If new process has higher priority than current running process,
    // the current running process must become READY and move to TAIL.
    if (priority > pcb[parent].priority) {

        // Move running process from head to tail
        int pri = pcb[parent].priority;
        struct Node* head = rl.levels[pri];

        if (head && head->pid == parent) {
            rl.levels[pri] = head->next;
            head->next = NULL;

            struct Node* tail = rl.levels[pri];
            if (tail == NULL) {
                rl.levels[pri] = head;
            } else {
                while (tail->next) tail = tail->next;
                tail->next = head;
            }
        }

        pcb[parent].state = READY;
        running_pid = -1;

        return scheduler();
    }

    return running_pid;
}


int is_descendant(int ancestor, int target) {
    if (ancestor == target) return 1;
    struct Node* child = pcb[ancestor].children;
    while (child) {
        if (is_descendant(child->pid, target)) return 1;
        child = child->next;
    }
    return 0;
}

void remove_from_rl(int pid) {
    int pri = pcb[pid].priority;

    struct Node* head = rl.levels[pri];
    struct Node* prev = NULL;
    struct Node* curr = head;

    while (curr) {
        if (curr->pid == pid) {
            if (prev == NULL) {
                rl.levels[pri] = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}


int destroy(int target_pid) {
    int current = running_pid;

    if (target_pid < 0 || target_pid >= MAX_PCB || pcb[target_pid].state == 0) {
        return -2;
    }

    // Must be descendant (or self)
    if (!is_descendant(current, target_pid)) return -2;

    // Recursively destroy children first
    struct Node* child = pcb[target_pid].children;
    while (child) {
        int child_pid = child->pid;
        struct Node* next_sibling = child->next;
        destroy(child_pid);
        child = next_sibling;
    }

    // Release all resources held by target
    struct Node* res = pcb[target_pid].resources;
    while (res) {
        int r = res->pid;
        int k = res->units;

        rcb[r].state += k;

        // Wake up waiting processes for this resource
        while (rcb[r].waitlist && rcb[r].state > 0) {
            struct Node* waiter = rcb[r].waitlist;

            if (rcb[r].state >= waiter->units) {
                rcb[r].state -= waiter->units;

                int pid = waiter->pid;
                int units = waiter->units;

                pcb[pid].state = READY;

                struct Node* new_res = new_node(r, units);
                new_res->next = pcb[pid].resources;
                pcb[pid].resources = new_res;

                rcb[r].waitlist = waiter->next;
                free(waiter);

                add_to_rl(pid, pcb[pid].priority);
            } else {
                break;
            }
        }

        res = res->next;
    }

    // Remove target from all waitlists
    for (int r = 0; r < MAX_RCB; r++) {
        struct Node** curr = &rcb[r].waitlist;
        while (*curr) {
            if ((*curr)->pid == target_pid) {
                struct Node* temp = *curr;
                *curr = (*curr)->next;
                free(temp);
                break;
            } else {
                curr = &(*curr)->next;
            }
        }
    }

    // Remove from ready list (if present)
    remove_from_rl(target_pid);

    // Free children/resources lists
    free_list(pcb[target_pid].children);
    free_list(pcb[target_pid].resources);

    // Clear PCB
    memset(&pcb[target_pid], 0, sizeof(struct PCB));

    // If we just destroyed the running process, running_pid is now invalid
    if (target_pid == current) {
        running_pid = -1;
    }

    // Reschedule according to new RL state
    return scheduler();
}