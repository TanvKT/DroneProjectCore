/**
 * @file myRTOS_memory.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-08-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "myRTOS_config.h"
#include "myRTOS_types.h"
#include "myRTOS_memory.h"
#include <stdio.h>
#include <stdint.h>
#include <memory.h>

/* grab allocated memory regions ensuring alignment */
/* also want to ensure linear allocation (mostly for testing purposes, but can provide caching benefits) */
typedef struct MYRTOS_MEMORY {
    uint8_t tasks_arr        [MYRTOS_TASK_SIZE + 1]          __attribute__((aligned(8)));
    uint8_t tasks_queue_arr  [MYRTOS_TASK_QUEUE_SIZE + 1]    __attribute__((aligned(8)));
    uint8_t stacks_arr       [MYRTOS_STACK_SIZE + 1]         __attribute__((aligned(8)));
    uint8_t heap_arr         [MYRTOS_HEAP_SIZE + 1]          __attribute__((aligned(8)));
    uint8_t lock_heap_arr    [MYRTOS_LOCK_HEAP_SIZE + 1]     __attribute__((aligned(8)));
} myRTOS_memory;
static myRTOS_memory s_mem;

/* static stack pointer to start of next allocatable block of stack */
static size_t s_off_p = 0;
/* static pointer to beginning of stack memory */
static uint8_t* stack_start = &s_mem.stacks_arr[0];

/**
 * @brief Get the task queue base pointer
 * 
 * @return void* pointer to base of task arr
 */
void* myrtos_get_task_queue_bp()
{
    return &s_mem.tasks_queue_arr[0];
}

/**
 * @brief Get the task arr base pointer
 * 
 * @return void* pointer to base of task arr
 */
void* myrtos_get_task_arr_bp()
{
    return &s_mem.tasks_arr[0];
}

/**
 * @brief returns the base pointer to the stack memory region
 * 
 * @return void* pointer to stack
 */
void*   myrtos_get_stack_bp()
{
    return &s_mem.stacks_arr[0];
}

/**
 * @brief returns the base pointer to the heap memory region
 * 
 * @return void* pointer to heap
 */
void*   myrtos_get_heap_bp()
{
    return &s_mem.heap_arr[0];
}

/**
 * @brief 
 * 
 * @return void* 
 */
void*   myrtos_get_lock_heap_bp()
{
    return &s_mem.lock_heap_arr[0];
}

/**
 * @brief allocate a stack to a new task
 * 
 *          Need to we have an extra bit of padding for the case where we
 *              line up perfectly with the start of the heap, in this case the stack pointer would
 *              point to the start of the heap and would corrupt the header
 * 
 * @param s NOTE: NEEDS TO BE 8-BYTE ALIGNED
 * @return void* pointer to base of allocated stack
 */
void* myrtos_add_stack(size_t s)
{
    s_off_p += s;
    //ensure that we are not allocating past the buffer
    if ((stack_start + s_off_p) > &s_mem.stacks_arr[MYRTOS_STACK_SIZE])
    {
        #if MYRTOS_DEBUG_MODE
        printf("Attempted to allocate over bounds -> \nHeap Start    0x%x\nEnd Stack     0x%x\nStack Pointer 0x%x\n", (size_t)&s_mem.heap_arr[0], (size_t)&s_mem.stacks_arr[MYRTOS_STACK_SIZE], (size_t)stack_start+s_off_p);
        #endif
        s_off_p -= s;
        return NULL;
    }
    return (void*)stack_start + s_off_p; //since we are using an ARM based processor the stack grows down
}

/**
 * @brief reset the stack
 * 
 */
myRTOS_return_type_e myrtos_reset_memory()
{
    s_off_p = 0;
    //ensure non-null pointer to memory sections, redundant check for safety reasons
    if (NULL == stack_start || NULL == &s_mem.tasks_arr[0] || NULL == &s_mem.heap_arr[0]) return MYRTOS_MEMINIT_FAIL;

    //zeroize
    if (!memset(stack_start,                  0, sizeof(uint8_t)*(MYRTOS_STACK_SIZE-1)))        return MYRTOS_MEMINIT_FAIL;
    if (!memset(&s_mem.tasks_arr[0],          0, sizeof(uint8_t)*(MYRTOS_TASK_SIZE-1)))         return MYRTOS_MEMINIT_FAIL;
    if (!memset(&s_mem.tasks_queue_arr[0],    0, sizeof(uint8_t)*(MYRTOS_TASK_QUEUE_SIZE-1)))   return MYRTOS_MEMINIT_FAIL;
    if (!memset(&s_mem.heap_arr[0],           0, sizeof(uint8_t)*(MYRTOS_HEAP_SIZE-1)))         return MYRTOS_MEMINIT_FAIL;
    if (!memset(&s_mem.lock_heap_arr[0],      0, sizeof(uint8_t)*(MYRTOS_LOCK_HEAP_SIZE-1)))    return MYRTOS_MEMINIT_FAIL;

    return MYRTOS_SUCCESS;
}