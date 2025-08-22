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

#include "myRTOS_types.h"
#include "myRTOS_memory.h"
#include <stdint.h>

/* grab allocated memory regions ensuring 8-bit alignment */
#ifdef MYRTOS_USE_LINKER
__attribute__((aligned(8), section(".myrtos_task_stacks")))
static uint8_t task_stacks_arr[MYRTOS_STACK_SIZE];

__attribute__((aligned(8), section(".myrtos_heap")))
static uint8_t heap_arr[MYRTOS_HEAP_SIZE];
#else
static uint8_t task_stacks_arr[MYRTOS_STACK_SIZE] __attribute__((aligned(8)));
static uint8_t heap_arr[MYRTOS_HEAP_SIZE] __attribute__((aligned(8)));
#endif

/* static stack pointer to start of next allocatable block of stack */
static size_t s_off_p = 0;
/* static pointer to beginning of stack memory */
static uint8_t* stack_start;

/**
 * @brief set pointer to start of usable stack for tasks
 * 
 * @param p pointer to start of stack region
 */
void myrtos_memory_set_stack_start(void* p)
{
    stack_start = (uint8_t*)p;
}

/**
 * @brief Get the task arr base pointer
 * 
 * @return void* pointer to base of task arr
 */
void* myrtos_get_task_arr_bp()
{
    return &task_stacks_arr[0];
}

/**
 * @brief allocate a stack to a new task
 * 
 * @param s 
 * @return void* pointer to base of allocated stack
 */
void* myrtos_add_stack(size_t s)
{
    s_off_p += s;
    //ensure that we are not allocating past the buffer
    return ((stack_start + s_off_p) <= &task_stacks_arr[MYRTOS_STACK_SIZE - 1]) ? (void*)stack_start +s_off_p : NULL;
}

/**
 * @brief reset the stack
 * 
 */
void    myrtos_reset_stack()
{
    s_off_p = 0;
}

/**
 * @brief returns the base pointer to the heap memory region
 * 
 * @return void* pointer to heap
 */
void*   myrtos_get_heap_bp()
{
    return &heap_arr[0];
}