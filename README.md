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
Run the program and provide a text file as input to have the output inserted into a seperate text file:

```bash
./manager < input.txt > output.txt
```

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
Creates a new process with priority level <p>;.

```
cr <p>
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
Request <k> units of resource <r>.

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
Release <k> units of resource <r>.

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

## File Structure
```
main.c        - command parser
manager.c     - scheduler + system init
pcb.c         - process creation/destruction
rcb.c         - resource management
manager.h     - shared structures + prototypes
```

---

## Author
Cecilia Rodriguez

