# Lecture 18
## Dec 3, 2019
---
## Wrap-Up Threading: Caches + Threading and Atomic Operations

### Multi-core processors and Caches
- Small thread specific caches for individual cores and then a large shared cache
- Multiple copies of shared variables are fine! We can have data in shared cache and in thread cache too
- Read/write same variable can cause issues
  - Anytime a thread writes to its cache, other caches must update to account for cache value change
  - Invalidated other thread's cache value

### False sharing
- If we write two different variables but on the same cache line -- slow
  - Can mutually invalidated each other's cache values bc they change different values in the same cache line

### Using a semaphore mutex
- We use a global variable that holds the sum, this sum will be protected by a mutex
- One step further: we can store i sums into a global array, and then sum them at the end
  - If we store sum into a register value and then store to the array at the last instance, we get faster

### Advanced Synchronization
- Semaphores are slow (have to call into OS)
  - P() -> if s = 0, we have to deschedule
  - V() -> if s = 0, we schedule a new thread
  - Context switching takes time
- Atomic instructions are lower level primitive instructions that can update data without OS support
  - Read and update state "all at once"
  - No intervening b/w write and update (either everything happens or nothing does)

### Ex. Compare and Swap
- bool __ sync_ bool_ compare_ and_ swap (type* ptr, type oldval, type newval)
- If mem at ptr = oldval, set value at ptr to newval
- Returns true if the swap happens
- Can have mutex without OS support (better for short critical sections)
- This is a spin lock
```
int mutex = 1;
while (!__sync_bool_compare_and_swap(mutex, 1, 0)){
  //critical
  __sync_bool_compare_and_swap(mutex, 0, 1);
}
```

### Atomic add
- Automatically add a value to a memory location
- GCC has __ sync _ fetch_ and_ add(mem, value)

---
## Linking
### Making compiling code faster
- If we edit a few files out of all of them, we can reuse compilation results of remaining files and just update the ones that were edited
  - Basic idea = compile to intermediate (object files with .o suffix) -- separate compilation
    - Object files = assembly snippets that can be linked together later for a final binary
    - Assembly for one C file
  - When you change one C file, you only have to recompile the file that changed and then relink
  - Makefile is a build system to coordinate

### Static Linking
- gcc -c generates object file, then a final command can combine object files to an executable
- Generating object file
  - cpp: Preprocessing (ex. macros/text replacement), cc1: compiling, as: assembling

### Why linkers?
- Modularity and separate compilation
- Sharing libraries saves space

### What do linkers do?
- Symbol resolution: define and reference symbols
  - Stored in symbol table with name, size, and location
  - Associate each symbol reference with ONE definition
- Relocation: merge code and data into single section
  - Relocate symbols to absolute locations in executable and updates references to symbols

### Kinds of object files
- Relocatable object (.o): can be combined with other relocatable objects -- from ONE C source file
- Executable object (.out): code/data can be copied directly into memory for execution
- Shared object (.so): special type of relocatable object, can be loaded into memory dynamically at load/run time

### Executable and Linkable format (ELF)
- Standard binary format for object files
- Unified format for .o/.out/.so
- Generically called ELF binaries
- ELF Object file format:
  - a header
    - word size/byte ordering/file type/matchine type/etc.
  - segment header table
    - Tells system how to create virtual memory image for program -- map sections to VM
  - .text section (code)
  - .rodata (read only data like jump tables)
  - .data section (initialized global vars)
  - .bss
    - uninitialized global vars
    - "Block started by symbol" | "Better save space"
    - Has section header but occupies no space
  - .symbtab
    - Symbol table, procedure/static var names, Section names and locs
  - .rel.text
    - Relocation info for .text section
    - Addresses of instructions that will need to be modified in executable
  - .rel.data
    - Relocation info for .data section
    - Addresses of pointer data that will need to be modified in the merged executable
  - .debug  
    - Info for symbollic debugging (ex. gcc -c)
  - Section header table
    - Offsets and sizes of each section

### Linker symbols
- Global symbols
  - Defined by module *m* that can be referenced by other modules
  - ex. non-static C functions and non-static global vars
- External symbols  
  - Global symbols referenced by module *m* but defined by some other modules
  - Can be explicit with extern keyword before var name
- Local symbols
  - Symbols defined and referenced exclusively by module *m*
  - ex. C functions and global vars defined with the static attribute
  - NOT THE SAME AS LOCAL PROGRAM VARS

### Local Symbols
- Local non-static C variables v. local static C variables
  - local non-static variables: stored on the stack
  - local static C variables: stored in either .bss or .data

### How Linker resolves duplicate symbol definitions
- Rules that are part of C LANGUAGE
- Program symbols are either strong of weak
  - Strong: procedures and initialized globals
  - Weak: uninitialized globals
  - Strong takes precedence over weak
- RULE 1: Multiple strong symbols are not allowed
  - Each item can be defined only once
  - LINKER ERROR
- RULE 2: Given a strong symbol and multiple weak symbols, choose the strong symbol
  - References to the weak symbol resolve to the strong symbol
- RULE 3: Multiple weak symbols, pick an arbitrary one
  - Can override this within gcc
- This is why we need alignment rules!

### Global variables
- Avoid if you can
  - Use static/locals
- Otherwise, if we define a global var, use extern if we reference it

### Relocation
- relocatable object files --> executable object file
- We may not know how to refer to data/procedures that we have written globally/externally
  - Assembly needs some relocation entry slot that the linker will FILL IN with the locations in memory

### Loading
- execve invokes the loader and allocated virtual pages for .text and .data sections -- creates PTEs marked as invalid
- .text and .data sections are copied, page by page, on demand by the virtual memory system

### Packaging Commonly Used functions
- Package functions commonly used by programmers
  - libraries for functionalities
- Awkward given the linker framework so far
  - Option1: put all functions into ONE source file
    - programmers link big object file into their programs
    - space and time inefficient
  - Option2: put each function into separate source file
    - So many different files
- Static libraries (.a archive files):
  - concatenate related relocatable objects into one file with an index
  - Enhance linker so that it tries to resolve unresolved external references by looking for symbols in archives
  - If an archive member file resolves references, link it to executable
  - Disadvantages:
    - have to create a copy of all the code in final binary (not space efficient)
    - wastes resources if multiple programs use it
- Shared libraries:
  - Like an object file BUT relocates differently
  - Object files contain code and data shared across processes
  - Loaded and linked into an application at execution time
  - Usually happens by default by dynamic linker
  - Depend externally on library existing on whatever machine you are using -- tradeoff efficiency and portability

### Summary
- Know the principles/concepts of libraries
- Know resolution of symbols (strong/weak rules, etc. ) 
