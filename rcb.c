#include <stdlib.h>
#include "manager.h"

int holding_resource(int pid, int r, int k) {
    struct Node* res = pcb[pid].resources;
    int total = 0;
    while (res) {
        if (res->pid == r) {
            total += res->units;
        }
        res = res->next;
    }
    return total >= k;
}

int request(int r, int k) {
    if (r < 0 || r >= MAX_RCB || k <= 0)
        return -2;
    if (k > rcb[r].total_units)
        return -2;

    int current = running_pid;
    if (current <= 0 || pcb[current].state != RUNNING)
        return -2;

    // Enough units available: allocate and continue running
    if (rcb[r].state >= k) {
        rcb[r].state -= k;

        struct Node* new_res = new_node(r, k);
        new_res->next = pcb[current].resources;
        pcb[current].resources = new_res;

        return running_pid;
    }

    // Not enough units: block current process
    pcb[current].state = BLOCKED;

    // Remove from RL
    remove_from_rl(current);

    // Add to resource waitlist (FCFS)
    struct Node* wait_node = new_node(current, k);
    wait_node->next = NULL;

    if (rcb[r].waitlist == NULL) {
        rcb[r].waitlist = wait_node;
    } else {
        struct Node* tail = rcb[r].waitlist;
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = wait_node;
    }

    running_pid = -1;
    return scheduler();
}



int release(int r, int k) {
    if (r < 0 || r >= MAX_RCB || k <= 0)
        return -2;

    int current = running_pid;
    if (current < 0)
        return -2;

    // Must hold at least k units of resource r (total across nodes)
    if (!holding_resource(current, r, k))
        return -2;

    // Remove k units of r from the process's resource list
    int remaining = k;
    struct Node** res_ptr = &pcb[current].resources;

    while (*res_ptr && remaining > 0) {
        if ((*res_ptr)->pid == r) {
            if ((*res_ptr)->units > remaining) {
                // Reduce units in this node
                (*res_ptr)->units -= remaining;
                remaining = 0;
            } else {
                // Remove entire node and continue
                remaining -= (*res_ptr)->units;
                struct Node* to_free = *res_ptr;
                *res_ptr = (*res_ptr)->next;
                free(to_free);
                continue;  // res_ptr already updated
            }
        }
        res_ptr = &(*res_ptr)->next;
    }

    // Return units to the resource
    rcb[r].state += k;

    // Wake up waiting processes in FCFS order
    while (rcb[r].waitlist && rcb[r].state > 0) {
        struct Node* waiter = rcb[r].waitlist;

        if (rcb[r].state >= waiter->units) {
            rcb[r].state -= waiter->units;

            int pid = waiter->pid;
            int units = waiter->units;

            // Unblock process
            pcb[pid].state = READY;

            // Grant resource
            struct Node* new_res = new_node(r, units);
            new_res->next = pcb[pid].resources;
            pcb[pid].resources = new_res;

            // Remove from waitlist
            rcb[r].waitlist = waiter->next;
            free(waiter);

            // Add to ready list tail
            add_to_rl(pid, pcb[pid].priority);
        } else {
            break;
        }
    }

    // Reschedule
    return scheduler();
}

void add_to_rl(int pid, int priority) {
    struct Node* node = new_node(pid, 0);
    node->next = NULL;

    if (rl.levels[priority] == NULL) {
        rl.levels[priority] = node;
    } else {
        struct Node* tail = rl.levels[priority];
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = node;
    }
}
