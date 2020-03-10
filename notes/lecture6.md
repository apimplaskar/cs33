# CS 33 - Leture 6
## Week 3, Tuesday (10/15)
---
### Review
- function modifies both %rbx and %rcx
  - do either of the registers have to be saved on the stack?
  - %rbx is callee saved - so that function is responsible for saving it
- first 6 int arguments are passed through registers, the rest on the stack
  - why not all of them on the stack? SPEED
- don't return an address to something on stack -- but could return address of malloc or of global var
---
### Recursive function
```
long pcount_r(unsigned long x){
  if (x == 0){ return 0; }
  else{
    return (x&1) + pcount_r(x>>1);
  }
}

pcount_r:
  movl $0 %eax
  testq %rdi $rdi //&
  je .L6
  push1 %rbx
  movq %rdi, %rbx
  andl $1, %ebx
  shrq %rdi
  call pcount_r
  addq %rbx, %rax
  popq %rbx
.L6:
  ret
```
- caller saved registers pushed on stack if caller uses; callee pushes on stack if callee uses
- Handled without special consideration
- Unused positions in stack:
  - Lack of an instruction
  - Unused parts corresponds to stack allocation (sub $0x8 %rsp)
---
## Machine level programming: Data
###Array allocation:
- T A[L] -- array A of type T and size L
- Contiguously located region of L*sizeof(T) bytes in memory
- Very likely to be allocated in subsequent addresses

### Multidimensional arrays:
- T A[R][C] -- 2d array of type T with R rows and C cols, Type T requires K bytes
- Array size = R * C * K bytes
- Arrangement: row major ordering:
  - consecutive elements of a row are contiguous
- A[i] is the ith row array of C elements
  - A + i * (C * K)
    - A starting address, i = row, K bytes/elt, C elts/row
- A[i][j]
  - A + i * (C * K) + j * K
  - = A + (i * C + j) * K

### Multi-level arrays:
- Each element is a pointer -- flexible in terms of size for each component array
- Cost: have to perform a pointer access (load from memory)
- arr[ind][dig] = Mem[Mem[arr+k * ind] + 4 * dig]

### Structure representation
- C standard determines overall size + positions of fields
- Struct represented as a block of memory (big enough to hold all fields)
- Fields ordered according to declaration (does not optimize)
- Compiler obeys standard when generating code (no description of structure itself -- only accesses data with the appropriate address)

### Structures and Alignment
- unaligned: no regard for multiples of 4
- aligned: primitive data types require K bytes
  - Address must be a multiple of K
-
