/* 
 * mm.c -  Simple allocator based on implicit free lists, 
 *         first fit placement, and boundary tag coalescing. 
 *
 * Each block has header and footer of the form:
 * 
 *      31                     3  2  1  0 
 *      -----------------------------------
 *     | s  s  s  s  ... s  s  s  0  0  a/f
 *      ----------------------------------- 
 * 
 * where s are the meaningful size bits and a/f is set 
 * iff the block is allocated. The list has the following form:
 *
 * begin                                                          end
 * heap                                                           heap  
 *  -----------------------------------------------------------------   
 * |  pad   | hdr(8:a) | ftr(8:a) | zero or more usr blks | hdr(8:a) |
 *  -----------------------------------------------------------------
 *          |       prologue      |                       | epilogue |
 *          |         block       |                       | block    |
 *
 * The allocated prologue and epilogue blocks are overhead that
 * eliminate edge conditions during coalescing.
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "mm.h"
#include "memlib.h"
#include <stdint.h>

/* Your info */
team_t team = { 
    /* First and last name */
    "Aditya Pimplaskar",
    /* UID */
    "804900470",
    /* my message */
    "I have no idea how I am going to finish this lab (11/23/19 - 3:45 pm)"
}; 

/* $begin mallocmacros */
/* Basic constants and macros */
#define WSIZE       4       /* word size (bytes) */
#define DSIZE       8       /* doubleword size (bytes) */
#define CHUNKSIZE  (1<<16)  /* initial heap size (bytes) */
#define OVERHEAD    8       /* overhead of header and footer (bytes) */

#define MAX(x, y) ((x) > (y)? (x) : (y))  

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)       (*(uint32_t *)(p))
#define PUT(p, val)  (*(uint32_t *)(p) = (val))  

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
// should I have macros that set size/alloc? no, that is pack

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)  // subtract one word
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) //can use this to find pointer to a block from just the last

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/*
My explicit list free block layout
 [size/alloc - 4 bytes][next - 8 bytes][prev - 8 bytes][ junk + padding - 8 * n bytes - where coalesced blocks can go] [size/alloc - 4 bytes]
*/
#define FREEBLOCKSIZE       24 // space for header/footer/two pointers
#define BLK_NXTP(bp) ((char*)(bp)) /* to access part of block that holds next pointer */
#define BLK_PRVP(bp) ((char*)(bp) + DSIZE) /* to access part of block that holds previous pointer */
#define PRED(bp) (*(void **)(bp))
#define SUCC(bp) (*(void **)(bp+WSIZE))

#define PUT_NXT(bp, newp) (*(unsigned long *)(SUCC(bp)) = (unsigned long)(newp))
#define PUT_PREV(bp, newp) (*(unsigned long *)(PRED(bp)) = (unsigned long)(newp))
#define PUT_64VAL(ptr, value)(*(uint64_t *)(ptr) = value)

/* $end mallocmacros */

/* Global variables */
static char *heap_listp = NULL;  /* pointer to first block */
static char *heap_basefree = NULL; /* pointer to first unallocated block of exp free list -- this will be updated as we allocate, but we need it as a place to start our searches for blocks that will fit our data*/

// prof. uses a next fit rover ifdef NEXT_FIT (its a static char pointer) ?

/* function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);
static void printblock(void *bp); 
static void checkblock(void *bp);

// my internal helper functions
static void *find_fit_exp(size_t asize);
static void *exp_coalesce(void *bp);
static void exp_place(void *bp, size_t asize);
static void coalesce_helper(void *bp);
/* 
 * mm_init - Initialize the memory manager 
 */
/* $begin mminit */
int mm_init(void)
{
    /* create the initial empty heap */
    if ((heap_listp = mem_sbrk(4*WSIZE)) == NULL)
    //if ((heap_listp = mem_sbrk(8*WSIZE)) == NULL)
        //also want heap_basefree = mem_sbrk(4*WSIZE); (first block init as free)
        return -1;
    PUT(heap_listp, 0);                        /* alignment padding */
    
    heap_listp += WSIZE;d
    PUT(heap_listp, PACK(FREEBLOCKSIZE, 1));  /* prologue header */
    heap_listp+= WSIZE;
    printblock(heap_listp);
    PUT(heap_listp+(DSIZE*2), PACK(FREEBLOCKSIZE, 1));  /* prologue footer */
    
    PUT(heap_listp+(DSIZE*2)+WSIZE, PACK(0, 1));   /* epilogue header */
    
   // heap_listp += DSIZE;
    heap_basefree = heap_listp; // give me start of the free blocks
    PUT_64VAL(heap_listp, NULL); //next
    PUT_64VAL(heap_listp+DSIZE, NULL); //prev
    
    
    printf("init results prior to extending: \n");
    
    uint32_t hsize, halloc, fsize, falloc;
    
    hsize = GET_SIZE(HDRP(heap_listp));
    halloc = GET_ALLOC(HDRP(heap_listp));
    fsize = GET_SIZE(FTRP(heap_listp));
    falloc = GET_ALLOC(FTRP(heap_listp));
    
    printf("%p: header: [%d:%c] footer: [%d:%c], succ: %p, pred: %p \n", heap_listp,
           hsize, (halloc ? 'a' : 'f'),
           fsize, (falloc ? 'a' : 'f'),
           (void*)(SUCC(heap_listp)),
           (void*)(PRED(heap_listp)));
    
    
    
    
    //printblock(heap_listp);
    
    // prof: ifdef next fit, rover = heap_listp
    
    mm_checkheap(1); //check heap after init to see if it works
    
    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
    return 0;
}
/* $end mminit */

/* 
 * mm_malloc - Allocate a block with at least size bytes of payload 
 */
/* $begin mmmalloc */
void *mm_malloc(size_t size)
{
    printf("Entered malloc\n");
    uint32_t asize;      /* adjusted block size */
    uint32_t extendsize; /* amount to extend heap if no fit */
    char *bp;
    
    /* Ignore spurious requests */
    if (size <= 0)
        return NULL;
    
    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = DSIZE + OVERHEAD;
    else
        asize = DSIZE * ((size + (OVERHEAD) + (DSIZE-1)) / DSIZE); //for aligning to 8 bytes (rounding/exploiting int arith)
    
    /* Search the free list for a fit */
    printf("Call find fit exp\n");
    if ((bp = find_fit_exp(asize)) != NULL) {
        exp_place(bp, asize);
        return bp;
    }
    
    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize,CHUNKSIZE);
    printf("Call extend_heap\n");
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    exp_place(bp, asize);
    return bp;
}
//void *mm_malloc(size_t size)
//{
//    uint32_t asize;      /* adjusted block size */
//    uint32_t extendsize; /* amount to extend heap if no fit */
//    char *bp;
//
//    /* Ignore spurious requests */
//    if (size <= 0)
//    return NULL;
//
//    /* Adjust block size to include overhead and alignment reqs. */
//    if (size <= DSIZE)
//    asize = DSIZE + OVERHEAD;
//    else
//    asize = DSIZE * ((size + (OVERHEAD) + (DSIZE-1)) / DSIZE); //for aligning to 8 bytes (rounding/exploiting int arith)
//
//    /* Search the free list for a fit */
//    if ((bp = find_fit(asize)) != NULL) {
//    place(bp, asize);
//    return bp;
//    }
//
//    /* No fit found. Get more memory and place the block */
//    extendsize = MAX(asize,CHUNKSIZE);
//    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
//    return NULL;
//    place(bp, asize);
//    return bp;
//}
/* $end mmmalloc */

/* 
 * mm_free - Free a block 
 */
/* $begin mmfree */
void mm_free(void *bp)
{
    printf("Entering free\n");
    uint32_t size = GET_SIZE(HDRP(bp));
    
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    
    //tack on new block to start of exp list
    PUT_NXT(bp, heap_basefree);
    PUT_PREV(heap_basefree, bp);
    heap_basefree = bp;
    
    
    exp_coalesce(bp);
}
//void mm_free(void *bp)
//{
//    uint32_t size = GET_SIZE(HDRP(bp));
//
//    PUT(HDRP(bp), PACK(size, 0));
//    PUT(FTRP(bp), PACK(size, 0));
//    coalesce(bp);
//}

/* $end mmfree */

/*
 * mm_realloc - naive implementation of mm_realloc
 * DON'T NEED TO DO THIS -- NONE OF OUR TRACES USE REALLOC
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *newp;
    size_t copySize;

    if ((newp = mm_malloc(size)) == NULL) {
	printf("ERROR: mm_malloc failed in mm_realloc\n");
	exit(1);
    }
    copySize = GET_SIZE(HDRP(ptr));
    if (size < copySize)
      copySize = size;
    memcpy(newp, ptr, copySize);
    mm_free(ptr);
    return newp;
}

/* 
 * mm_checkheap - Check the heap for consistency
 * iterates through blocks in heap and makes sure certain invariants are true (header == footer, iteration matches size of heap, etc.)
 * Should add to this function as we add more and more features to our allocator -- better than debugging, we get incremental check of whether or not our allocator works
 * sprinkle calls to checkheap around my code (as a proxy for a breakpoint)
 */
void mm_checkheap(int verbose) 
{
    char *bp = heap_listp;

    if (verbose)
	printf("Heap (%p):\n", heap_listp);

    //if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
     if ((GET_SIZE(HDRP(heap_listp)) != 24) || !GET_ALLOC(HDRP(heap_listp)))
	printf("Bad prologue header\n");
    checkblock(heap_listp);

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
	if (verbose) 
	    printblock(bp);
	checkblock(bp);
    }
     
    if (verbose)
	printblock(bp);
    if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
	printf("Bad epilogue header\n");
}

/* The remaining routines are internal helper routines */

/* 
 * extend_heap - Extend heap with free block and return its block pointer
 */
/* $begin mmextendheap */
static void *extend_heap(size_t words) 
{
    printf("Calling extend heap\n");
    char *bp;
    uint32_t size;
	
    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if (size < FREEBLOCKSIZE){ size = FREEBLOCKSIZE; }
    
    // This says size = if words is odd: size = words +1 * WSIZE, else size = words * WSIZE
    if ((bp = mem_sbrk(size)) == (void *)-1) 
	return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* free block header */
    //need to add in the next and prev pointers for this extended block
//    PUT_64VAL(bp, NULL); //next
//    PUT_64VAL(bp+DSIZE, NULL); //prev
    PUT(FTRP(bp), PACK(size, 0));         /* free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* new epilogue header */

    /* Coalesce if the previous block was free */
    //return coalesce(bp);
    printf("returning after coalescing\n");
    return exp_coalesce(bp);
}
/* $end mmextendheap */

/* 
 * place - Place block of asize bytes at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
/* $begin mmplace */
/* $begin mmplace-proto */

static void exp_place(void *bp, size_t asize){
    printf("Place called...\n");
    uint32_t csize = GET_SIZE(HDRP(bp));
    
     //ALSO NEEDS TO ACCOUNT FOR POINTER MODIFICATIONS
    //next pointer of block before bp needs to become bp's next pointer
    //prev pointer of block after bp needs to become bp's prev pointer
    char* prev = BLK_PRVP(bp);
    char* next = BLK_NXTP(bp);
    PUT_NXT(PREV_BLKP(bp), next);
    PUT_PREV(NEXT_BLKP(bp), prev);
    
    if ((csize - asize) >= (FREEBLOCKSIZE)) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        coalesce_helper(bp);
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize-asize, 0));
        PUT(FTRP(bp), PACK(csize-asize, 0));
        exp_coalesce(bp);
    }
    else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
        coalesce_helper(bp);
    }
   
}

static void place(void *bp, size_t asize)
/* $end mmplace-proto */
{
    uint32_t csize = GET_SIZE(HDRP(bp));   

    if ((csize - asize) >= (DSIZE + OVERHEAD)) { 
	PUT(HDRP(bp), PACK(asize, 1));
	PUT(FTRP(bp), PACK(asize, 1));
	bp = NEXT_BLKP(bp);
	PUT(HDRP(bp), PACK(csize-asize, 0));
	PUT(FTRP(bp), PACK(csize-asize, 0));
    }
    else { 
	PUT(HDRP(bp), PACK(csize, 1));
	PUT(FTRP(bp), PACK(csize, 1));
    }
}
/* $end mmplace */

/* 
 * find_fit - Find a fit for a block with asize bytes 
 */
static void *find_fit_exp(size_t asize){
    printf("Made it into find fit exp\n");
    //now we can implement find fit for explicit list
    void *bp;
    
//    bp = heap_basefree; /* now we want to jump by pointer */
//    if(asize <= GET_SIZE(HDRP(bp))){
//        printf("The first block is being allocated\n");
//        printf(bp, "\n");
//        heap_basefree = ACC_NEXT(bp);
//        printf(heap_basefree, "\n");
//        return bp;
//    }
//
    
    for (bp = heap_basefree; !GET_ALLOC(HDRP(heap_basefree)); bp = SUCC(bp)){
        printf("Entered find fit loop\n");
        // I am infinite looping in here
        if(asize <= GET_SIZE(HDRP(bp))){
        /*if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {*/
            return bp;
        }
    }
    printf("Didn't find a fit\n");
    return NULL; /* no fit */
}

static void *find_fit(size_t asize)
{
    //want to check if our base block works
    //if it works, then we want to push the base of our explicit free list to the NEXT block
    /* first fit search */
    
    void *bp;

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
	if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
	    return bp;
	}
    }
    return NULL; /* no fit */
}

/*
 * coalesce - boundary tag coalescing. Return ptr to coalesced block
 */

static void coalesce_helper(void *bp){ // to handle pointer modifications
    printf("Calling coalesce helper\n");
    if(bp != NULL){
        printf("bp is not null in coalesce helper\n");
        if(PRED(bp) != NULL){
            printf("pred bp is not null in coalesce helper\n");
            SUCC(PRED(bp)) = SUCC(bp);
        }
        else{
            printf("pred bp IS null in coalesce helper, i.e bp is the first block\n");
            heap_basefree = SUCC(bp);
        }
        //SO SUCC BP IS NOT INITIALIZING AS NULL!
        if(SUCC(bp) != NULL){
            printf("succ bp is not null in coalesce helper\n");
//            void* temp = PRED(bp);
//            void* temp2 = PRED(SUCC(bp));
            printf("is it predsucc bp?\n");
            PUT_64VAL(PRED(SUCC(bp)),PRED(bp));
            printf("am I segfaulting in the coalesce helper?\n"); //YES
        }
    }
    printf("leaving coalesce helper\n");
}


static void *coalesce(void *bp) 
{
    uint32_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    uint32_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    uint32_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {            /* Case 1 */
	return bp;
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */
	size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size,0));
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */
	size += GET_SIZE(HDRP(PREV_BLKP(bp)));
	PUT(FTRP(bp), PACK(size, 0));
	PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
	bp = PREV_BLKP(bp);
    }

    else {                                     /* Case 4 */
	size += GET_SIZE(HDRP(PREV_BLKP(bp))) + 
	    GET_SIZE(FTRP(NEXT_BLKP(bp)));
	PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
	PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
	bp = PREV_BLKP(bp);
    }

    return bp;
}


static void *exp_coalesce(void *bp){

    mm_checkheap(1);
    printf("called coalesce\n");
    uint32_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    if(bp == NULL){ prev_alloc = 1;}
    uint32_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    uint32_t size = GET_SIZE(HDRP(bp));
    printf("coalesce before cases\n");
    
    if (prev_alloc && next_alloc) {            /* Case 1: Both flanking blocks alloc */
        printf("CASE 1 of COALESCING\n");
        
        printf("put bp at prev of old free base\n");
        PUT_64VAL(PRED(heap_basefree), bp);
        
        printf("put bp as next of old free base\n");
        PUT_64VAL(SUCC(bp), heap_basefree);
        
        printf("put bp's prev as null\n");
        PUT_64VAL(PRED(bp), NULL);
        
        printf("put bp as new free base\n");
        heap_basefree = bp;
        
        printf("Returning from coalesce\n");
        return bp;
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2: Next block is free */
        printf("CASE 2 of COALESCING\n");
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        coalesce_helper(NEXT_BLKP(bp));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size,0));
    }
    
    else if (!prev_alloc && next_alloc) {      /* Case 3: Previous block is free */
        printf("CASE 3 of COALESCING\n");
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        //pointer mods
        printf("about to reset bp\n");
        bp = PREV_BLKP(bp);
        if(bp == NULL){ printf("bp is null now!\n");}
        printf("set bp to prev block\n");
        coalesce_helper(bp);
        printf("Putting HDRP\n");
        PUT(HDRP(bp), PACK(size, 0));
        printf("Putting FTRP \n");
        PUT(FTRP(bp), PACK(size, 0));
    }

    else {                                     /* Case 4: Both flanking blocks free */
        printf("CASE 4 of COALESCING\n");
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
        GET_SIZE(FTRP(NEXT_BLKP(bp)));
        //pointer mods
        coalesce_helper(PREV_BLKP(bp));
        coalesce_helper(NEXT_BLKP(bp));
        bp = PREV_BLKP(bp);
        
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }

    
    mm_checkheap(1);
    
    //need to put the coalesced block somewhere, we can insert at the front
    
//    if(PRED(heap_basefree) == NULL){
//        printf("pred of heap base free is NULL");
//    }
    if((uint64_t)(*heap_basefree) == 0){
        printf("succ of heap base free is NULL");
    }
    
    // #define PRED(bp) (*(char **)(bp))
    printf("heap_basefree is %d\n", (char**)heap_basefree);
    printf("pred is %d\n", *(char**)heap_basefree);
    
    printf("put bp at prev of old free base\n");
    PUT_64VAL(PRED(heap_basefree), bp);
    //PRED(heap_basefree) = bp;
   
    printf("put bp as next of old free base\n");
    PUT_64VAL(SUCC(bp), heap_basefree);
    
    //SUCC(bp) = heap_basefree;
    printf("put bp's prev as null\n");
    PUT_64VAL(PRED(bp), NULL);
    //PRED(bp) = NULL;
    printf("put bp as new free base\n");
    //PUT_64VAL(SUCC(bp), heap_basefree);
    heap_basefree = bp;
    
    printf("Returning from coalesce\n");
    return bp;

}



static void printblock(void *bp) 
{
    uint32_t hsize, halloc, fsize, falloc;

    hsize = GET_SIZE(HDRP(bp));
    halloc = GET_ALLOC(HDRP(bp));  
    fsize = GET_SIZE(FTRP(bp));
    falloc = GET_ALLOC(FTRP(bp));  
    
    if (hsize == 0) {
	printf("%p: EOL\n", bp);
	return;
    }

    printf("%p: header: [%d:%c] footer: [%d:%c], succ: %p, pred: %p \n", bp,
	   hsize, (halloc ? 'a' : 'f'), 
	   fsize, (falloc ? 'a' : 'f'),
       (void*)(SUCC(bp)),
       (void*)(PRED(bp)));
}

static void checkblock(void *bp) 
{
    if ((uint32_t)bp % 8)
	printf("Error: %p is not doubleword aligned\n", bp);
    if (GET(HDRP(bp)) != GET(FTRP(bp)))
	printf("Error: header does not match footer\n");
}

