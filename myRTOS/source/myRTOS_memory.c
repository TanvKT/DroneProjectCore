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
#include <memory.h>

/* grab allocated memory regions ensuring 8-bit alignment */
#ifdef MYRTOS_USE_LINKER
__attribute__((aligned(8), section(".myrtos_tasks")))
static uint8_t tasks_arr[MYRTOS_TASK_SIZE];

__attribute__((aligned(8), section(".myrtos_stacks")))
static uint8_t stacks_arr[MYRTOS_STACK_SIZE];

__attribute__((aligned(8), section(".myrtos_heap")))
static uint8_t heap_arr[MYRTOS_HEAP_SIZE];
#else
static uint8_t tasks_arr[MYRTOS_TASK_SIZE] __attribute__((aligned(8)));
static uint8_t stacks_arr[MYRTOS_STACK_SIZE] __attribute__((aligned(8)));
static uint8_t heap_arr[MYRTOS_HEAP_SIZE] __attribute__((aligned(8)));
#endif

/* static stack pointer to start of next allocatable block of stack */
static size_t s_off_p = 0;
/* static pointer to beginning of stack memory */
static uint8_t* stack_start = &stacks_arr[0];

/**
 * @brief Get the task arr base pointer
 * 
 * @return void* pointer to base of task arr
 */
void* myrtos_get_task_arr_bp()
{
    return &tasks_arr[0];
}

/**
 * @brief returns the base pointer to the stack memory region
 * 
 * @return void* pointer to stack
 */
void*   myrtos_get_stack_bp()
{
    return &stacks_arr[0];
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

/**
 * @brief allocate a stack to a new task
 * 
 *          Need to subtract 1 from size since stack array indexing starts at 0
 *              Therefore at this point s_off_p = 0 means we have 1 byte allocated
 *              Since s_off_p is unsigned, we can't start it at -1, so we do subtraction here
 * 
 * @param s 
 * @return void* pointer to base of allocated stack
 */
void* myrtos_add_stack(size_t s)
{
    s_off_p += (0 == s_off_p) ? (s - 1) : s;
    //ensure that we are not allocating past the buffer
    if ((stack_start + s_off_p) > &stacks_arr[MYRTOS_STACK_SIZE - 1])
    {
        s_off_p -= (0 == s_off_p) ? (s - 1) : s;
        return NULL;
    }
    return (void*)stack_start + s_off_p;
}

/**
 * @brief reset the stack
 * 
 */
myRTOS_return_type_e myrtos_reset_memory()
{
    s_off_p = 0;
    //zeroize
    if (!memset(stack_start, 0, sizeof(uint8_t)*(MYRTOS_STACK_SIZE-1))) return MYRTOS_MEMINIT_FAIL;
    if (!memset(&tasks_arr[0], 0, sizeof(uint8_t)*(MYRTOS_TASK_SIZE-1))) return MYRTOS_MEMINIT_FAIL;
    if (!memset(&heap_arr[0], 0, sizeof(uint8_t)*(MYRTOS_HEAP_SIZE-1))) return MYRTOS_MEMINIT_FAIL;

    return MYRTOS_SUCCESS;
}