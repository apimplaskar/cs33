# Lecture 14
## Nov 14, 2019
---
### Explicit Free Lists
- maintain lists of free blocks (NOT ALL BLOCKS)
  - next free block could be anywhere, so we store forward/back pointers (not just size)
  - still need boundary tags (size at front and back so we can coalesce)

### Allocating from explicit free lists
- Iterate along every block so we can find one that satisfies our size constraints
- Find a big enough block, fill in our data, and split the remaining space off and add that to the explicit free list

### Freeing from explicit free list
- Insertion policy: where in free list do you put newly freed block?
  - LIFO (last in first out), insert freed block at beginning of free list
    - simple, constant time, but fragmentation worse
  - Address ordered
    - requires search, but better for fragmentation
- Easy case: when no coalescing (like pointer management in linked lists)
- Case with coalescing:
  - splice out predecessor/successor blocks -- coalesce memory blocks (by recording new size in header and footer), insert new block at root of list
- **Why not just record distance to nearest free block?**

### Explicit list summary
- Allocate is linear time in number of FREE blocks not ALL blocks -- faster
- a little more complicated for splicing and coalescing + more space for links

### Segregated free list
- Each size class of blocks has its own free list
- Separate classes for each small size
- For large sizes: use classes for each two-power size

### Blocks sorted by size
- Can use a binary/red-black tree with pointers within each free block
  - Use length as a key

### Summary
- A lot of methods to do this -- variable sized memory allocation is difficult

### Dynamic Memory allocation
- Explicit allocators: application allocates and frees spaces
  - malloc/free in C, new/delete in C++
- Implicit allocators: application allocates but does not free space
  - garbage collection in Java, ML, Lisp

### Implicit allocators: garbage collection
- Garbage collection: automatic reclamation of heap-allocated memory
- Think about memory as a graph
  - Ecah block is a node, each pointer is an edge
  - Locations not in the heap that contain pointers into the heap are ROOT NODES (registers, locations on stack, global vars)
  - Node is reachable if there is a path from a root to that node
  - Non-reachable nodes are garbage (not needed by application)

---
## Looking over the provided malloc source code
---
## Exceptional Control Flow: Exceptions and Processes
### Control flow
- CPU execution model (Von Neumann): sequential
  - Each instruction modifies program state (regs/memory)
  - Next instruction determined only by previous instruction and state of program

### Control Flow Mechanisms
- jumps, loops, etc.

### How to react to events
- Insufficient for a useful system
- Difficult to react to changes in system (EXTERNAL) state
  - Data from disk, network
  - ctrl-C at the keyboard
  - System timer expires
  - Division by 0, etc.
- Can we poll each state?
- Need a mechanism for exceptional control flow

### Exceptional control flow
- Exceptions:
  - Change in control flow in response to a system event (change in system state)
  - Implement using combination of OS and hardware
- What can you do?
  - Process content switch: implemented by OS software and hardware timer
  - Signals (messages from OS)

### Exceptions
- def: a transfer of control to the OS *kernel* in response to some *event* (i.e change in processor state)
  - Kernel = memory-resident part of OS (manage core functionality)
  - Ex: div by 0, arithmetic overflow, I/O request completes, ctrl-C, page fault

### Exception tables
- each type of event has a unique exception number k
- k = index into exception table (aka interrupt vector)
- Handler k is called each time exception k occurs

### Asynchronous Exceptions (interrupts)
- Caused by events external to processor
  - Indicated by setting processor's interrupt pin -- handler returns to next instruction

### Synchronous Exceptions
- Caused by events that occur as a result of executing a particular instruction
  - Traps:
    - intentional call into the OS (ex. system calls, breakpoint traps, special instructions, etc.)
    - Return control to next instruction
  - Fault:
    - unintentional but possibly recoverable
    - ex. page faults (REC), protection faults (UNREC), floating point exception
    - re-execute faulting (current) instruction OR abort
  - Abort:
    - Unintentional and unrecoverable
    - ex. illegal instruction
    - Aborts current program

### DISAMBIGUATION: Exceptions
- OS Exceptions:
  - as above
- Exception handling:
  - special cases in programming languages
- THESE ARE DIFFERENT!

### System calls
- each x86-64 system call has a unique ID
- Some ex.
  - | ID | call | Description|
| :------------- | :------------- |:----|
| 0       | read    | read file|
| 1 | write | write file |
etc.
- ex. Page fault:
  - User writes to memory location
  - if we don't have enough space, write a new page and write to that location
  - return
- ex. invalid mem reference
  - page fault --> detect invalid address --> signal process to abort
  - SIGSEGV (segmentation fault)

### Processes
- Def: process is an instance of a running program (not the same as PROGRAM or PROCESSOR)
- Process provides each program with 2 key abstractions
  - Logical control flow:
    - each program has exclusive control of CPU, provided by kernel's context switching
  - Private address space
    - Exclusive use of main memory, provided by kernel's virtual memory

### Multiprocessing on a single core
- Single processor can execute multiple processes concurrently
  - Process executions interleaved (multitasking)
  - Address spaces managed by virtual memory system
  - Register values for non-executing processes are saved in memory
- Save registers into memory --> schedule next process for execution --> load saved registers and switch address space (context switch)

### Concurrent Processes
- Each process is a logical control flow
- Two processes are *concurrent* if their flows overlap in time
- Otherwise, they are *sequential*

### Context switching
- Processes are managed by a shared chunk of memory-resident OS code called the *kernel*
  - KERNEL IS NOT A SEPARATE PROCESS, runs as part of some existing process
- Control flow passes from one process to the next via context switch

### Process Control
- pstree gives you process trees, you can also get individual process IDs

### Creating and terminating processes:
- process can be in one of three states
  - running: currently executing or waiting to be executed/will be scheduled by the kernel
  - stopped: execution is suspended and will not be scheduled until further notice, state is intact
  - terminated: stopped permanently

### Terminating processes
- Process terminates b/c of:
  - Receiving signal whose default action is to terminate
  - Return from main routine
  - call exit function
- ```
void exit(int status)
```
  - terminates with exit status of status
  - convention: 0 = everything is OK, nonzero for error
- exit is called once but NEVER returns

### Creating process
- Parent process can create a new running child process by calling fork
- ```
int fork(void)
```
  - returns 0 to the child process, child's PID to the parent
  - child is ALMOST identical to parent
    - different PID
    - same BUT SEPARATE copy of parent's virtual address space (and file descriptors)
- fork is called once but returns twice
  - concurrent execution
  - can't predict execution order of parent and child

### Process graph
- useful tool to capture partial ordering of statements in concurrent program
  - vertex = execution of statement
  - a -> b means a before b
  - Edges can be labeled with current value of variables
  - ANY TOPOLOGICAL ORDERING IS A VALID PROCESS

### Child processes
- Idea: when a process terminates, it still consumes system resources
  - We call such a terminated process a zombie
- Reaping
  - performed by parent on terminated child (call wait or waitpid)
  - Parent is given exit status info, and then kernel deletes zombie process
- If a parent doesn't reap:
  - if any parent terminates without reaping a child, orphaned child will be reaped by init process (pid==1)
  - So only need explicit reaping in long-running processes
