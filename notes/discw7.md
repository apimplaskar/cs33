# Week 7 Discussion
---
### Memory management
- CPU -> MMU (memory management unit) -> memory
  - CPU gets virtual address to MMU -> MMU has physical address to memory
- Virtual memory: array of N contiguous bytes stored on disk
- Contents of array on disk are cached in physical memory (DRAM cache)
  - cache block called a page

### Fragmentation
- Internal Fragmentation
  - If payload smaller than block size, caused by padding/alignment issues
- External Fragmentation
  - enough aggregate heap memory, but not enough in a single free block

### Need to know how much memory is free/to free
- Keep a word at the beginning of a block that keeps track of size of the block

### Structures
- Implicit list: Use length to link ALL blocks
- Explicit list: list of free blocks
- Segregated free list: separate blocks by payload size
- Sorted

### Implicit list:
- For each block, we need size and allocation status
  - Can store these in one header
  - Last bit of word = allocation status
     - 1: allocated, 0: free
- Size/alloc word - payload - padding
- Finding free block:
  - First fit: search list from beginning and choose first free block that fits (Linear in total number of blocks)
  - Next fit: keep track of where previous search ended, continue from there
  - Best fit: choose best block for each payload (minimize leftover bytes), reduces fragmentation but is slower (has to scan ALL blocks once first, and then stores)
- Coalescing:
  - Can join two free blocks together
- Bidirectional tagging:
  - Can keep a header and a footer boundary tag that stores size and allocation status (can have a list that traverses both forwards and backwards)

### Explicit free list:
- Physically blocks can be in any order, we just link free blocks together -- pointer adjustment

### Segregated free list:
- group lists based on block size

### Implicit v. explicit allocation
- Explicit: we have to allocate and free memory ourselves
- Implicit: garbage collector can free allocated memory that we didn't free ourselves
---
### Control flow:
- Processors do only one thing at a time
- Can alter control flow:
  - in program: jump/switch/call/return
    - Changes to program state
  - What if we have changes to a system state?
    - External action (data arrives, keyboard)
  - System needs mechanisms for exceptional control flow

### Exception:
- Transfer of control to OS kernel in response to some event
  - passes control to kernel that decides which exception it is and whether or not we can continue
- Asynchronous exceptions/interrupts:
  - Set processor's interrupt pin
- Synchronous exceptions
  - Trap: intentional (ex. breakpoint/special instructions), code handles these exceptions -- kernel will go to next instruction
  - Fault: unintentional but recoverable (maybe), re-execute current instruction or abort
  - Aborts: unintentional and unrecoverable, (ex. illegal instruction or illegal memory access), kernel aborts program

### Context switching:
- OS allows for swapping between processes and can maintain address spaces of each of these processes

### Terminating processes:
- Can terminate using exit function
- Exit status used as a flag for whether or not behavior was as expected

### Creating processes
- Fork method creates a child process
  - Returns 0 to the child process and returns process ID of child to the parent process
  - Called once and returns twice

### Reaping child processes
- When a program terminates, it may still use system resources -- zombie process
- Reaping: performed by parent on terminated child (wait/waitpid), parent gets exit status information of child and kernel then deletes zombie child if exit status is 0
- If parent doesn't reap: after parent terminates, orphaned child will be reaped by init process (pid == 1)
- Only explicitly reap in long-running processes
