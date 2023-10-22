#include "allocator.h"

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdalign.h>

#define ALIGNED_MALLOC(alignment, size) aligned_alloc((alignment), (size))
#define CUSTOM_MALLOC(size) malloc((size))

static bool isPowerOfTwoUPTR(uintptr_t align){
    return (align & (align - 1)) == 0;
}

static bool isPowerOfTwoUINT16(uint16_t align){
    return (align & (align - 1)) == 0;
}

#define isPowerOfTwo(align) \
    _Generic((align), \
            uintptr_t: isPowerOfTwoUPTR((align)), \
            uint16_t: isPowerOfTwoUINT16((align)) \
            )

static uint32_t alignSizeForward(size_t size, size_t align){
    assert(isPowerOfTwo(align) && "align is not a power of two");
    
    uint16_t modulo = size & (align - 1);

    if(modulo != 0){
        size += align - modulo;
    }

    return size;
}

PoolAlloc* alloc_createPoolAllocator(size_t size, size_t chunkAlignment, size_t chunkSize){
    assert(isPowerOfTwo(chunkAlignment) && "align is not a power of two");
    
    size_t chunkCount = size / chunkSize;

    if(chunkAlignment < alignof(PoolAllocNode)){
        chunkAlignment = alignof(PoolAllocNode);
    }
    
    // align the size 
    size = alignSizeForward(size, chunkAlignment);
    chunkSize = alignSizeForward(chunkSize, chunkAlignment);
    
    size_t diff = chunkCount - size/chunkSize;
    if(diff > 0){
        size += diff * chunkSize;
    }

    assert(chunkSize >= sizeof(PoolAllocNode) && "chunk size less than header size");
    assert(size >= chunkSize && "total size is less than chunk size");

    PoolAlloc* pa = (PoolAlloc *) CUSTOM_MALLOC(sizeof(PoolAlloc));
    
    pa->totalSize = size;
    pa->chunkSize = chunkSize;
    pa->buffptr = ALIGNED_MALLOC(chunkAlignment, size);
    pa->head = NULL;
    
    // create the free list
    alloc_poolAllocFreeAll(pa);
    
    return pa;
}

void * alloc_poolAllocAllocate(PoolAlloc * restrict pa){
    PoolAllocNode * node = pa->head;
    if(node == NULL){

#ifndef NDEBUG
        printf("%zu\n", pa->dbgS.allocatedChunks);
#endif
        assert(0 && "Pool allocator is empty");
        return NULL;
    }

    pa->head = pa->head->next;

#ifndef NDEBUG
    pa->dbgS.allocatedChunks += 1;
#endif
    // zero the memory and return it 
    return memset(node, 0, pa->chunkSize);
}

void alloc_poolAllocFree(PoolAlloc * restrict pa, void * restrict ptr){
    if(ptr == NULL){
        return;
    }
    
    void * start = pa->buffptr;
    void * end = &pa->buffptr[pa->totalSize];
    
    if(!(start <= ptr && ptr < end)){
        assert(0 && "Invalid memory");
        return;
    }

    PoolAllocNode * node = (PoolAllocNode *) ptr;
    node->next = pa->head;
    pa->head = node;

#ifndef NDEBUG
    pa->dbgS.allocatedChunks -= 1;
#endif
}

void alloc_poolAllocFreeAll(PoolAlloc * restrict pa){
    size_t chunkCount = pa->totalSize / pa->chunkSize;

    PoolAllocNode * node = NULL;
    for(uint32_t i = 0; i < chunkCount; i++){
        node = (PoolAllocNode *) &pa->buffptr[i * pa->chunkSize];
        node->next = pa->head;
        pa->head = node;
    }

#ifndef NDEBUG
    pa->dbgS.allocatedChunks = 0;
#endif
} 

extern void alloc_freePoolAllocator(PoolAlloc * restrict pAlloc){
    free(pAlloc->buffptr);
    free(pAlloc);
}

/*
 *
 *
 * Static linear allocator that can allocate only one chunk size of the alignment
 *
 * */


LinearAllocFC * alloc_createLinearAllocFC(size_t numChunks, 
                                          size_t chunkSize, 
                                          size_t chunkAlignment){

    assert(isPowerOfTwo(chunkAlignment));
    chunkSize = alignSizeForward(chunkSize, chunkAlignment);

    LinearAllocFC * outLAFC = CUSTOM_MALLOC(sizeof(LinearAllocFC));
    outLAFC->totalSize = numChunks * chunkSize;
    outLAFC->alignment = chunkAlignment;
    outLAFC->curOffset = 0;
    outLAFC->chunkSize = chunkSize;
    outLAFC->bufptr = ALIGNED_MALLOC(chunkAlignment, outLAFC->totalSize);

    return outLAFC;
}


void * alloc_linearAllocFCAllocate(LinearAllocFC * lafc){
    if((lafc->curOffset + lafc->chunkSize) > lafc->totalSize){
        assert(0 && "Linear allocator is full");
        return NULL;
    }

    void * outAddr = lafc->bufptr + lafc->curOffset;
    lafc->curOffset += lafc->chunkSize;
    
    return memset(outAddr, 0, lafc->chunkSize);
}

void alloc_linearAllocFCFreeAll(LinearAllocFC * restrict lafc){
    lafc->curOffset = 0;
}

void alloc_freeLinearAllocFC(LinearAllocFC * restrict lafc){
    free(lafc->bufptr);
    free(lafc);
}



/*
 *
 * Static stack allocator
 *
 * */

// assumes that alignment is a power of two
static size_t paddingForHeader(uintptr_t curAddr, uint16_t alignment, size_t headerSize){ 
    size_t modulo = curAddr & (alignment - 1);
    size_t padding = 0;
    if(modulo != 0){
        padding = alignment - modulo;
    }
    
    if(padding < headerSize){
        headerSize -= padding;

        if((headerSize & (alignment - 1)) != 0){
            padding += alignment * (1 + (headerSize/alignment));
        }else{
            padding += headerSize;
        }
    }

    return padding;
}


// Create stack allocator
StackAlloc* alloc_createStackAllocator(size_t size){
    if(size == 0){
        return NULL;
    }

    StackAlloc * outSA = CUSTOM_MALLOC(sizeof(StackAlloc)); 

    if(outSA == NULL){
        return NULL;
    }

    outSA->buffptr = CUSTOM_MALLOC(size);

    if(outSA->buffptr == NULL){
        free(outSA);
        return NULL;
    }

    outSA->totalSize = size;
    outSA->offset = outSA->prevOffset = 0;
    outSA->isFull = false;
    
    return outSA;
}

void * alloc_stackAllocAllocate(StackAlloc * restrict sa, size_t allocSize, size_t alignment){
    if(!isPowerOfTwo(alignment)){
        return NULL;
    }

    if(alignment < alignof(StackAllocHeader)){
        alignment = alignof(StackAllocHeader);
    }

    uintptr_t curAddr = (uintptr_t)(sa->buffptr + sa->offset);

    size_t padding = paddingForHeader(curAddr, alignment, sizeof(StackAllocHeader));

    if(sa->offset + padding + allocSize > sa->totalSize){
        sa->isFull = true;
        return NULL;
    }

    uintptr_t nextAddr = curAddr + padding;
    
    StackAllocHeader * header = (StackAllocHeader *)(nextAddr - sizeof(StackAllocHeader)); 
    header->padding = padding;
    header->prevOffset = sa->prevOffset;

    sa->prevOffset = sa->offset; 
    sa->offset += padding + allocSize;
    
    if(sa->offset >= sa->totalSize){
        sa->isFull = true;
    }

    return memset((void *)nextAddr, 0, allocSize);
}

bool alloc_stackAllocFree(StackAlloc * restrict sa, void * ptr){
    uintptr_t start = (uintptr_t)sa->buffptr;
    uintptr_t end = start + sa->totalSize;
    uintptr_t curAddr = (uintptr_t)ptr; 
    if(!(start <= curAddr && curAddr < end)){
        return false;
    }


    if(curAddr > (start + sa->offset)){
        return true;
    }
    
    StackAllocHeader * header = (StackAllocHeader *)(curAddr - sizeof(StackAllocHeader));
    size_t prevOffset = curAddr - header->padding - start;

    if(prevOffset != sa->prevOffset){
        return false;
    }

    sa->offset = sa->prevOffset;
    sa->prevOffset = header->prevOffset;  

    return true;
}

void alloc_stackAllocFreeAll(StackAlloc * restrict sa){
    sa->offset = sa->prevOffset = 0;
    sa->isFull = false;
}

// destroy the stack allocator
void alloc_freeStackAllocator(StackAlloc * sa){
    free(sa->buffptr);
    free(sa);
}
/*
 *
 * Dynamic stack allocator
 *
 * */
void alloc_createPtrStack(PtrStack * restrict ps, size_t maxPointers){
    ps->bufptr = CUSTOM_MALLOC(maxPointers * sizeof(void *));
    
    if(ps->bufptr == NULL){
        ps->valid = false;
        return;
    }

    ps->valid = true;
    ps->maxPointers = maxPointers;
    ps->curOffset = 0;
}

bool alloc_ptrStackPush(PtrStack * restrict ps, void * val){
    if(!ps->valid) { 
        return false;
    }
    
    if(ps->curOffset >= ps->maxPointers){
        return false;
    }
    
    ps->bufptr[ps->curOffset] = val;
    ps->curOffset += 1;

    return true;
}

bool alloc_ptrStackPop(PtrStack * restrict ps, void ** restrict out){
    if(!ps->valid){
        return false;
    }
    
    if(!ps->curOffset){
        return false;
    }
    
    ps->curOffset -= 1;
    *out = ps->bufptr[ps->curOffset];

    return true;
}

void alloc_freePtrStack(PtrStack * restrict ptr){
    free(ptr->bufptr);
}

#define PTR_STACK_TOP_ELEMENT(ptrs) (ptrs).bufptr[(ptrs).curOffset - 1]
#define DSA_IS_ON_TOP(dsa) ((dsa)->numAllocatedStacks == (dsa)->ps.curOffset)

void alloc_createDynamicStackAlloc(
        DynamicStackAlloc * restrict dsa, 
        size_t maxAllocatorSize,
        size_t maxAllocators){

    if(!maxAllocatorSize){
        dsa->valid = false;
        return;
    }

    alloc_createPtrStack(&dsa->ps, maxAllocators);
    dsa->allocatorSize = maxAllocatorSize;
    dsa->isMax = false;
    dsa->valid = dsa->ps.valid;
    dsa->numAllocatedStacks = 0;
}

DynamicStackAlloc * alloc_createDynamicStackAllocD(
        size_t maxAllocatorSize,
        size_t maxAllocators){

    DynamicStackAlloc * dsa = CUSTOM_MALLOC(sizeof(DynamicStackAlloc));
    if(dsa == NULL){
        return NULL;
    }

    alloc_createDynamicStackAlloc(dsa, maxAllocatorSize, maxAllocators);
    return dsa;
}

static bool addStackAlloctor(DynamicStackAlloc * restrict dsa){
    if(dsa->isMax){
        return false;
    }

    StackAlloc * sa = alloc_createStackAllocator(dsa->allocatorSize); 

    if(sa == NULL){
        return false;
    }

    if(!alloc_ptrStackPush(&dsa->ps, sa)){
        return false;
    }
    
    dsa->isMax = dsa->ps.curOffset >= dsa->ps.maxPointers;
    dsa->numAllocatedStacks += 1;
    return true;
}

void* alloc_dynamicStackAllocAllocate(
        DynamicStackAlloc * restrict dsa,
        size_t allocSize,
        size_t alignment){
   
    if(!dsa->valid || !dsa->ps.valid){
        return NULL;
    }

    StackAlloc * sa;
    if(dsa->numAllocatedStacks == 0){
        if(!addStackAlloctor(dsa)){
            return NULL;
        }
        sa = PTR_STACK_TOP_ELEMENT(dsa->ps);
    }else{
        sa = PTR_STACK_TOP_ELEMENT(dsa->ps);

        if(sa->isFull){
            if(!DSA_IS_ON_TOP(dsa)){
                dsa->ps.curOffset += 1;
            }else if(!addStackAlloctor(dsa)) {
                return NULL;
            }
            
            sa = PTR_STACK_TOP_ELEMENT(dsa->ps);
        }  
    }
    void * v = alloc_stackAllocAllocate(sa, allocSize, alignment);
    
    if(v == NULL && sa->isFull){
        if(!addStackAlloctor(dsa)){
            return NULL;
        }

        sa = PTR_STACK_TOP_ELEMENT(dsa->ps);
        return alloc_stackAllocAllocate(sa, allocSize, alignment);
    }else{
        return v;
    }
}

bool alloc_dynamicStackAllocFree(DynamicStackAlloc * restrict dsa, void * ptr){
    if(!dsa->valid || !dsa->ps.valid || !dsa->ps.curOffset){
        return false;
    } 
    
    StackAlloc * sa = PTR_STACK_TOP_ELEMENT(dsa->ps);

    if(sa->offset == 0){
        if(dsa->ps.curOffset == 1){
            return false;
        }else{
            dsa->ps.curOffset -= 1;
        }
    }

    sa = PTR_STACK_TOP_ELEMENT(dsa->ps);

    return alloc_stackAllocFree(sa, ptr);
}


bool alloc_dynamicStackAllocFreeAll(DynamicStackAlloc * restrict dsa){
    if(!dsa->valid || !dsa->ps.valid || !dsa->ps.curOffset){
        return false;
    } 
    
    StackAlloc * v;
    for(size_t i = 0; i < dsa->ps.curOffset; i++){
        v = dsa->ps.bufptr[i];
        alloc_stackAllocFreeAll(v);
    }
    dsa->ps.curOffset = dsa->ps.curOffset != 0;
    
    return true;
 }

void alloc_freeDynamicStackAlloc(DynamicStackAlloc * restrict dsa){
    void * v = NULL;
    while(dsa->ps.curOffset){
        alloc_ptrStackPop(&dsa->ps, &v); 
        alloc_freeStackAllocator((StackAlloc *)v);    
    }

    free(dsa->ps.bufptr);
}

void alloc_freeDynamicStackAllocD(DynamicStackAlloc * restrict dsa){
    alloc_freeDynamicStackAlloc(dsa);
    free(dsa);
}

#undef PTR_STACK_TOP_ELEMENT
#undef DSA_IS_ON_TOP
