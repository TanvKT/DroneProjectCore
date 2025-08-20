/**
 * @file myRTOS_init.c
 * @author Tanvin Thiagarajan (tthiagarajan@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "myRTOS.h"
#include "myRTOS_config.h"
#include "myRTOS_UART.h"
#include "myRTOS_memory.h"
#include "myRTOS_sched.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * @brief array type to assist in dynamic allocation of tasks
 * 
 */
typedef struct
{
    int len;
    myRTOS_int_task_type_s arr[MYRTOS_MAX_TASKS];
} myRTOS_task_arr_s;

/**
 * @brief GLOBALS
 * 
 */
static size_t n_tasks = 0;

/**
 * TASK HOLDING ARRAY
 * 
 * Memory block is preallocated with the worst case scenario in terms of maximum tasks possible
 *      The number of tasks possible is calculated using the minimum stack size for a task
 *      and the allocated stack space for those tasks
 */
static myRTOS_task_arr_s* tasks;

/**
 * @brief default initialization function for myrtos
 * 
 * Initialize MyRTOS stack and management data
 * 
 * @param c config values for MyRTOS program, if null will use defaults
 * 
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_init()
{
    myRTOS_return_type_e my_ret;

    //need to initialize myRTOS UART before anything
    my_ret = myRTOS_uart_init();
    if (my_ret != MYRTOS_SUCCESS) return my_ret;
    #if MYRTOS_DEBUG_MODE
    printf("---------MYRTOS UART INITIALIZED---------\r\n");
    #endif

    //initialize memory regions for myRTOS
    my_ret = myrtos_memory_init();
    if (my_ret != MYRTOS_SUCCESS) return my_ret;
    #if MYRTOS_DEBUG_MODE
    printf("--------MYRTOS MEMORY INITIALIZED--------\r\n");
    #endif

    //initialize heap for dynamic task allocation
    my_ret = myrtos_heap_init();
    if (my_ret != MYRTOS_SUCCESS) return my_ret;
    #if MYRTOS_DEBUG_MODE
    printf("---------MYRTOS HEAP INITIALIZED---------\r\n");
    #endif

    //grab the base pointer of the task management array
    tasks = (myRTOS_task_arr_s*)myrtos_get_task_arr_bp();
    for (int i = 0; i < MYRTOS_PRIORITY_LEVELS; i++)
    {
        tasks[i].len = 0;
    }
    #if MYRTOS_DEBUG_MODE
    printf("------MYRTOS TASK ARRAY INITIALIZED------\r\n");
    #endif

    #if MYRTOS_DEBUG_MODE
    printf("-----MYRTOS SUCCESSFULLY INITIALIZED-----\r\n");
    #endif
    return MYRTOS_SUCCESS;
}

/**
 * @brief Register task to myRTOS scheduler
 * 
 * Task being passed in MUST exist in memory throughout entirety of function call
 * 
 * @param t task to register
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_register_task(myRTOS_task_type_s* t)
{
    myRTOS_int_task_type_s* t_i;
    size_t ss;

    #ifndef MYRTOS_ROUND_ROBIN
    //need to consider priority when allocating task
    if (MYRTOS_MAX_TASKS * MYRTOS_PRIORITY_LEVELS == n_tasks) return MYRTOS_TASK_LIMIT_REACHED;
    uint8_t p = t.priority < MYRTOS_PRIORITY_LEVELS ? t.priority : MYRTOS_PRIORITY_LEVELS - 1;
    t_i = &tasks[p].arr[tasks[p].len];
    if (!memcpy(t_i, t, sizeof(myRTOS_task_type_s))) return MYRTOS_MEMCPY_FAIL;
    tasks[p].len++;
    #else
    //default to round robin (equal priority on tasks)
    if (MYRTOS_MAX_TASKS == n_tasks) return MYRTOS_TASK_LIMIT_REACHED;
    t_i = &tasks[0].arr[tasks[0].len];
    if (!memcpy(t_i, t, sizeof(myRTOS_task_type_s))) return MYRTOS_MEMCPY_FAIL;
    tasks[0].len++;
    #endif

    //allocate stack in reserved memory
    ss = (t->stack_size < MYRTOS_MIN_STACK_SIZE) ? MYRTOS_MIN_STACK_SIZE : t->stack_size;
    t_i->sp = myrtos_add_stack(ss);
    if (!t_i->sp) return MYRTOS_MEMORY_LIMIT_REACHED;

    n_tasks++;
    return MYRTOS_SUCCESS;
}

/**
 * @brief Debug print function that directs to a specific global string value
 *              based on a passed return type
 * 
 * @param r return value to decode
 * @return const char* pointer to debug string
 */
const char* myrtos_debug_print(myRTOS_return_type_e r)
{
    switch(r)
    {
        case MYRTOS_FAIL :
            return MYRTOS_FAIL_S;
        case MYRTOS_TASK_LIMIT_REACHED :
            return MYRTOS_TASK_LIMIT_REACHED_S;
        case MYRTOS_MEMORY_LIMIT_REACHED :
            return MYRTOS_MEMORY_LIMIT_REACHED_S;
        case MYRTOS_MEMORY_INVALID :
            return MYRTOS_MEMORY_INVALID_S;
        case MYRTOS_MEMCPY_FAIL :
            return MYRTOS_MEMCPY_FAIL_S;
        case MYRTOS_UART_INIT_FAIL :
            return MYRTOS_UART_INIT_FAIL_S;
        case MYRTOS_UART_BAUD_FAIL :
            return MYRTOS_UART_BAUD_FAIL_S;
        default :
            return MYRTOS_SUCCESS_S;
    }
}
