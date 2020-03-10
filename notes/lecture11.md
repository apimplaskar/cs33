# Lecture 11: Memory Hierarchy
## Nov 5, 2019
---
### Three fundamental optimizations
- Pipelining: multiple instructions through pipeline at same time, increase freq/throughput of processor
- Superscalar: execute more than one instruction (per stage) at a time -- INSTRUCTION LEVEL PARALLELISM
- Out of order execution: reorder instructions to execute past long-latency instructions, out of program order
- Have to respect dependencies in the prorgam

### Dependence graphs
- instructions mapped out between registers
- create dynamic dependence graph -- longest path = critical path
  - for loops: multiply by number of iterations
- see slides for example

### Vector case study
- see slides

### Haswell CPU
- 8 total functional units
- Multiple instructions can execute in parallel
- See slides for functional units and latencies/cycles per issue for each instruction
  - Cycles per issue tells you if it is pipelined or not
  - i.e cycles/issue = 1 -- IT IS PIPELINED

### Loop unrolling -- 1
```
#old
for(i = 0; i < limit; i++){
  x = x OP d[i];
}

#new
for(i = 0; i < limit ; i+=2){
  x = (x OP d[i]) OP d[i+1];
}
```
- Have to be wary of whether or not this has effects on the actual computation (for instance, here it works for ints but may throw off floats)
- In this case, we increase the parallelism of the computation
- Can change latency bound since dependence height changes

### Throughput bound
- Best speed of a certain program, determined by resource count and assuming no dependencies
- Consider throughput bound in the context of CPE
- max over each resource((Cycles/Issue) * Ops Required) / (# of resources)

### Loop unrolling -- 2
- Eliminate dependencies with separate accumulators
```
for (i = 0; i < limit; i+=2){
  x0 = x0 OP [i];
  x1 = x1 OP [i+1];
}
x = x0 OP x1;
```

### SIMD instructions: single instruction multiple data
- Can vectorize computations

---
## MEMORY HIERARCHY
### Big lessons:
- Many kinds of memory (reg. files, SRAM, DRAM, Flash, Disk)
- Faster memories are smaller (faster memories are "more expensive", smaller is faster by design)
- [SLOW] Remote secondary storage -  Local secondary storage (file storage) - Main memory (DRAM) - Caches - Registers (SRAM) [FAST]

### Random access memory (RAM)
- Basic storage unit is normally a cell (one bit per cell)
- Multiple RAM chips form a memory
- Two varieties -- static (SRAM) and dynamic (DRAM)
- Static RAM: 4-6 transistors, ~1 ns access, ~100x cost, on-chip caches
- Dynamic RAM: 1 transistor, 10-100 ns access, ~1x cost, main memory (large)
- Value storage dependent on power (voltage)

### Flash memory:
- nonvolatile, can use for long-term file storage (SSDs)
- Can read/write data in units of pages (512KB - 4KB, blocks 32-128 pages)
- Wear out: can wear after ~ 100,000 repeated writes

### SSD performance characteristics
- Bandwidth in between that of DRAM and of spinning disks
- Sequential access is faster than random access (common theme in memory hierarchy)
- Random writes are slower

### Logical Disk blocks
- modern disks present a simpler abstract view of complex sector geometry
  - set of available sectors is modeled as sequence of b-sized logical blocks
- Mapping between logical blocks and physical locations (maintained by disk controller)

### Disk drive
- arm touches platters that surround spindle
- Disk consists of platters each with two surfaces, each surface has concentric tracks, tracks have sectors separated by gaps
- Disk spins at fixed rotational rate -- wait for read/write sector to rotate around and can perform operation

### Traditional Bus Structure Connecting CPU and Memory
- Bus = collection of parallel wires that carry address, data, and control signals
- I/O bridge between chip and DIMM
  - system bus from CPU to I/O and memory bus from I/O to DIMM
- I/O bus: connects to USB controllers, graphics adapters, disk controllers, network adapters, etc.

### Reading a disk sector
- CPU initiates disk read by writing a command, logical block number, and destination memory address to a port (address) associated with disk controller
- Disk performs direct memory access (DMA) into main memory

### Locality
- How do we manage data so that we are fast and also have large memory?
- Principle of locality: programs tend to use data/instructions with addresses near or equal to those they have used recently
- Temporal locality and spatial locality
