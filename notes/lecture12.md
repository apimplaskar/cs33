# Lecture 12: On-chip Cache
## Nov 7, 2019
---
### Review
- Volatile := does memory get wiped when power is lost
- SRAM: really fast, low density (expensive), volatile
- DRAM: fast, medium density, volatile
- Flash: slow, high density, not volatile
- Disk: super slow, high density, not volatile

### Locality
- Principle of locality: programs tend to use data/instructions with addresses near or equal to those they have used recently
  - Temporal: recently referenced items likely to be referenced again
  - Spatial: items with nearby addresses are referenced close to each other in time

### Locality -- the how = CACHE
- Storage structure to hold/manage data that we think will be useful in the near future
- Cache hit: data we want is in the cache
- Cache miss: data we want is not in the cache
  - We will have to load the value (from next level in memory hierarchy) into the cache and figure out what to evict

### Conceptual cache design 1: Fully associative
- Look up data? Check all addresses and see if one matches
- On cache fill, we evict: least recently used (LRU) on bottom, insert new item at top
- GOOD: very flexible
- BAD: complex hardware -- hard for large caches

### Making caches simpler:
- don't need full flexibility to map to address anywhere (overkill)
- Can we restrict how much of the cache we have to check?
  - Use the address itself to decide what to evict, rather than how recently we uesd the data

### Conceptual design 2: Direct mapped
- Look only at lower bits of address to determine which element to check
- Only evict element which the address maps to on cache fill
- GOOD: Less work for hardware to perform
- BAD: might evict a useful item

### Conceptual design 3: Set associative
- Lookup: look at lower bits of address to determine which set to check -- then check all addresses in that set
- Evict: one of the options in the set -- probably the least recently used
- Tradeoff between complexity and flexibility

### Address overhead problem
- PROBLEM: Address takes up too much overhead
- Solution 1: Bigger cache lines: 1 byte --> 16 bytes (or more) -- for data line
- Solution 2: Don't store the whole address
  - Tag = truncated address with implicit bottom bits

### General Set Associative Cache Organization (S, E, B)
- S = 2<sup>s</sup> sets
- E = 2<sup>e</sup> lines per set
- B = 2<sup>b</sup> bytes per cache block
- Cache size = S * E * B bytes
- Extra bit stored: valid bit

### Cache read:
- tag = t bits | set index = s bits | block offset = b bits
- Locate set using set index
- Check if any line in set has matching tag (as requested address) -- FOR ALL BLOCKS IN THE SET
- If the tag matches and the valid bit is set = HIT
- Locate data starting at offset
- If tag doesn't match -- evict old line and replace

### Writes
- Multiple copies of data exist: L1, L2, L3, main memory
- Write-Hit
  - Write through: immediately to next level
  - Write-back: defer write to memory until replacement of line
    - Need a dirty bit (line diff from memory or not)
- Write-miss
  - Write allocate: load into cache, update line in cache
    - Good if more writes to location follow
  - No-write-allocate: write straight to memory, do not load into cache
- Typically:
  - Write-through + No-write-allocate
  - Write-back + Write-allocate (don't send data to next level farther from CPU if you don't have to)
- For examples, we will assume we try to keep data in the cache so write-back + write-allocate

### Types of Cache Misses
- Cold (compulsory) miss: empty cache
- Conflict miss: item fits would have fit in the cache -- was evicted due to restrictions on where it can be placed (associativity)
- Capacity miss: set of active cache blocks (working set) is larger than the cache

### Performance metrics
- miss rate: fraction of memory references not found in cache
  - misses/accesses = 1 - hit rate
  - Typical numbers: 3-10% for L1, smaller for L2
- Hit time: time to deliver a line in cache to processor
  - Includes time to determine whether or not line is in the cache
- Miss penalty: additional time required bc of stack miss
- Huge discrepancy between hits and misses
  - Misses are MUCH worse than hits so we use miss rate more than hit rate
  - ex. 99% hits v 97% hits yields a huge discrepancy in cycles

### Writing cache friendly code
- Focus on inner loops of core functions to make common case go fast
- Minimize misses in inner loops (repeated references to variables for temporal locality, stride-1 reference patterns for spatial locality)

### Matrix multiplication ex. for cache ops
