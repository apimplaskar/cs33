# Lecture 2
## October 1, 2019
---
### Review
- Binary can be used to represent everything
- Binary integers for: signed/unsigned numbers, boolean values (yes/no or t/f), sets of integers, etc.
- Limitations:
  - UMAX/TMAX and potential overflow
- Hexadecimal (base 16) is useful because it is easy to see correspondence with binary
- 0x2 & 0x1 is not the same as 0x2 && 0x1
  - & is bitwise
  - && is boolean
- Some practice to think through:
```{C}
  char a = 0x24;
  a = a && 0x3A; //a = 0x1 now
  a = a << 1; // a = 0x2
  a = ~a + 1; // inverts so now -2
  ```
- Data types | size (bytes)
  ---------- | ------------
  char       | 1
  short      | 2
  int        | 4
  long       | 8
- Always maintain unsigned, and always take larger type when evaluating expressions
  - But floats take priority
- Two's complement representation:
  - Most significant bit is the sign bit (has negative weight)

---
### Dealing with two's complement:
- Adding the complement: ```x + ~x = -1```
  - Every bit will be filled 1111...1111 or 0xFFFFF...F
- Just manipulating this expression gives us an identity for negating a number:
  - ```-x = ~x + 1 ```
- Harder to convert to higher precision
  - ex. -5 is 1011, cast to five bit?
    - Just padding with zeroes gives us 01011 = 11 (off by 16)
    - Put a one in the top digit: 11011 = -5
- Sign extension:
  - Make k copies of sign bit in front
  - Automatically happens in C when we cast (ex. from short to int)
---
### Integer Arithmetic
- UAdd<sub>w</sub> for unsigned addition: u + v (u and v are w bits long)
  - May have w+1 bits in the true sum -- would have to discard the carry to have w bits
  - This is modular arithmetic (u + v) mod 2<sup>w</sup> (wraps/overflows if sum >= 2<sup>2</sup> )
- TAdd<sub>w</sub> for signed addition: u + v (w bits long)
  - Same issue with discarding the carry, but also need to account for sign bit
- UAdd and TAdd have identical bitwise behavior
- Multiplying w-bit numbers x,y (either signed or unsigned):
  - Unsigned: Up to 2w bits
  - Two's complement min: Up to 2w-1 bits
  - Two's complement max: Up to 2w bits (TMin<sub>w</sub><sup>2</sup>)
- Multiplying by power of two?
  - Left shift
- Unsigned power of two division
  - Logically shift -- will throw away bits after binary point (floor)
  - u >> k gives floor (u/2<sup>k</sup>)
- Division of signed numbers cannot be done by shifting
  - Shift right and replicate the first bit at the beginning of the result
  - Arithmetic shift
- Want to round towards zero, but dividing negative numbers won't do so
  - ex. -5 is 1011 --> -5/2 = 1101 which is -3
  - We want it to be -2
  - We can do this by adding a bias before dividing (2<sup>k</sup>-1)
    - So now we compute : ``` (x + (1<<x)-1) >> k ```

### Why to use unsigned:
- Don't use without understanding implications:
  ```
  unsigned i;
  for (i = cnt-2; i >= 0; i--){
    //can be infinite loop because of i overflowing
    a[i] += a[i+1]
  }
  ```
---
### DataLab Preview
- Bit-level manipulations
- dlc to check legal, btest to check test cases
---
### Fundamental Question for ISA: Where do we put data?
### Address space (abstraction!)
- envision it as a large array of bytes (conceptually)
- address is like an index into that array, and a pointer variable stores an address
- Note: system provides private address spaces to each "process"
- Program cannot access data of others
- Byte addressable:
  - Can't refer to a bit, only byte or larger
    - 0x0001 refers to first byte
  - Programs don't usually access individual bits (simple types in C operate on byte level) -- makes our addresses smaller
- Address size is nominal (word size)
  - Most machines used to have word size of 32 bits (4 bytes) -- limits addresses to 4GB
  - Now have about 64-bit word size
    - Potentially 18EB of addressable memory

### Numeral ordering:
- ex. One Hundred and Forty two
  - Decimal: 1 * 10<sup>2</sup> + 4 * 10<sup>1</sup> + 2 * 10<sup>0</sup>
  - Big endian: 142
  - Little endian: 241
- Some ISAs have big endian properties -- least significant byte has highest address
- Others have large endian -- least significant byte has lowest address (x86, ARM, iOS/Windows)
- Truncating little endian makes more sense (don't need to move things)

### Strings in C
- represented by an array of characters
  - each character encoded in ASCII format
  - 0 is 0x30, i is 0x30 + i

### Integer C puzzles
```
int x = foo();
int y= bar();
unsigned ux = x;
unsigned uy = y;
```
is the implication always true?
- x < 0  --> ((x*2) > 0)?
  - NO, could overflow
- ux >= 0
  - YES
- x & 7 == 7 --> ((x << 30) < 0)
  - YES
  - x = 0000...0111  --> shifted to 1000...0000
- ux > -1
  - NO
  - -1 gets cast to unsigned (UMAX) prior to comparison
- x > y --> -x < -y
  - x = TMAX, y = TMIN
  - -TMAX = TMIN + 1, -TMIN = TMIN
  - NO
- x * x >= 0
  - NO, could overflow and wrap around
- x > 0 && y > 0 --> x+y > 0
  - NO, could overflow
- x >= 0 --> -x <= 0
  - YES
- x <= 0 --> -x >= 0
  - NO, -TMIN = TMIN
- (x|-x) >> 31 == -1
  - NO, x=0
- ux >> 3 == ux/8
  - YES
- x >> 3 == ux/8
  - NO
- x & (x-1) != 0
  - NO
  - False for any power of 2 -- of the form 100000000...0000
