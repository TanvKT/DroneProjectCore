/**
 * @file myRTOS_lock_heap.c
 * @author your name (you@domain.com)
 * @brief Dynamic memory needed for locking implementation
 *              Uses same functionality as user heap, but without locks as global interrupts will be disabled
 * 
 *        Since the functionality of this heap is the same as the user heap, we only test the user heap in the unit tests
 * 
 * @version 0.1
 * @date 2025-12-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "myRTOS.h"
#include "myRTOS_memory.h"
#include "myRTOS_heap.h"
#include "myRTOS_lock_heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 

/**
 * @brief Globals
 * 
 */
static myrtos_block_header_s* top;
static myrtos_block_header_s* end;

/**
 * @brief Zeroizes entire heap
 * 
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_zeroize_lock_heap()
{
    uint8_t* heap = (uint8_t*)myrtos_get_lock_heap_bp();
    if (NULL == heap) return MYRTOS_MEMORY_INVALID;
    //could just use memset for this, but compiler will optimize this the same way
    for (size_t i = 0; i < MYRTOS_LOCK_HEAP_SIZE; i++)
    {
        heap[i] = 0;
    }

    return MYRTOS_SUCCESS;
}

/**
 * @brief heap initialization function
 * 
 * Reserves fixed amount of memory in bytes using c stdlib malloc
 * Allocates global variables to keep track of heap allocation
 * 
 * @param s size of heap in bytes
 * 
 * @return myRTOS_return_type_e
*/
myRTOS_return_type_e myrtos_lock_heap_init()
{
    //using malloc to allocate static array for our heap
    //end is assigned as the last block of our heap
    //////////////////////////////////////////////////////////////////////////////////////////////
    //          |                                                                   |           //
    // TOP      |                             free block                            |       END //
    //          |                                                                   |           //
    //////////////////////////////////////////////////////////////////////////////////////////////
    myRTOS_return_type_e ret;

    //need to ensure heap is zeroized before setting top and end blocks
    ret = myrtos_zeroize_lock_heap();
    if (ret != MYRTOS_SUCCESS) return ret;

    top = (myrtos_block_header_s*)myrtos_get_lock_heap_bp();
    if (!top) return MYRTOS_MEMORY_INVALID;
    end = (myrtos_block_header_s*)((void*)top + MYRTOS_LOCK_HEAP_SIZE - MYRTOS_HEADER_SIZE);

    top->next = end;
    end->next = NULL;
    top->prev = NULL;
    end->prev = top;

    return ret;
}

/**
 * @brief heap allocation function
 * 
 * Doing a greedy search for free blocks
 *      - start at top block and search through to end for free block of requested size
 * Thread safe operation, if another task is asking for an allocation the requesting task will have to wait
 * 
 * @param s size of block requested in bytes
 * @return void* pointer to memory given or null if no space
 */
void* myrtos_lock_alloc(size_t s)
{
    //ensure valid parameters
    if (0 == s) return NULL;

    //Find the first free block of a specified size rounded up to nearest 8th bit
    s = (s % 8) ? s + 8 - (s % 8) : s;
    myrtos_block_header_s* curr = top;
    while (curr->next != NULL)
    {
        size_t size = (size_t)curr->next - (size_t)curr;

        if (curr->size == 0 && size >= (s + sizeof(myrtos_block_header_s))) //is free and has space for alloc
        {
            //allocate block
            curr->size = s;
            myrtos_block_header_s* tmp = curr->next;
            curr->next = (void*)curr + s + MYRTOS_HEADER_SIZE;
            if ((tmp == curr->next) && (curr->next != end)) break;
            curr->next->next = (curr->next == end) ? NULL : tmp; //need to make sure we don't break end block
            curr->next->prev = curr;
            if (curr->next->next != NULL)
                curr->next->next->prev = curr->next;
            curr->next->size = 0;  //we are creating a new block here so we need to ensure it is marked as free
            break;
        }
        else
        {
            //else go to next block
            curr = curr->next;
        }
    }
    //if there is no space return null, else return mem address of user block
    if (curr->next == NULL)
        return NULL;
    else
        return (void*)curr + MYRTOS_HEADER_SIZE;
}

/**
 * @brief Reallocate a block of memory
 * 
 * Automatically frees original memory but does not change pointer value
 * 
 * @param p pointer to original memory
 * @param s size of new block
 * @return void* pointer to new block or NULL if no space available
 */
void* myrtos_lock_realloc(void* p, size_t s)
{
    //get the header from this block and make sure it is valid (in the header list)
    size_t s_tmp;
    myrtos_block_header_s* head = (myrtos_block_header_s*)(p - MYRTOS_HEADER_SIZE);
    myrtos_block_header_s* tmp = top;

    if (0 == s) return NULL;
    if (head->size == s) return p;
    while (tmp->next != NULL)
    {
        if (tmp == head) break;
        tmp = tmp->next;
    }
    if (tmp != head) return NULL;

    /* Allocate a new block */
    s_tmp = head->size;
    void* ret = myrtos_lock_alloc(s);

    if (ret == NULL) return NULL;
    if (ret == p) return ret;  //this is the case where we just extend the block
    
    //copy to new block
    s_tmp = (s_tmp > s) ? s : s_tmp;
    memcpy(ret, p, s_tmp);

    //free old block
    myrtos_lock_free(p);
    return ret;
}

/**
 * @brief Frees a block from heap memory
 * 
 * This function does not zero out previously used memory
 * 
 * @param p pointer to memory to free
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_lock_free(void* p)
{
    myrtos_block_header_s* head = p - MYRTOS_HEADER_SIZE;

    head->size = 0; //indicates block is free

    //need to consolidate blocks in the case where two free blocks are adjacent
    if ((head->next != end) && (0 == head->next->size))
    {
        head->next->next->prev = head; //leaving heap "dirty" here (not clearing memory)
        head->next = head->next->next;
    }
    if ((head != top && (0 == head->prev->size)))
    {
        head->prev->next = head->next;
        head->next->prev = head->prev;
    }

    return MYRTOS_SUCCESS;
}