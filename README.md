# CS143B-Project-Process-and-Resource-Management

## Overview
This project simulates a simplified process and resource manager with:
- Process Control Blocks (PCB)
- Resource Control Blocks (RCB)
- Multi-level ready queue scheduling
- Resource allocation with blocking

It supports a small command language to create, destroy, schedule, and manage processes and resources.

---

## Build Instructions
Compile all source files using GCC:

```bash
gcc main.c manager.c pcb.c rcb.c -o manager
```

---

## Run Instructions
Run the program and provide an input file:

```bash
./manager < input.txt
```

Example:

```bash
./manager < test4.txt
```

---

## Input Commands
Each line in the input file is a command.

### 1. init
Initializes the system and creates process 0.

```
in
```

Output: prints running process (0)

---

### 2. create process
Creates a new process with priority 1 or 2.

```
cr <priority>
```

Example:
```
cr 2
```

---

### 3. destroy process
Destroys a process and all its descendants.

```
de <pid>
```

Example:
```
de 3
```

---

### 4. request resource
Request k units of resource r.

```
rq <r> <k>
```

Example:
```
rq 2 1
```

If unavailable → process is blocked.

---

### 5. release resource
Release k units of resource r.

```
rl <r> <k>
```

Example:
```
rl 1 1
```

---

### 6. timeout
Preempts running process and schedules next.

```
to
```

---

## Output Format
The program prints **ONLY integers separated by spaces**:

- return value of each command
- -1 indicates error
- scheduler result is printed automatically

Example:
```
0 1 1 2 2 1
```

---

## Process States
| State | Meaning |
|------|--------|
| 0 | FREE |
| 1 | READY |
| 2 | RUNNING |
| 3 | BLOCKED |

---

## Resource System
There are 4 resources:
- R0 → 1 unit
- R1 → 1 unit
- R2 → 2 units
- R3 → 3 units

If a process requests more than available → it is blocked.

---

## Scheduling Policy
- Priority-based scheduling
- Higher priority runs first (priority 2 > 1 > 0)
- Round-robin within same priority using `timeout`

---

## File Structure
```
main.c        - command parser
manager.c     - scheduler + system init
pcb.c         - process creation/destruction
rcb.c         - resource management
manager.h     - shared structures + prototypes
```

---

## Common Issues

### 1. Multiple definition of DEBUG_MODE
- Ensure it is defined in ONLY ONE .c file or as extern

### 2. Wrong output (-1 unexpectedly)
- Usually caused by:
  - RL inconsistency
  - invalid scheduler state
  - blocked process still in ready queue

### 3. Build errors
Use clean build:
```bash
rm manager
make clean (if available)
```

---

## Notes
- Debug prints must be removed for grading
- Only numeric output is accepted
- Memory is dynamically allocated for queue nodes

---

## Author
CS143B Project Implementation

