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
#include "myRTOS_task_queue.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * @brief GLOBALS
 * 
 */

 /**
 * @brief Reset myRTOS allocations
 * 
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_reset()
{
    myRTOS_return_type_e my_ret;

    //reset memory (zeroize)
    my_ret = myrtos_reset_memory();
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
    my_ret = myrtos_init_task_queue(myrtos_get_task_arr_bp());
    if (my_ret != MYRTOS_SUCCESS) return my_ret;
    #if MYRTOS_DEBUG_MODE
    printf("------MYRTOS TASK ARRAY INITIALIZED------\r\n");
    #endif

    #if MYRTOS_DEBUG_MODE
    printf("-----MYRTOS SUCCESSFULLY INITIALIZED-----\r\n");
    #endif
    return MYRTOS_SUCCESS;
}

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

    return myrtos_reset();
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
        case MYRTOS_TASK_QUEUE_EMPTY :
            return MYRTOS_TASK_QUEUE_EMPTY_S;
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
        case MYRTOS_MEMINIT_FAIL :
            return MYRTOS_MEMINIT_FAIL_S;
        default :
            return MYRTOS_SUCCESS_S;
    }
}
