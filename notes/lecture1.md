# CS 33 Lecture 1
## Intro to Computer Organization
## Instructor: Tony Nowatzki -- tjn@cs.ucla.edu
---
```{C}
//sample code block
```
---
Algorithm -> Application -> Programming Language -> Compiler -> Operating System (**SOFTWARE**)

--> Instruction Set Architecture (ISA)

--> Hardware Organization -> Component Design -> Circuit Design -> Devices (Transistors) -> Mechanical/Physical components (**HARDWARE**)

---
ISA: interface determines representation of programs to hardware (we focus on X86 in this class)

What should be in an ISA?
- Memory (to hold values) + input/output
- Instruction: Manipulating memory, move from one state to the next
- Program: set of instructions (in memory)
- Execution model: When do we execute each instruction?
  - Von Neuman Model: execute instructions sequentially, defined by program counter

Executing an instruction:
- Fetch (grab instruction from memory)
- Decode (interpret instruction)
- Execute (perform computation)
- Writeback(update state)

Theme: Abstraction is good, but...
- examples in slides

---
## Bits, Bytes, and Integers
### Bits
- Each bit is 0 or 1
- We encode/interpret sets of bits in various ways
- Easier for hardware to have bistable elements (2 states)
- Counting in binary
  - Base 2 number representation:
    33 base 10 = 10001 base 2
    - 3x10^1 + 3x1 = 1x2^5 + 1x1
  - Also works for decimals -- but there are some numbers we cannot express with a finite number of digits (ex. repeating decimals)

### Bytes
- Byte = 8 bits -- represent with hexadecimal (base 16 using 0-9 and A-F)
  - ex. 11110000 (base 2) = 240 (base 10) = 0xF0 (hex - two digits)
  - 0x is a notation for hexadecimal
- Why do bytes have 8 bits?
  - Can fit all of ASCII chars into 8 Bits
  - Power of 2 and smallest useful data type
- Data types in C have sizes that are multiples of 8

### Bit-level manipulations
- Boolean algebra:
  - True = 1, False = 0
  - Truth tables:
    - AND: 0&0 = 0, 0&1 = 0, 1&0 = 0, 1&1 = 1
    - OR: 0|0 = 0, 0|1 = 1, 1|0 = 1, 1|1 = 1
    - NOT: ~0 = 1, ~1 = 0
    - EXCLUSIVE-OR (XOR): 0^0 = 0, 0^1 = 1, 1^0 = 1, 1^1 = 0
- General boolean algebras:
  - Operate on bit vectors -- applied bitwise
- ex. represent/manipulate sets
  - Set: {0,1,..,w-1}
  - Boolean: 1 = present in subset, 0 = not present
  - Operations: & is intersection, | is union, ^ is symmetric diff, ~ is complement
- Bitwise operations are different from logical operations:
  - && , ||, !
  - View 0 as False, anything nonzero as True, always return 0 or 1
  - Early termination
  - ex. p && * p for a pointer p
    - Prevent segfault (not nullptr and nonzero)
- Shift operations:
  - Left shift: x << y
    - Shift bit-vector x left y positions
    - Throw away extra bits on the left
    - Fill in with 0s on the right
  - Right shift: x >> y =
    - Shift bit-vector x right y positions
    - Throw away extra bits on the right
    - Logical shift:
      - Fill in with 0s on the left
    - Arithmetic shift:
      - Replicate most significant (top) bit on left
  -Undefined behavior:
    - Shift amount < 0 or >= word size

### Integers
- Unsigned: take each bit value and multiply it by a weight
  - B2U(X) = sum from 0 to w-1(xi * 2^i)
- Two's Complement: Interpret a bit vector as a signed number
  - B2T(X) = -xw-1 * 2^(w-1) + sum from 0 to w-2(xi * 2^i)
  - -xw-1 * 2^(w-1) is the sign bit
  - See slides for better sum formula
- In C, short is 2 bytes long
- Sign bit: most significant bit is 0 for nonnegative, 1 for negative
- UMin = 0, UMax = 2^w - 1
- TMin = -2^(w-1), TMax = 2^(w-1) -1
- Minus 1  111...1
- Equivalence (nonnegative values encoded the same), Uniqueness (each bit pattern has a unique int value, each representable int has a unique bit encoding)
  - Can invert mappings
- Conversion/casting:
  - Keep bit representations and reinterpret
  - Ordering inversion from 2s comp to Unsigned
    - Negative --> big Positive
- Signed v. Unsigned in C:
  - By default: considered signed
  - If have U as a suffix: unsigned
- Casting:
  - explicit casting between signed and unsigned is the same as U2T and T2U
  ```{c}
  int tx, ty;
  unsigned ux, uy;
  tx = (int) ux;
  uy = (unsigned) ty;
  ```
  - Implicit casting on assignment (also via procedure call arguments)
  ```{c}
  tx = ux;
  uy = ty;
  ```
- Expressions and Types in C
  - Need same bit width for operations
  - If both are integers always convert to the longer data type
  - Unsigned takes precedence over signed
  - Check slides for some surprises/exceptions
  - Apply signed/unsigned prior to casting
