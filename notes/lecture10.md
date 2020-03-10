# Lecture 10: Optimizing Performance across Hardware/Software
## October 31, 2019
---
- More to performance than asymptotic complexity
  - can optimize at multiple levels
- Useful to understand: how compilers work and how the hardware/microarchitectures work (parallelism?)

### Background: clock cycles
- synchronous logic
  - changes in state synchronized by clock
  - easiest way to design chips
  - vast majority of chips/circuits use this
- Clock cycle: time between two adjacent clock edges (of same type)
  - Frequency (cycles/sec) = inverse cycle length
  - 1ns Cycle = 10^9 cycles/sec = 1GHz clock
  - *Smallest unit of time for a task*
- Tradeoff: smaller cycle = higher frequency = faster circuit (can't be too fast though -- all logic takes some time)

### Iron Law of Performance
- Time/program = (Instructions/program) * (Cycles/instruction) * (Time/cycle)
  - Instructions/program: runtime count of instructions
  - Cycles/instruction
  - Seconds/cycle: length of each cycle (clock period) or use inverse metric of cycles/second

### Computer system layers:
- Instructions/program: Compiler/OS/Application
- Cycles/instruction: ISA, microarchitecture
- Time/cycle: microarchitecture, digital circuits, gates/transistors
- How do we improve instructions/program and cycles/instruction?

### Compiler optimizations:
- Provision of efficient mapping of program to machine:
  - register allocation, code selection and ordering (scheduling), dead code elimination, eliminate minor inefficiencies
- Don't usually affect asymptotic complexity (programmer's job)
- Have difficulty overcoming optimization blockers; memory aliasing/procedure side effects
- LIMITATIONS:
  - must not cause change in program behavior (unless making  use of nonstandard language features) -- can prevent optimizations
  - most analysis performed only within procedures (too expensive to have whole program analysis)
    - new versions of GCC can do it program wide within the same file though
  - limited to static information (doesn't know about inputs)

### Generally useful optimizations:
- Code motion:
  - Move code to reduce frequency of execution
    - Most common: move code outside of a loop
  - Compiler generated code motion (-O1)
- Reduction in strength:
  - Reduce costly operations with simpler one:
    - ex. shift instead of multiply/divide
  - Sequence of products -> incremental addition?
- Share common subexpressions: (-O1)
  - Reusing portions of expressions

### Optimization blocker #1: Procedure calls
- convert string to lower case
- THIS IS SLOW
```
void lower(char *s){
  size_t i;
  for (i = 0; i < strlen(s); i++){
    if(s[i] >= 'A' && s[i] <= 'Z'){
      s[i] -= ('A'-'a');
    }
  }
}
```
- Convert loop to goto form:
```
void lower(char *s){
  size_t i;
  if (i>= strlen(s)){ goto done; }
  loop:
    if(s[i] >= 'A' && s[i] <= 'Z'){
      s[i] -= ('A'-'a');
    i++;
  if(i < strlen(s)){ goto loop; }
  done:
}
```
- Notice: we execute strlen (loops every char until it hits a null byte) every iteration -- order N^2
  - Compute strlen once outside of the loop and store it in some variable
- Notice: also the 'A'-'a' computation is redundant
- Compiler treats procedure call as a black box
  - Remedy: GCC does it with -O1 within a single file, can do it ourselves

### Dealing with memory:
- Ex: Advanced code motion:
  - incrementing memory
  - ```{c}
    double *b = ...;
    *b = 0;
    for (j = 0; j < n; j++){
      *b += ...;
    }
    ```
  - Why not just accumulate sum and add it to memory at the end?
  - ```{c}
    double temp = 0;
    for (j = 0; j < n; j++){
      temp += ...;
    }
    *b += temp;
    ```
- This holds for row sums example
- Memory aliasing:
  - If we have code that updates a particular location in memory on every iteration

### Approach 2: tell compiler to optimize assuming no aliasing
- use the flag __ restrict

### Steps of executing an instruction
- Fetch: grab instruction from memory
- Decode: parse/interpret instruction
- Dispatch: read operands from reg. file
- Excute: perform computation
- Memory: access memory
- Writeback: write results to reg. file

### Basic processor design:
- Unit: instruction fetch --> address to memory
- Memory --> instruction decode
- Instruction decode --> dispatch <-- register file + condition codes
- Dispatch --> Functional units for computation execution
- Access memory and writeback result of computation

### Dynamic instruction view:
- Assume each instruction takes equal time (not really true, but a simplification)

### Three fundamental optimizations:
- Pipelining: multiple instructions in the pipeline at the same time -> increase frequency and throughput of processor
- Superscalar: execute more than one instruction (per stage) at a time, exploits **instruction level parallelism**
- Out of order execution: reorder instructions to execute past long-latency instructions, out of program order

### Pipelining:
- Think about it with the Subway analogy:
  - people always busy, but passing things along as they go
- Multiple instructions in hardware at same time (frequency increases)

### Superscalar/parallelism:
- Can I do multiple instructions at a time? Every cycle, we cut down an instruction
- Control dependence: have to execute a branch to know what instruction to execute
  - Every time there is a conditional branch, we have one of these
- We want to rearrange instructions and can run multiple instructions concurrently so long as there are no dependences

### Out-of-order execution
- Reorder buffer dynamically reorders instructions (subject to dependence) such that we can maximize instruction bandwidth
- Figure out direction of branches prior to execution

### Model of an Out-of-order core so far:
- Operates at a fixed frequency (unit of time = cycles)
- Core reorders instructions:
  - Preserve data dependences but breaks control dependences
  - Max amount of instruction parallelism
- Other assumptions:
  - All computations take 1 cycle each
  - Memory ops take 1 cycle each
- We can redefine the amount of parallelism and time per computation going forward

### Case study: vector operations
- See slides for basic optimizations
- We see that the speed of operations depend on the data types that are being operated on

### Improving our model of hardware:
- Operation of execution units
- Pipelining: divide computation into stages and pass partial computations from stage to stage
- Pipelined functional units:
  - Stage i can start new computation once values passed to stage i+1
  - If each multiplication takes 3 cycles -- we can do two in 7 (example in slides (54), this doesn't make sense without the context)
- Parameters for functional units:
  - Latency (number of stages), Cycles/issue (# of cycles between when you can start one operation and start the next operation -- throughput)
  - If a functional unit is perfectly pipelined: has cycles/issue of 1
