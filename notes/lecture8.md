# CS33 - Lecture 8
## Midterm Review
---
### TOPICS:
- number representation, bit operators, casting, assembly, endianness, stacks and frames, structs and unions

### 1. Number Representation
- Binary and 2s complement
- Data types/sizes
- Tmin/Tmax (32 bit):
- Umin/Umax (32 bit):

### 2/3. Bit operators + casting
- C puzzles!
-
```
int x = foo(); unsigned ux = x;
x > 0 && y > 0 --> x + y > 0
5 * ux > ux
x < 0 --> 2 * ux > ux
x & 7 == 7 --> x << 29 < 0
```
- F (sign overflow), F (unsigned overflow), F (tmin), T

### 4. Assembly
- Can appear as:
  - Match assembly snippets to C Code or vice versa
  - Write alternative assembly instructions (unlikely)
-
```
leaq(%rbx, %rbp), %rbp <--> add %rbx, %rbp
leaq(, %rdi, 2), %rdi <--> imul $2, %rdi
mov %rax, %rax <--> nop
add $0, %eax <--> mov %eax, %eax
xor %rbx, %rbx <--> mov $0, %rbx
```

### 5. Endianness + Structs/Unions
- Given memory (as in the slide), interpret as different data types
- example:
```
struct S1{ char a; short b; int c; char d; } instance;
short i[4];
```
  - int = 0x63333C49
  - C string = 0x49 0x3C 0x33 0x63 0x73 0x33 0x33 = "I<3cs33"
  - instance.d = 0xFE =
  - i[2] = 0x63333C49 0x00333373
- example:
  ```
  typedef struct {
    char c;
    int i;
    double d;
    int n;
  } s;

  void main(int argc, char** argv){
    s_arr[7];
    printf("%d\n", (int)sizeof(arr));
  }
  ```
  - Draw out struct: 24 bytes
  - prints 24*7 = 168
---
