/**
 * @file myRTOS_memory.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-08-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __MYRTOS_MEMORY_H__
#define __MYRTOS_MEMORY_H__

#include "myRTOS_config.h"
#include "myRTOS_types.h"
#include "myRTOS_task_queue.h"
#include <stdlib.h>

#define     MYRTOS_TASK_SIZE                 MYRTOS_MAX_TASKS * sizeof(myRTOS_int_task_type_s)
#define     MYRTOS_TASK_QUEUE_SIZE           sizeof(myRTOS_queue_arr_s)


/* memory reset */
myRTOS_return_type_e    myrtos_reset_memory();

/* task arr */
void*                   myrtos_get_task_queue_bp();
void*                   myrtos_get_task_arr_bp();

/* task stack */
void*                   myrtos_get_stack_bp();
void*                   myrtos_add_stack(size_t s);

/* task heap */
void*                   myrtos_get_heap_bp();

/* lock heap */
void*                   myrtos_get_lock_heap_bp();

#endif