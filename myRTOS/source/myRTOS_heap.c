/**
 * @file myRTOS_heap.c
 * @author Tanvin Thiagarajan (tthiagarajan@wisc.edu)
 * @brief 
 *          Using a simple heap allocater to allow for thread safe heap allocations
 *          Allocate entire heap as one large free block shared between tasks
 *          Access to malloc and free operations controlled by a mutex
 *          During malloc and free operations, interrupts are disabled
 * 
 *          There are no protections for memory accesses outside of requested bounds
 *          Writing outside of bounds will destroy the next header
 * 
 * @version 0.1
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "myRTOS.h"
#include "myRTOS_memory.h"
#include "myRTOS_heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 

/**
 * @brief Globals
 * 
 */
static myRTOS_mutex_handle_s sg_mem_lock;
static myrtos_block_header_s* top;
static myrtos_block_header_s* end;

/**
 * @brief Zeroizes entire heap
 * 
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_zeroize_heap()
{
    uint8_t* heap = (uint8_t*)myrtos_get_heap_bp();
    if (NULL == heap) return MYRTOS_MEMORY_INVALID;
    //could just use memset for this, but compiler will optimize this the same way
    for (size_t i = 0; i < MYRTOS_HEAP_SIZE; i++)
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
myRTOS_return_type_e myrtos_heap_init()
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
    ret = myrtos_zeroize_heap();
    if (ret != MYRTOS_SUCCESS) return ret;

    top = (myrtos_block_header_s*)myrtos_get_heap_bp();
    if (!top) return MYRTOS_MEMORY_INVALID;
    end = (myrtos_block_header_s*)((void*)top + MYRTOS_HEAP_SIZE - MYRTOS_HEADER_SIZE);

    top->next = end;
    end->next = NULL;
    top->prev = NULL;
    end->prev = top;

    //initialize memory lock so that we are thread safe
    ret = myrtos_mutex_init(&sg_mem_lock);

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
void* myrtos_alloc(size_t s)
{
    //take lock so that only one allocation can happen at a time
    myrtos_mutex_take(&sg_mem_lock);

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

    //give up lock so that other tasks pending on a heap allocation can go ahead
    myrtos_mutex_give(&sg_mem_lock);

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
void* myrtos_realloc(void* p, size_t s)
{
    myrtos_mutex_take(&sg_mem_lock);

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
    myrtos_mutex_give(&sg_mem_lock);
    void* ret = myrtos_alloc(s);
    myrtos_mutex_take(&sg_mem_lock);

    if (ret == NULL) return NULL;
    if (ret == p) return ret;  //this is the case where we just extend the block
    
    //copy to new block
    s_tmp = (s_tmp > s) ? s : s_tmp;
    memcpy(ret, p, s_tmp);

    //free old block
    myrtos_mutex_give(&sg_mem_lock);
    myrtos_free(p);
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
myRTOS_return_type_e myrtos_free(void* p)
{
    myrtos_mutex_take(&sg_mem_lock);
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

    myrtos_mutex_give(&sg_mem_lock);
    return MYRTOS_SUCCESS;
}

/**
 * @brief Debug helper to return the addresses of header within the heap
 * 
 * @param arr array of header pointers
 * @param n maximum number of items to add
 * 
 * @return size_t number of elements added
 */
size_t myrtos_list_heap(myrtos_block_header_s** arr, size_t n)
{
    myrtos_mutex_take(&sg_mem_lock);
    if ((NULL == arr) || 0 == n) return 0;

    myrtos_block_header_s* curr = top;
    for (size_t i = 0; i < n; i++)
    {
        arr[i] = curr;
        curr = curr->next;
        if (NULL == curr) return i + 1;
    }
    myrtos_mutex_give(&sg_mem_lock);
    return n;
}

/**
 * @brief Helper debug function to print heap layout into string
 * 
 * @param str pointer to string to print to
 *            up to user to ensure string buffer has enough space to hold string
 */
void myrtos_print_heap(char** str)
{
    myrtos_mutex_take(&sg_mem_lock);
    myrtos_block_header_s* head = top;
    size_t allocd_size = 0;
    size_t free_n = MYRTOS_HEAP_SIZE - 2*MYRTOS_HEADER_SIZE;
    char tmp[100];

    sprintf(*str, "\r--------------------------------------------------\n\r");
    while (head->next != NULL)
    {
        strcat(*str, "|                                                |\n\r");
        sprintf((char*)tmp, "           ADDR: 0x%x --- SIZE: %d\n\r", (unsigned int)head + MYRTOS_HEADER_SIZE, (unsigned int)head->size);
        strcat(*str, (char*) tmp);
        strcat(*str, "|                                                |\n\r");
        strcat(*str, "--------------------------------------------------\n\r");
        allocd_size += head->size;
        free_n -= MYRTOS_HEADER_SIZE + head->size;
        head = head->next;
    }
    strcat(*str, "|                                                |\n\r");
    sprintf((char*) tmp, "                  ALLOCATED: %d                 \n\r", (unsigned int)allocd_size);
    strcat(*str, (char*) tmp);
    sprintf((char*) tmp, "                   FREE: %d/%d                  \n\r", (unsigned int)free_n, MYRTOS_HEAP_SIZE);
    strcat(*str, (char*) tmp);
    strcat(*str, "|                                                |\n\r");
    strcat(*str, "--------------------------------------------------\n\r");
    myrtos_mutex_give(&sg_mem_lock);
}
