# Lecture 13
## Nov 12, 2019
---
### Why cache?
- Smaller fixed-size staging area for memory in hardware
- Constant time lookups/insertion - data structure?
- Hash table?
  - Problem 1: real hash functions take too long
    - just use some bits of address
  - Problem 2: hash collisions cause serial checks
    - in hardware, we can do N-way parallel comparison
    - hash to find set of elements -- do parallel checks for each set (set-associative cache)

### Cache read algorithm
- Address of word
  - [ t bits of tag | s bits of set index | b bits of block offset ]
- Cache has S = 2<sup>e</sup> sets, E = 2<sup>e</sup> lines per set
- Locate set by set index
- Look at each line in set to ensure they are valid
- Check if tag is the same as the tag we request
- Use block offset to index into data associated with each line -- extract relevant bits

### Cache preference
- Problem that lets us decide which cache is best (direct vs. set associative)
- Not always the easiest to decipher in terms of code

### Blocking
- If data can't fit in cache, we can split up the data so that we compute partial values that do fit in cache
- Can usually reduce cache misses

### Review the matrix-vector case study in Nowatzki's slides

---
### What is on the book's cover?
- Tradeoff between spatial and temporal locality

### The Memory Mountain:
- Read throughput (read bandwidth): number of bytes read from memory per second (MB/s)
- Memory mountain: measured read throughput as a function of spatial and temporal locality, characterize memory system's performance
- Can get ridges of temporal locality

### Cache Summary:
- significant performance impact
- Focus on inner loops where bulk of computation/memory access occurs
- Can reorganize loops (smaller stride for spatial locality, reuse for temporal locality)
---
## Dynamic Memory Allocation
- Stored on the heap -- if we don't know about size until runtime
- Whatever virtual memory we use has to be backed by physical memory (important to manage it)

### Target interface: malloc package
```
#include <stdlib.h>
void *malloc(size_t size)
```
- Succesful: returns pointer to memory block of at least size bytes aligned to 8/16 byte boundary
  - If size == 0, returns NULL
- Unsuccesful: returns NULL and sets errno

```
void free(void *p)
```
- Returns block pointed at by p to a pool of available memory
- p must come from previous call to malloc or realloc
- Other functions: calloc (initializes allocated block to 0), realloc (change size of previously allocated block), sbrk (used internally by allocators to grow/shrink the heap)

### Assumptions for this lecture
- Memory is word-addressed, words are int-sized

### Constraints
- Applications:
  - can issue arbitrary sequence of malloc and free requests
  - free request must be to a malloc'd block
- Allocators:
  - can't control number/size of allocated blocks
  - must respond immediately to malloc requests (can't reorder or buffer requests)
  - must allocate blocks from free memory
  - must align blocks so they satisfy alignment requirements
  - can't manipulate and modify only free memory
  - can't move allocated blocks once they are malloc'd

### How to implement a dynamic allocator
- Role of Operating System:
  - OS controls how much virtual memory a process has access to
  - Program can request more virtual memory
    - sbrk(int x) system call
    - Argument x = how many bytes more virtual memory this process wants
- How do we decide which part of the virtual memory we assign to which request?
  - Need some data structures to manage that
  - Performance and memory overhead are critical

### Performance goal: throughput
- Given some sequence of malloc/free requests
  - R0, R1, ... Rk, ..., Rn-1
- Def: aggregate payload P<sub>k</sub>
  - malloc(p) results in a block with a payload of p bytes
  - After request Rk has completed, aggregate payload is the sum of currently allocated payloads
- Def: current heap size Hk
  - Hk is monotonically nondecreasing (heap only grows when allocator uses sbrk)
- Def: Peak memory utilization after k+1 requests
  - Uk = (max Pi)/Hk

### Fragmentation
- poor memory utilization caused by fragmentation
- Internal fragmentation
  - if payload is smaller than block size
  - caused by maintaining heap/alignment restrictions
  - depends on pattern of previous requests
- External Fragmentation
  - enough aggregate heap memory but no single free block is big enough

### Implementation Issues
- How do we know how much memory to free given just a pointer?
- How do we keep track of free blocks?
- How do we pick a block to use for allocation -- many free blocks might fit
- If we pick a block that is too big, what do we do with extra space?
- How do we reinsert freed blocks?

### Knowing how much to free
- Standard method: keep the length of a block in the word preceding the block
  - Called header (field)
- Require extra word for every allocated block

### Keeping track of free blocks
- Method 1: implicit list using length -- links all blocks
  - We can just iterate over the header blocks based on size
  - Keep going until we find a block that fits
- Method 2: explicit list among free blocks using pointers`
- Method 3: segregated free list
  - different free lists for different size classes
- Method 4: blocks sorted by size
  - use a balanced (red-black) tree with pointers within each free block -- length is a key

### Method 1: implicit list
- For each block we need both size and allocation status
  - Could store this information in two words: wasteful
  - Use an always 0 bit as an allocated/free flag
  - When reading size word, we just mask out this bit

### Implicit list: find a free block
- First fit: search list from beginning and return first block that fits
```
p = start;
while((p < end) && ((*p & 1) || (*p <= len)))
  p = p+(*p & -2);
```
- Next fit: like first fit but search list starts where previous search ended, faster than first fit to avoid re-scanning unhelpful blocks
- Best fit: search list and choose best free block (fits with fewest unused bytes), keep fragments small (better memory use) but slower

### Implicit list: allocating in a free block
- Splitting:
  - Since allocated space might be smaller than free space, we might want to split the block

### Implicit list: freeing a block
- Simplest implementation: need only clear the allocated flag
  - Can lead to false fragmentation
- Coalescing: join with the next/previous block if they are free

### Implicit list: bidirectional coalescing
- Boundary tags (Knuth 73)
  - Replicate size/allocated word at the bottom (end) of the free block
  - Allows us to traverse list backwards (though it takes extra space)

### Constant time Coalescing
- Cases in the slides

### Summary of Allocator Policies in slides
