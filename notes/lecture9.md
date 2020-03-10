# CS 33 Lecture 9
## 10/24/19
---
### Buffer Overflow
- allow remote machines to access and execute arbitrary code on victim machines
- create a buffer that gets put on the stack, if user info overflows it, we may inch into different spots on the stack
- HANDLING THIS:
  - programmer: use library routines that limit input lengths
  - system level protections:
    - randomize stack offsets (more difficult for a hacker to predict beginning of inserted code/beginning of stack frame)
    - marking regions of the stack as executable/read only/writeable/etc.
  - compiler:
    - Stack canary: place value on stack just beyond buffer and check it for corruption before exiting the function
    -
      ```
      -fstack-protector
      ```
    - load canary --> run function --> compare canary value before we return

### Return-oriented programming attacks

- Challenge for hackers with stack randomization and non-executable stack marks
- Alternative strategy is to use existing code to perform desired functionality
- ROP Execution
  - Add desired function calls at return address so that we can get sequential execution of desired functions
    - Return from one function = pointer to next function
- ROP gadgets:
  - gadget: ending fragments of a function
  - sequence of instructions in ret (encoded by single byte ending in 0xc3)

---
## Floating point representation
### Binary fractions
- 5 3/4 --> 101.11, 2 7/8 --> 10.111, 1 7/16 --> 1.0111
  - Beyond binary point, we get into negative powers of 2
- Can still perform the operations as expected

### Fixed point fractional binary numbers
- Binary point is put X digits into the number (X = 3)
  - 00001000 = 1.000 = 1
  - 00000001 = 00000.001 = 1/8
- Allows us to use integer arithmetic
- Limitation: can only get numbers of the form x/2<sup>k</sup>
  - Some rationals have repeating representations
- Limitation: one setting of binary point within w bits (fixed point) -- yields a limited range of numbers

### IEEE 754 standard -- for floating point representation
- three components: sign, location of binary point, actual bits of the number
- (-1)<sup>S</sup> M 2 <sup>E</sup>
  - s = sign bit
  - M = significand (mantissa) -- fractional value in range [1,2)
  - E = exponent weights value by power of two
- Encoding:  
  s | exp | frac
  - Most significant bit s encodes sign bit, exp field encodes E (but is not equal to E), frac field encodes M (but is not equal to M)
- Precision options:
  - 32 bits: 1 bit for sign, 8 bits for exp, 23 bits for frac
  - 64: 1, 11, 52
  - 80: 1, 15, 63-64
- Different encodings for numbers dependent on range of the number

### Normalized values
- when exp is not 000...0 and exp is not 111...1
- Exponent coded as a biased value: E = Exp - bias
  - Exp = unsigned value of exp field
  - Bias = 2<sup>k-1</sup> - 1 (k = number of exponent bits)
    - Binary rep of bias becomes the 0
- Significand coded with implied leading 1: M = 1.xxx...x<sub>2</sub>
  - xxx...x<sub>2</sub> = bits of the frac field
  - Miminum = frac = 000...0 (M = 1.0)
  - Max = frac = 111...1 (M = 2.0 - epsilon)
  - Get extra leading bit for free
  - WE DON'T HAVE A ZERO
- ex. 33 = 10 0001<sub>2</sub>
  - 1.00001<sub>2</sub> x 2<sup>5</sup>
  - Significand: M = 1.00001<sub>2</sub>
    - frac = 00001000000000000000000<sub>2</sub>
  - Exponent: E = 5
    - bias = 127
    - exp = 132 = 10000100<sub>2</sub>
  - Sign = 0
  - Result: 0 1000100 00001000000000000000000

### Denormalized values
- Purpose: represent values close to 0
- Denorm condition: exp = 000...0
  - Exponent interpretation: increase E by 1
  - E = 1-bias (NOT 0-bias)
  - Significand interpretation: no leading 1
  - M= 0.xxx...x<sub>2</sub> where xxx...x are bits of frac
- Cases: exp = 0, frac = 0 is ZERO
  - But +0 and -0 are distinct bc of sign bit

### Special Cases
- exp = 111...1
  - AND frac = 000...0 = +/- INFINITY
  - AND frac != 000...0 = NaN

### 8 bit floating point:   
- s = 1, exp = 4, frac = 3

### General notes
- Distribution of numbers gets denser closer to zero
- Largest possible number (in 6 bit IEEE like format -- 1 2 3 for s e f):
  - 0 110 11: exp = 6-3 = 3, frac = 1.11 -- largest number = 1110 = 14

### Special properties of IEEE encoding
- Zero is all bits 0
- Unsigned int comp:
  - First compare sign bits
  - -0 = 0
  - NaN is problematic
  - Otherwise it works fine

### Floating point operations
- x +<sub>f</sub> y = Round(x+y)
- x x<sub>f</sub> y = Round(x * y)
- Rounding modes:
  - Toward zero
  - Round down
  - Round up
  - Nearest even (default) + deal with halfway in between by rounding to nearest even

### Multiplying:  
- sign: s1 ^ s2
- Significand M: M1 x M2
- Exponent E: E1 + E2
- Need to make M have an implied M
  - if M >= 2, shift R and increment E
  - if E out of range, overflow
  - Round M to fit frac precision
- Implementation: hardest part is multiplying significands

### Addition:
- sign S, significand M: result of signed align & add
- exponent: E1
- Fixing:
  - if M>=2, shift M R and increment E
  - if M<1 shift M L k positions, decrement E by k
  - out of range = overflow

### Warning -- floats are not reals
- FP Ops are not associative
- Addition:
  - (3.14 + 1e-10) - 1e-10 = 0
  - 3.14 + (1e-10 + 1e-10) = 3.14
- Multiplying:
  - (1e20 * 1e20) * 1e-20 = inf
  - 1e20 * (1e20 * 1e-20) = 1e20

---
### Floating point in C
- two levels: float (single precision), double (double precision)
- Conversion/casting
  - int/float casting can change representations
  - double/float --> int
    - truncates fractional part, like rounding to zero
    - undef for out of range or NaN -- generally TMin
  - int --> double
    - exact conversion as long as int has <= 53 bits word size
  - int --> float
    - will round according to rounding mode

### Floating point puzzles
- Puzzles solns:
  - No (can lose some precision), Yes, Yes, No, Yes, ?, ? , ? , No
