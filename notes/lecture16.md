# Lecture 16
## Nov 21, 2019
---
### Review
- Page table: array of page table entries (PTE) that maps virtual pages to physical pages
  - Pre-process kernel data structure in DRAM
  - Naive implementation is HUGE bc of how sparse the address space is
- Two-level page table hierarchy (store it like a tree)
- Page table is stored in the virtual memory space of every process (not accessible area by user's part of the code -- kernel memory)
  - Every process has its own section with its own page table
- Memory management unit:
  - TLB is a hardware cache that allows easy conversion from virtual to physical addresses
  - Otherwise, we have to look up twice (once in table, once in physical memory)
- Who manages the page table? **kernel/OS**
- Where is the TLB? **hardware/on-chip** -- need quick access for EVERY load and store we perform
- TLB is indexed by: **virtual memory address - specific bits**
- Instructions that cause TLB access: **loads/stores, branches**
- Can cause an exception: **Page miss**
- For allocating memory to programs (malloc) we have variable size chunks, BUT allocating physical to virtual memory has FIXED SIZE chunks
  - We can choose any page

### Page Access example in slides
- Note: virtual and physical page offsets are the same

### VM as a tool for memory protection
- PTE can have permission bits that MMU can examine

### VM as a tool for memory management
- each process has its own virtual address space -- can use this to maximize locality

### Sharing (COWs)
- Forked processes can share physical memory despite different virtual memory (just need a copy of the page table)
- Mapping on a private copy-on-write (COW) object
  - Can't write over the OTHER process' physical memory
- Instruction writing to a COW page can trigger a protection fault
  - Handler will create a new R/W page -- instruction restarts
  - We defer copying as long as possible

---
## Threading and basic synchronization
- Parallelizing operations across cores -- MAKE USE OF FORK AND WAIT
- Everytime we process something, we should fork and then update with child and then exit
  - Be wary of making copies vs. using shared data
  - Need some abstraction for this

### Threads
- Process = process context + code + data + stack
  - Alternate view: thread + code, data, kernel context
  - Separate logical flow of execution from the rest of the data
- Multiple threads can be associated with a process
  - Has its own stack (not necessarily protected from other threads) and registers, but can share the rest of the data
  - Distinguishable by thread ID
  - Share code, data, kernel context

### Concurrent Threads
- Threads are concurrent if their flows overlap in time, otherwise sequential
- Single core processor: time slicing can simulate parallelism
- Multi-core processor: can have true parallelism

### Threads vs. processes
- Similar: logical control flow, run concurrently with others (possibly on diff cores), context switched
- Different:
  - Threads share all code/data (except local stacks), processes do not
  - Threads are less expensive
    - Process control is more expensive b/c of page table management (memory copying, etc.)

### Posix threads (Pthreads) Interface
- Pthread: standard interface for ~60 functions that manipulates threads from C programs
  - Create/reap: pthread_create(), pthread_join()
  - Thread id: pthread_self()
  - Terminate: pthread_cancel(tid), pthread_exit(), exit() for all threads, RET for current thread
  - Synchronize access to shared variables: pthread_mutex_init, pthread_mutex_[un]lock

### Problem/Benefit of Threads: Sharing
- Easy to share data structures b/w threads
- Must be careful to avoid unintended sharing
  - ex. passing pointer to main thread's stack

### Share Variables in Threaded C programs
- Question: which variables in a threaded C program are shared? (not as simple as just saying global shared, stack private)
- Def: A variable X is shared iff multiple threads reference some instance of X

### Mapping Variable Instances to Memory
- Global variables: declared outside of a function
  - Virtual memory contains EXACTLY ONE instance of any global var
- Local variables: declared inside a function, without static attribute
  - Each tread stack contains one instance of each local variable
- Local static variables: declared inside function, with static attribute
  - Virtual memory contains exactly one instance of any local static variable

### Concurrent execution
- Key idea: in general, sequentially consistent interleaving is possible, but you can get unexpected results
- Process graphs depict execution state space
  - Each axis corresponds to sequential order of instructions in a thread, each point is an execution state

### Process graphs:
- Critical sections can depict "unsafe" regions, want our process graph to avoid these regions
  - Critical region: want a set of instructions to all happen at once
  - Use synchronization

### Semaphores
- Non-negative global integer synchronization variable manipulated by P and V operations
- P(s) - sem_wait in pthreads
  - If s is nonzero, decrement s by 1 and return immediately
    - Test and dec operations occur automatically
  - If s == 0, suspend thread until s becomes nonzero and thread is restarted by a V operation
  - After restarting, P operation decrements s and returns control to caller
- V(s) - sem_post in pthreads
  - Increment s by 1 (automatic)
  - If any threads blocked in a P operation waiting for s to become nonzero - restart exactly one of those threads, which then completes p
- Semaphore invariant (s >= 0)

### How Mutexes work
- Provide mutually exclusive access to shared variable by surrounding critical sections with P and V operations on a semaphore S (initially set to 1)
- Create a forbidden region that encloses unsafe region/cannot be entered by any trajectory on the process graph
