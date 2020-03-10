# Lecture 15
## Nov 19, 2019
---
### Processes
- Process: an instance of a program that is running
- Two key abstractions: logical control flow (seems like it has sole access to CPU, switched by kernel's context switching), has own private address space (provided by kernel -- virtual memory)

### Fork
- Creates new processes (children from parents)
- Takes copy of abstractions of parent process (logical control flow and address space), the only difference is the return from the fork fn (returns 0 to child, returns child's pid (nonzero) to the parent)
- Easily depicted as graphs

### Zombies
- A process that hasn't been cleaned up yet (after it has died)
- Initial process can automatically clean up children (init process)
- Reaping
- Child thread can continue executing even if parent thread terminates
- Counting zombies: all threads - (branches that exit + child processes that can keep running) -- there may be intricacies, this is a very barebones idea

### Wait
- for synchronizing with children, wait function will return to parent AFTER the child process calls exit
- waitpid for waiting for a particular process

### execve: Loading and running programs
- load and run in the current process
  - executable file = filename (object or script file)
  - argument list argv
  - environmental variables list envp
- int execve(char* filname, char* argv[], char* envp[])
- overwrites code, data, and stack
  - retains PID, open files, signal context
- Called once and never returns (Just becomes the new program)
  - Unless there is an error

### Signals
- Signal = small message that notifies process than an event of some kind has occurred in the system
  - Exceptions are used to implement signals
  - Sent from kernel (sometimes at request of another process) to a process
  - Signal type is identified by small integer ID
  - Only info in a signal is the ID and the fact that it has arrived

### Sending a signal
- Kernel sends signal to a destination process by updating some state in context of destination process
- Why?
  - Detection of some event
  - Another process has sent a signal
    - ex. kill system call to explicitly requests kernel to send a kill signal to the destination process

### Receiving a signal
- Destination process receives signal when forced by kernel to react in some way to the delivery of the signal
- Can ignore signal, terminate the process, or catch the signal using a signal handler

### Default actions and overriding
- Each signal has some default action
- We can modify this default action with a separate handler-
- handler_t* signal(int signum, handler_t* handler)
- INSTALL handler so we can CATCH signal

### Signal Handlers as concurrent flow
- Separate logical flow (NOT PROCESS) that runs concurrently with the main program

### Why is this important?
- Helps us take advantage of many processors
- Can use these to build a shell -- just a program that starts other programs

----
## Virtual Memory
- Large linear address space, independent of physical memory, unique to each process

### Managing Virtual Memory
- Software perspective:
  - Stack: alloc/dealloc with function calls
  - Heap: malloc library for allocation
  - Global data: allocate when program is loaded
- Hardware perspective:
  - How do we map virtual memory onto physical memory? Allocation problem

### Simple: What if we don't use virtual memory? Can we get away with physical addressing?
- All processor addresses are in terms of physical addresses
- This simple system is in microcontrollers/appliances (ex. microwaves/toasters/IoT sensors/simple parts of car)
- Benefit is simplicity but a lot of drawbacks
  - Without separate address space, any process can overwrite any other process' data
  - How much memory goes to each process?

### Using Virtual Memory
- Approach: every process gets its own full address space
- Simplify memory management in programming (uniform address space)
- Use main memory efficiently: DRAM as cache for parts of virtual address space
- Can spill over onto disk
- BETTER

### Supporting Virtual Memory
- Map regions of virtual address space data onto the physical memory
  - Address translation occurs as the middleman step
- We add an extra level in between CPU and physical memory
- Virtual address goes through MMU (memory management unit) to get translated to a physical address

### Basic strategies
- Paging:
  - Logically divide memory into chunks called pages
  - Make decision of where to put memory on Page Granularity
  - typically 4 KB
- Flexible translation:
  - Any virtual page (VP) can be mapped to any physical page (PP)
  - Fully associative
  - Requires a large mapping function, Too complicated and open-ended for a hardware implementation
- **DATA STRUCTURE**: Page table
  - Array of page-table entries (PTEs) that maps virtual pages to physical pages
  - Pre-process kernel data structure in DRAM

### Page hit/fault:
- HIT: reference to VM word that is in physical memory (DRAM cache hit)
- FAULT: reference to VM word that is not in physical memory (DRAM cache miss) -- might map to somewhere on the disk
  - Need to load that data into memory
  - Page fault is an exception -- handler can select a victim to be evicted -- restart offending instruction to get page hit
  - KEY POINT: wait until the miss to copy the page to DRAM = demand paging

### Address Translation with Page Table:
- Processor sends virtual address to MMU --> MMU fetches PTE from page table in memory --> MMU sends physical address to cache/memory --> Cache/memory sends data word to processor

### Multi-level page tables (ex. of 2 level)
- Level 1: each PTE points to a page table (always a memory resident)
- Level 2: each PTE points to a page (paged in and out like any other data)
- Helps minimize memory overhead
- What about a k-level page table?
  - K memory accesses (loads/stores)
  - We need another cache! Translation lookaside buffer (TLB)

### TLB
- Small set-associative hardware cache in MMU
- Mapping between virtual and physical page numbers
- Contains complete page table entries for small number of pages
- TLB Hit:
  - eliminates a memory access
- TLB Miss:
  - Incur an additional memory access (PTE), but this is rare as a result of locality
- Accessing the TLB: MMU uses VPN portion of virtual address

### Addressing for memory system
- Hvae a virtual address number and have offset bits
  - OFFSETS ARE THE SAME FOR VP AND PP
  
