# Lecture 4
## CS 33 - 10/8/190
---
### Review
- Name the hardware implementation that is not visible to the programmer: **microarchitecture**
- Where would I store (immediate, register, memory):
  - arrays of data: memory (can't index registers)
  - program code: memory
  - temporary variables: registers
  - constants: any
  - function args:
- in x86-64 assembly, two-operand instruction -- is src or dest the first argument:
  - Dest = Dest - src for subq
- b: byte (8bits), w: word (16 bits), l: long (32 bits), q: quad (64 bits)
- floating point: s: single (32 bits), l: long float (64 bits)
- mov**z**bl: zero extend a byte sized register to a long
- mov**s**bl: sign extend a byte sized register to a long
- RIP: instruction pointer
  - R = indicates instruction pointer is a 64 bit register

### Review: Address computation instruction
- leaq src, dest  -- "load effective address"
- Uses:
  - Address comp: compute addresses without a memory reference
  ```
  p = &x[i].val
  ```
  - Sneaky math: x+k*y for k = 1,2,4,8
  ```
  leaq(%rdi, %rdi, 2), %rax = t <- x+x*2
  ```
  - Doesn't delete the original register
---
### Some instructions use implicit registers
- Mainly one operand instructions
- imull S : multiplies S with R[%eax]
  - Lower 32 bits in eax, upper 32 bits in edx
  - Signed
- mull: same as above, just unsigned
- cltd: sign extends eax to quad word
  - Lower 32 bits in eax, upper in edx

### Ordering of instructions -- Control Flow
- Control: broadly refers to how to decide what to do next
- Metaphor:
  - program counter points to instruction that has control
  - Control flow: each instruction passes flow to the next instruction
  - Control instruction: alter which instruction to execute next
  - Control statement: for, while, if, return
- Strategy in x86:
  - Arithmetic instructions have side effects
  - Results of side effects are held in additional registers called condition codes
  - Control instructions (ex. branch/jump) use control codes to decide where to pass control

### Processor state
- Information about currently executing program
    - Temporary data (%rax, ...)
    - Location of runtime (%rsp, ...)
    - Location of Control
    - Condition codes

### Condition codes
- CF: carry flag (for unsigned) -- set if the result overflows
- SF: sign flat (for signed) -- if result is signed
- ZF: zero flag -- if result is zero
- OF: overflow flag (for signed) -- if signed overflow
- Explicit setting:  
  - compare - cmpq Src2, Src1
    - cmpq b, a like computing a-b without setting destination
    - CF set if carry out from most significant bit
    - ZF if a == b
    - SF if (a-b) < 0
    - OF set if (a>0 && b<0 && (a-b) <0 || a<0 && b>0 && (a-b)>0)
  - test - testq Src2, Src1
    - testq b, a like computing a&b without dest
  - SetX instruction variants: set low order byte of dest to 0 or 1 based on combinations of condition code
    - ex. sete %al -- sets %al to 1 if zero flag is set

### Jumping
- There is an unconditional jump (jmp), but there are also jumps for all condition code flag combos
  - ex. je (condition = ZF) -- if equal/diff is zero

### Using Goto code:
  - C allows goto statement -- jump to position designated by label
  ```
  long absdiff(long x, long y){
      long result;
      if (x>y){
        result = x-y;
      }
      else{
        result = y-x;
      }
      return result;
  }

  long absdiff_j(long x, long y){
      long result;
      int ntest = x <= y;
      if(ntest) goto Else;
      result = x-y;
      goto Done;
    Else:
      result = y-x;
    Done:
      return result;  
  }
  ```

### Ternary operator translation
```
val = Test ? Then_Expr : Else_Expr:

val = x>y ? x-y : y-x;
```
### Alternate to control flow
- Conditional move instructions:
  - Instruction supports -- if(Test) Dest <- Src
  - ex. cmove %rdx, %rax
    - cmov + e -- move rdx to rax if zero flag is set
  - This does BOTH the THEN and the ELSE and then uses conditional to set the result
- Bad cases for conditional moves:
  - expensive computations or many computations
  - computations with side effects
  - risky computations

### Loops
- Do while example:
```
long pcount_do(unsigned long x){
    long result = 0;
    do{
      result += x & 0x1;
      x >> 1;
    }while(x);
    return result;
} //popcount - counts 1s in argument x
```
```
long pcount_goto(unsigned long x){
  long result = 0;
  loop:
    result += x&0x1;
    x >> 1;
  if(x) goto loop;
}
```

  ```
  do{...}while(condition);

  loop: (...)
  if(condition): goto loop
  ```
- While loop:
  ```
  while(condition){...}

  goto(condition)
  loop:
    ...
  if(condition): goto loop
  ```
- For loop:
  ```
  for(init; test; update){...}

  //to while Loops
  init;
  while(test){...
    update
  }

  //then use goto for while loop
  ```

### Switch statements:
```
switch(x){
  case 0: y = func1(x); break; //say its 0x400
  case 1: y = func2(x); break;
  case 2: y = func3(x); break;
  case 3: y = func4(x); break;
}


```
- Consecutive range of case addresses
  - Jump to 0x400 + 8 * x
- Store addresses into a jump table and indirect jump to it
- Jump
  | Case address    | Target   |
  | :------------- | :------------- |
  | 0x400       | points to Func1       |
  | 0x408       | points to Func2       |  
  | 0x412       | points to Func3       |
  | 0x416       | points to Func4       |
- Handling fall through, missing cases, defaults, non-zero start range, etc.
- jmp * .L4(%reg, bytes) -- INDIRECT, SHOULD BE SWITCH
- jmp .L8 -- DIRECT JUMP
