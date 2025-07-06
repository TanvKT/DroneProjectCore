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

#include "myRTOS_heap.h"
#include "memory.h"
#include "stdio.h"
#include "string.h"

/**
 * @brief Local struct for heap header type
 * 
 * Total size 16 bytes, using multiples of 8 for each block
 *
 */
typedef struct BLOCK_HEADER {
    size_t size;                    //(4 bytes) when size is 0, the block is free
    struct BLOCK_HEADER* next;      //(4 bytes) pointer to next header, if next is NULL, then we are on end block
    struct BLOCK_HEADER* prev;      //(4 bytes) pointer to previous header, if prev is NULL, then we are on top block
} block_header_s;

#define HEADER_SIZE 16
 

/**
 * @brief Globals
 * 
 */
static myRTOS_mutex_handle_s sg_mem_lock;
static block_header_s* top;
static block_header_s* end;

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
    top = (block_header_s*)calloc(HEAP_SIZE, 1); //making sure all bytes of heap set to 0
    if (top == NULL)
        return MYRTOS_FAIL;
    end = (block_header_s*)((void*)top + HEAP_SIZE - HEADER_SIZE);

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

    //Find the first free block of a specified size rounded up to nearest 8th bit
    s = (s % 8) ? s + 8 - (s % 8) : s;
    block_header_s* curr = top;
    while (curr->next != NULL)
    {
        size_t size = (size_t)curr->next - (size_t)curr;

        if (curr->size == 0 && size >= (s + sizeof(block_header_s))) //is free and has space for alloc
        {
            //allocate block
            curr->size = s;
            block_header_s* tmp = curr->next;
            curr->next = (void*)curr + s + HEADER_SIZE;
            curr->next->next = (curr->next == end) ? NULL : tmp; //need to make sure we don't break end block
            curr->next->prev = curr;
            if (curr->next->next != NULL)
                curr->next->next->prev = curr->next;
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
        return (void*)curr + HEADER_SIZE;
}

/**
 * @brief Reallocate a block of memory
 * 
 * Automatically frees original memory but does not change pointer value
 * 
 * @param p pointer to original memory
 * @param c size of current memory to copy
 * @param s size of new block
 * @return void* pointer to new block or NULL if no space available
 */
void* myrtos_realloc(void* p, size_t c, size_t s)
{
    //allocate new block
    void* ret = myrtos_alloc(s);

    //if ret is null then return
    if (ret == NULL)
        return NULL;
    
    //copy to new block
    c = (c > s) ? s : c;
    memcpy(ret, p, c);
    //free old block and return new pointer
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
    block_header_s* head = p - HEADER_SIZE;

    //set previous and next header to point to eachother
    if (head != top) //don't need to change pointers if head is top block
    {
        head->prev->next = head->next;
        head->next->prev = head->prev;
    }
    head->size = 0; //indicates block is free

    return MYRTOS_SUCCESS;
}

/**
 * @brief Helper debug function to print heap layout into string
 * 
 * @param str pointer to string to print to
 *            up to user to ensure string buffer has enough space to hold string
 */
void myrtos_print_heap(char** str)
{
    block_header_s* head = top;
    size_t allocd_size = 0;
    size_t free_n = HEAP_SIZE - 2*HEADER_SIZE;
    char tmp[100];

    sprintf(*str, "\r--------------------------------------------------\n\r");
    while (head->next != NULL)
    {
        strcat(*str, "|                                                |\n\r");
        sprintf((char*)tmp, "           ADDR: 0x%x --- SIZE: %d\n\r", (unsigned int)head + HEADER_SIZE, (unsigned int)head->size);
        strcat(*str, (char*) tmp);
        strcat(*str, "|                                                |\n\r");
        strcat(*str, "--------------------------------------------------\n\r");
        allocd_size += head->size;
        free_n -= HEADER_SIZE + head->size;
        head = head->next;
    }
    strcat(*str, "|                                                |\n\r");
    sprintf((char*) tmp, "                  ALLOCATED: %d                 \n\r", (unsigned int)allocd_size);
    strcat(*str, (char*) tmp);
    sprintf((char*) tmp, "                   FREE: %d/%d                  \n\r", (unsigned int)free_n, HEAP_SIZE);
    strcat(*str, (char*) tmp);
    strcat(*str, "|                                                |\n\r");
    strcat(*str, "--------------------------------------------------\n\r");
}