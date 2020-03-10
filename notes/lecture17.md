# Lecture 17
## Advanced Synchronization
---
### Review: Semaphores to protect shared resources via mutual exclusion
- Basic idea:   
  - Associate a unique semaphore *mutex*, initially 1, with each shared variable (or related set of shared vars)
  - Surround each access to the shared variables with P(mutex) and V(mutex) ops
- Semaphore: non-negative global int synchronization variable. manipulate by P/V ops
  - P(s)
    - If s is nonzero: decrement s by 1 and return immediately
    - If s is zero: suspend thread until s nonzero and thread is restarted by V
    - After restarting, P operation decrements s and returns control to caller
  - V(s)
    - increment s by 1
    - If any threads blocked in a P op waiting for s to become non-zero, then restart exactly one of those threads, which then completes its P operation by decrementing s

---
### Data Races
- Does main or peer thread execute first?
- Race test
  - If no race, each thread can get diff values
  - If there is a race, thread ordering can determine correctness

### Deadlock
- Get into a state where we can never make any progress - wait for a condition that will never be true
- Typically
  - Process 1 and 2 need two resources (A/B) to proceed
  - Process 1 acquires A and waits for B
  - Process 2 acquires B and waits for A
  - Both wait forever
- To avoid: have processes acquire locks in the same order

### Thread safety
- A function is thread-safe if it ALWAYS produces correct result when called repeatedly from multiple concurrent threads
  - i.e. no races when accessed concurrently

### Classes of thread-unsafe functions:
- 1: Failing to protect shared variables
  - Fix using P and V semaphore ops
- 2: Rely on persistent state across multiple function invocations
  - ex. RNG that relies on static state
  - Key property: pseudo-random property: random numbers are generated deterministically the next time the program runs
  - To make this RNG thread safe, pass state as a part of the argument
- 3: returning a pointer to a static variables
  - Fix: rewrite function so caller passes address of var to store result
  - Fix: Lock-and-copy: create a semaphore lock in that region and make a private copy of it then unlock it
- 4: Calling a thread-unsafe functions

### Reentrant functions
- if they access no shared variables when called by multiple threads
  - Thread safe, requires no synchronization ops

---
## Parallelization Paradigms
### Bulk Parallelization
- Bulk synchronous parallel
  - Sequential followed by parallel program phases
- Quickly start multiple threads, then synchronize
- posix: pthreads_create and pthreads_join

### Pthread barriers
- when we create threads: surround sequential region so we wait before we combine threads again from parallel to sequential

### Producer Consumer Problem
- Producer thread puts something into shared buffer, Consumer thread uses the data from shared buffer
- Can have multiple producers and consumers
- Issues?
  - Make sure thread doesn't produce or consume too fast
    - Have a semaphore for items/slots
      - Push something in (slots decrements)
        - If slots = 0 then we deschedule!
      - Consume items (decrement items)
        - If items = 0, deschedule!
  - Need a shared buffer data structure
    - Use mutex when inserting/removing to make sure that spot is available

### Readers-Writers Problem
- Generalization of mutual exclusion problem
- Problem statement:
  - Reader threads only read the object, Writer threads modify the object
  - Writers have exclusive access
  - Unlimited number of readers can access
- Can have first readers/writers: reader gets priority, second readers/writers: writer gets priority, starvation: possible in both cases (one overloads the other)

### Amdahl's Law
- Model performance of parallelizing a region of code
- Principle:
  - Decompose program into parallel and sequential parts
  - p = Fraction of program that is parallel
  - s = Speedup factor
- Resulting speedup:
  - Sequential time = T
  - Parallel Time: pT/s + (1-p)T [parallel + sequential]
  - Speedup: 1/(p/s + (1 - p))
  - Best possible speedup: 1/(1-p)
