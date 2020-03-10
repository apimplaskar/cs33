# Discussion 1 (Week 0)
---
## GDB: GNU Debugger

useful commands:
  - step: step in
  - next: next block
  - kill: ends program
  - quit: exits GDB

---
## Bit operators
- Binary representations of numbers:
  - Keep dividing by two and read bottom up
  - Or: work with powers of two
- Logical boolean operators: && , || , !
  - Zero = false, Nonzero = True
- Bit boolean operators: & , | , ~ , ^
- Converts all numbers to binary and operates bitwise

### Some examples
- 8 & 7 --> 1000 & 0111 --> 0000
  - Returns 0
- 8 | 7 --> 1000 | 0111 --> 1111
  - Returns 15
- 12 | 11 --> 1100 | 1011 --> 1111
  - Returns 15
- 5 ^ 7 --> 101 ^ 111 --> 010
  - Returns 2
- ~15 --> ~1111 --> 0000
  - Returns 0

## Representing and Manipulating Sets
- Given a set S, we can decide whether or not a value is in a subset A by giving A all of the locations of S and simply assigning binary values (is it in A or not) to each location
  - S = {1,2,3,4,5,6,7,8} , A = {2,3,5,7,9} , B = {1,4,5,9}
    - A represented as {0,1,1,0,1,0,1,0,1}
    - B represented as {1,0,0,1,1,0,0,0,1}
  - A & B is the intersection: {0,0,0,0,1,0,0,0,1}
  - A | B is the union: {1,1,1,1,1,0,1,0,1}
  - ~A is the set complement: {1,0,0,1,0,1,0,1,0}
  - A ^ B is the symmetric difference: {1,1,1,1,0,0,1,0,0}
---
