#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef NDEBUG
typedef struct debugStruct {
    size_t allocatedChunks;
} DebugStruct;
#endif

typedef struct poolAllocNode PoolAllocNode;
typedef struct poolAllocNode{
    PoolAllocNode * restrict next; 
} PoolAllocNode;

typedef struct poolalloc PoolAlloc;
// Pool allocator
typedef struct poolalloc {

    // size of the memory in this allocator
    size_t totalSize;

    // size of each chunk
    size_t chunkSize;
    
    // information for debugging
#ifndef NDEBUG
    DebugStruct dbgS;
#endif 
    // points to the first node in the free list
    PoolAllocNode * restrict head;

    // the pointer to the buffer held by this allocator
    uint8_t * restrict buffptr;
} PoolAlloc;



// function to create a pool allocator
extern PoolAlloc* alloc_createPoolAllocator(size_t size, size_t chunkAlignment, size_t chunkSize);

// function to allocate chunks
extern void * alloc_poolAllocAllocate(PoolAlloc * restrict pa);

// free all the allocated chunk in allocator
// doesn't deallocate the memory allocated by the createPoolAllocator function
extern void alloc_poolAllocFreeAll(PoolAlloc * restrict pa); 

// free the chunk given by ptr 
// ptr should be allocate by using poolAllocAllocate function
extern void alloc_poolAllocFree(PoolAlloc * restrict pa, void * restrict ptr);

// free pool allocator
extern void alloc_freePoolAllocator(PoolAlloc * restrict pAlloc);


/*
 *
 *
 * Static linear allocator that can allocate only one chunk size of the alignment
 *
 * */
typedef struct linearAllocatorFC {
    uint8_t * bufptr;
    size_t totalSize;
    size_t curOffset; 
    size_t chunkSize;
    size_t alignment;
} LinearAllocFC;


// create a linearAllocator
extern LinearAllocFC * alloc_createLinearAllocFC(size_t numChunks, 
                                                 size_t chunkSize, 
                                                 size_t chunkAlignment);

// allocate memory
extern void * alloc_linearAllocFCAllocate(LinearAllocFC * restrict lafc);

// free all
extern void alloc_linearAllocFCFreeAll(LinearAllocFC * restrict lafc);

// destroy the linear allocator
extern void alloc_freeLinearAllocFC(LinearAllocFC * restrict lafc);

/* 
 *
 * Static stack allocator
 *
 * */

typedef struct stackAllocHeader{
    size_t padding;
    size_t prevOffset;
} StackAllocHeader;

// Static stack allocator
typedef struct stackAlloc {
    bool isFull;

    // pointer to the buffer
    uint8_t * buffptr;    

    // size of the buffer
    size_t totalSize;

    // offset of the memory that will be 
    // allocated next
    size_t offset;

    // value of offset before previous 
    // allocation 
    size_t prevOffset;
} StackAlloc;

// create stack allocator
extern StackAlloc* alloc_createStackAllocator(size_t size);

// allocate memory 
extern void * alloc_stackAllocAllocate(StackAlloc * restrict sa, size_t allocSize, size_t alignment);

// free the most recent allocation
extern bool alloc_stackAllocFree(StackAlloc * restrict sa, void * ptr);

// free all the allocations
extern void alloc_stackAllocFreeAll(StackAlloc * restrict sa);

// destroy the stack allocator
extern void alloc_freeStackAllocator(StackAlloc * sa);

/* 
 *
 * Dynamic stack allocator
 *
 * */

typedef struct ptrStack {
    size_t curOffset; 
    size_t maxPointers;
    void ** bufptr;
    bool valid;
} PtrStack;

extern void alloc_createPtrStack(PtrStack * restrict ps, size_t maxPointers);
extern bool alloc_ptrStackPush(PtrStack * restrict ps, void * val);
extern bool alloc_ptrStackPop(PtrStack * restrict ps, void ** restrict out);
extern void alloc_freePtrStack(PtrStack * restrict ptr);

typedef struct dynamicStackAlloc {
    PtrStack ps;
    size_t allocatorSize;

    size_t numAllocatedStacks;

    // is the allocator full 
    bool isMax;
    bool valid;
} DynamicStackAlloc;

extern DynamicStackAlloc * alloc_createDynamicStackAllocD(
        size_t maxAllocatorSize,
        size_t maxAllocators);


extern void alloc_createDynamicStackAlloc(
        DynamicStackAlloc * restrict dsa, 
        size_t maxAllocatorSize,
        size_t maxAllocators);

extern void* alloc_dynamicStackAllocAllocate(
        DynamicStackAlloc * restrict dsa,
        size_t allocSize,
        size_t alignment);

extern bool alloc_dynamicStackAllocFree(DynamicStackAlloc * restrict dsa, void * ptr);
extern bool alloc_dynamicStackAllocFreeAll(DynamicStackAlloc * restrict dsa);
extern void alloc_freeDynamicStackAlloc(DynamicStackAlloc * restrict dsa);
extern void alloc_freeDynamicStackAllocD(DynamicStackAlloc * restrict dsa);

#endif

