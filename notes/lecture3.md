# CS 33: Lecture 3
## October 3, 2019
## Machine-Level Programming I: Basics
---
### Review 1:
- When type casting integers, do I favor types with:
  - smaller or bigger size? bigger
  - signed or unsigned? unsigned
- What happens when we overflow when adding two unsigned numbers:
  - It's 2<sup>w</sup> too low (lose ability to represent that last 2<sup>w</sup>)
- Endianness? x86 is LITTLE ENDIAN
  - First digit in memory has the least significant bit value
  - By contrast, humans are BIG ENDIAN

### Review 2:
- How many instances of address space are there on any given computer?
  - One per program
- What defines size of address space?
  - "Word size" of the ISA (x86-64 is 64 bits)
    - i.e size of a pointer is 64 bits
- 0x30 0x31 0x32 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  - string "012"
    - 0x30 in ASCII is 0
  - for an int, we look at 4 bytes: 0x30 0x31 0x32 0x00
    - 0x 00 32 31 30 (little endian) in hex
  - for an short, we look at 2 bytes 0x30 0x31
    - 0x 31 30 in hex
---
### ISAs:
- Allow us to use the same software across different instances of hardware
- Examples: x86, etc.

### Definitions:
- **Architecture:** A specification for how software interacts with hardware
- **Instruction Set Architecture (ISA):** Specific language understood by hardware
  - ex. types of operations, registers
- **Microarchitecture:** Implemenation of Architecture
  - ex. cache sizes, core frequency
- **Compiled Code Forms:**
  - *Machine code:* Byte level representation of programs that a processor executes
  - *Assembly code:* Text representation of machine code

### What should be in an ISA:
- Be able to run any program in any language

### What should be in an architecture:
- Memory: a place to put values (place to put values): address space
- Instructions: move from one state to another
- Program: set of instructions (stored in memory)

MEMORY at time n --> INSTRUCTION --> MEMORY at time n+1

- Execution model: when do we execute each instruction?
  - Von Neumann: executed sequentially unless otherwise directed
  - Need an instruction pointer to point at the current instruction to be executed

### Computer Organization Viewpoint:
- CPU (Central Processing Unit): executes instructions
  - Programmer-Visible state:
    - PC: Program counter (aka instruction pointer)
      - Address of next instructions, instruction pointer or RIP (x86-64)
    - Register file:
      - Heavily used temporary data for faster access
      - Data used closed to where it is produced
    - Condition codes (somewhat specific to x86):
      - Some status info about most recent arithmetic or logical operation
      - Used for conditional branching
- Memory: address space exists here (byte addressable array)
  - Holds code and user data
  - Stack to support procedures
- Addresses (CPU --> memory), data (CPU --> memory), instructions (memory --> CPU) flow between CPU and memory

### RISC or CISC
- CISC: lots of instructions in ISA
  - Easier to add functionality, have backward compatibility
- RISC: fewer
  - Easier to implement, fast
- Hardware translation can keep backwards compatibility but benefit from RISC staying simple

### Compiling into Assembly
sum.c
```{C}
long plus(long x, long y);
void sumstore(long x, long y, long* dest){
  long t = plus(x,y);
  *dest = t;
}
```
-->
-Og to optimize
```
gcc -Og -S sum.c
```
--> sum.s
```{Asm}
sumstore:
  pushq %rbx
  movq  %rdx, %rbx
  call  plus
  movq  $rax, (%rbx)
  popq  %rbx
  ret
```
C++ program (text) (p1.c) --> gcc -Og -s to compile --> Asm program (text) (p1.s) --> assembler gcc or as --> Object program (binary) (p1.o) --> linker gcc or ld --> executable program (binary)
- instructions can be multiple bytes long
- Program has a particular address that it starts at

### Assembly Characteristics: Data types
- "Integer" data of 1,2,4,8 bytes
  - Data values, no distinction between signed/unsigned
  - Addresses (untyped pointers)
- Floating points of 4,8 bytes
  - (and 10 bytes but unused)
- Code: byte sequences encoding series of Instructions
- No aggregate types like arrays/structures
  - Just contiguously allocated bytes in memory
- Just primitive types in our ISA

### Assembly Characteristics: Operations  
- Arithmetic functions on register or memory data
- Transfer data between memory and register
  - Load memory data to register or store register data in memory

### Example
```{C}
*dest = t;
```
```{Asm}
movq  %rax, (%rbx)
```
- t: register %rax
- dest = register %rbx
  - * dest = Memory M[%rbx]
  - Parentheses mean access memory (like dereferencing a pointer)

```
0x40059e: 48 89 03
```
- Object code: 3-byte instruction, stored at address 0x40059e

### Single Example: MIPS format
- Length
  - 32-bits
  - 3 types for different Operations
  - Op: opcode, Rs: source Reg 1 , Rt Source Reg 2, Rd: Destination reg, Sh: shift, Func: additional opcode bits
- x86 is far more complicated: more prefixes and postfixes

### Disassembling Object code:
- Disassembler:
  ```
  objdump -d sum
  ```
  - Useful tool to examine object code
  - Analyze bit pattern to reproduce assembly code (approximately)
  - Can be run on either a.out (complete executable) or

### Reading Byte-Reversed Listings
- Value --> Pad to 32 bits --> Split into bytes --> Reverse

### x86-64 Integer registers
- 64-bits each, names prefixed with "r"
- Can reference lower-order 4 bytes with "e" prefix
- ex. %rax (%eax), %rbx (%eax), etc.
- Can reference lower orders in smaller registers all the way down to 8 bits
- ax = accumulate, cx = counter, dx = data, bx = base in array, si = source index, di = destination index, sp = stack pointer<sup>*</sup>, bp = base pointer
  - <sup>*</sup>rsp is a pointer to the data associated with a function

### Moving data:
```{Asm}
movq source, destination
```
- Operand types
  - Immediate: constant integer data
    - ```
      $0x400, $-533
      ```
    - Like C const but prefix with '$'
    - Encoded with 1,2,4 bytes

    - in C:
    ```{c}
      temp = 0x400; //register
      *p = -147 //memory
    ```
  - Register: one of 16 integer registers
    - ```
      %rax, %r13
      ```
    - %rsp reserved for special case
    - in C:
    ```{c}
    temp2 = temp1; //register
    *p = temp; //memory
    ```
  - Memory:
    - ```
    (%rax)
    ```
    - Various other "address modes"
    - in C:
    ```{c}
    temp = *p; //memory
    ```

### Example program for Move:
movq source, destination

```{C}
void swap(long *xp, long *yp){
  long t0 = *xp;
  long t1 = *yp;
  *xp = t1;
  *yp = t0;
}
```
- By convention, rdx holds first argument, rsi holds second

```{Asm}
swap:
  movq (%rdi), %rax   #to = *xp
  movq (%rsi), %rdx   #t1 = *yp
  movq %rdx, %(rdi)   #*xp = t1
  movq %rax, (%rsi)   #*yp = t0
  ret
```
### Simple Memory Addressing Modes
- Normal (R) -- Mem[Reg[R]]
  - Register R specifies memory address
  - Pointer dereference in C
  ```
  movq (%rcx), %rax  
  ```
- Displacement D(R) -- Mem[Reg[R] + D]
  - Register R specifies start of memory region
  - Constant displacement specifies offset
  ```
  movq 8(%rbp), %rdx
  ```
- Most General Form D(Rb, Ri, S) -- Mem[Reg[Rb]+S*Reg[Ri]+D]
  - D = constant "displacement" 1,2,4 bytes
  - Rb = base register (any of 16 int registers)
  - Ri = index register (except for %rsp)
  - S = scale 1,2,4,8
    - Powers of 2: how big is the data type in your array representation

### Some Arithmetic operations
- Computation: Dest = Dest operand src
- ex. addq (+), subq (-), imulq (x), etc. c
  - Check slides/book for list
- Argument order is relevant: 
  - Destroys the src register

### Address Computation Instruction
- leaq Src, Dst  -- "Load effective address"
- Src is address to mode expression
- Set Dst to address denoted by expression
- USES
  - Compute address without memory access
    - ```
     p = &x[i].val
     ```
  - Sneaky math: compute arithmetic of the form x + k*y
    - k = 1,2,4,8
    - example:
      ```
      long m12(long x){
        return x*12;
      }

      leaq (%rdi, %rdi, 2), %rax    # t <- x+x*2 x*3
      salq $2, %rax                 # return t << 2 arithmetic shift
      ```
