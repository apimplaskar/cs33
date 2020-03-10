# CS 33
## Machine Level Programming - Proedures
### Oct 10, 2019 -- Week 2, Thurs
---
### Review 1:
- movq %rdx, %rbx
  - Moves: quad = 8 bytes or 64 bits
  - %rdx might be a: long long int, long long unsigned int, double, pointer
- shrq %eax, %ebx
  - Logically shifts ebx right by eax (ebx = ebx >> eax)
  - ebx is likely an unsigned int (since we logically shift)

### Review 2:
- Why do we have registers?
  - Accessing larger storage structure takes longer and more energy

### Review 3:
- Function parameter: in register %edi
- Array: in memory (on the stack)
- Storing a value from indexed array: temp variable in the register (likely)
  - t = v[i];
- t += 5;
  - in %rax
- How do you set a particular register
  - can't, compiler does that
- Setting condition code:
  - use a comparison? can't do it directly
---
### Why learn assembly?
- C <--> Assembly is what compilers do
- Write faster programs
- Understand error sources
- Hacking: can reverse engineer code without source
- Debug low-level program errors

### Basic block
- single-entry/single-exit region of the program
- Control can only leave from bottom

### "how to hack"
- Break code into basic blocks and determine control flow
- Can reconstruct based on known registers
  - %edi is first function arg
  - %eax is return value
  - %edx has lower byte at %edl
- Assembly:
  - mov, jmp, je = equal to zero
- Use goto
- Clean it up

### Program structures
- Branches are likely conditionals (if/else/switch), reverse edges are likely loops

----
## Procedures
### API: Application Programming Interface
- calling other procedures/functions

### ABI Mechanisms in Procedures
- Passing control: to beginning of procedure code and back to return point
- Passing data: procedure args and return value
- Memory management: allocate during procedure execution, deallocate upon return

### How do we manage memory that is local to a function?
- One idea:
  - Give each function a special place in memory to put its temporary values
    - what if a function calls itself?
- Data-structure to keep track of which functions are active? STACK (LIFO)

### x86-64 Stack
- Region of memory managed with stack discipline
  - Grows towards lower addresses ("stack grows down")
  - We draw the bottom of the stack at the top of the page
- %rsp is the stack pointer (where we are in the stack)
  - lowest stack address (top element in the stack)
- pushq Src:
  - fetch operand at src register --> decrement %rsp by 8 --> write operand to address given by %rsp
- popq Dest:
  - read value at address given by %rsp
  - increment %rsp by 8
  - Store value at dest

### Procedure Control flow
- Procedure call: call label
  - Push return address on stack, jump to label
- Return address: address of next instruction right after call, example from disassembly
- Procedure return : ret
  - Pop address from stack, jump to address

### Passing data: Procedure Arguments and Return Value by ABI
- first 6 args: %rdi, %rsi, %rdx, %rcx, %r8, %r9
- return value: %rax
- Only allocate stack space when needed

### Stack based languages
- To support recursion
- Need to store args, local vars, return ptr of each call
- Stack discipline: state for procedure for limited time,
- Stack frame: single procedure instantiation

### Stack frames
- Contents: return info, local storage, temp space
- Management:
  - space allocated when we enter procedure (set up code, includes push by call)
  - deallocate when return (finish code, pop by ret)
- Current stack frame (top to bottom):
  - Argument build: parameters for function about to call
  - Local vars: if you can't keep in registers
  - Saved register context
- Caller stack frame:
  - return addresses
    - pushed by call instruction
  - arguments for call
- When the compiler allocates memory on the stack, it may allocate a little too much (multiples of 16?)

### Register Saving Conventions
- when procedure x calls y:
  - x is the caller, y is the callee
- Caller saved:
  - Caller's responsibility to save if the register is live (if its still needed)
- Callee saved:
  - Callee's responsibility
- BOTH depending on the register:
  - CALLER SAVED: %rax (return), %rdi ... %r9 (arg), %r10, %r11
  - CALLEE SAVED: %rbx, %rbp, %r12-%r15, %rsp
    - save the Bs! -- rbp, rbx are saved
