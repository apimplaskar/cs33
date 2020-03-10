# CS 33 - Lecture 7
## Oct 17, 2019
---
### Structure Representation
```
struct rec{
  int a[4];
  size_t i;
  struct rec *next;
}
```
[ --- a ---  | i | next]

- Aligned data: primitive data type requires K bytes, address must be a multiple of K
  - for easier memory access
  - Compiler: inserts gaps in struct to correctly align fields
- Specific cases of alignment:
  - 1 byte: char, etc. -- can be on any address
  - 2 byte: short -- lowest 1 bit of address must be 0<sub>2</sub>
  - 4 bytes -- lowest 2 bits are 00
  - 8 bytes -- lowest 3 bits are 000
  - 16 bytes -- lowest 4 bits are 0000
- Overall struct placement:
  - alignment requirement K (K = largest alignment of any element)
  - Initial address/length must be multiples of K
- Arranging our structs to save space:
  - so we can minimize byte padding

### C operators:
- see table in Lecture slides
- Practice problems in slides
---
### Review:
- in 2D array: can never tell how many rows there are simply by looking at assembly (but we can tell col size = number of elements per row)

### Unions:
- Allocate according to largest element, all elements share the same space (align together)

### Using Union to Access Bit Patterns
- Can reinterpret bit patterns as different types (different than casting values)

### Byte ordering (again)
- Big endian: most significant byte has lowest address
- Little endian: least significant byte has lowest address
- Bi endian: can configure either way
- Can use the bit patterns to detect endianness

### Compound types in C WILL BE ON MIDTERM
---
### Memory Layout
- Stack (runtime stack 8MB limit), ex. local vars
- Heap: dynamically allocate as needed (malloc(), calloc(), new())
  - Big dynamically allocated objects are higher up, smaller are lower down -- avoiding fragmentation
- Data: statically allocated data (global vars, static vars, string consts)
- Text/Shared libraries: executable machine instructions, read only

**TOP**
[STACK] 8MB --> [] [Shared Libraries] [] <-- [Heap][Data][Text - code]
**BOTTOM**

---
### Buffer Overflow (Vulnerability and Protection)
- For instance, if we have an array within a struct and we try to write outside of the allocated space for the array -- we may encounter some odd behavior since it may start modifying parts of the struct that it doesn't mean to
  - Maybe even overwrite return address since struct may grow up the stack frame

### Protect
- Randomize stack offsets:
  - Address space layout randomization
  - Protects from attacks
- Nonexecutable code segments
- Stack canary:
  - place special "canary" value just beyond buffer
  - Checks for corruption before exiting function
  - Randomized value at a particular loc in program -- if it is still there after the call is complete, we're safe
