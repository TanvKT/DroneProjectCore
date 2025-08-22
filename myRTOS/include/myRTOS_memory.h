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
#include <stdlib.h>

/* memory init */
void                    myrtos_memory_set_stack_start(void* p);
void*                   myrtos_get_task_arr_bp();

/* task stack modification */
void*                   myrtos_add_stack(size_t s);
void                    myrtos_reset_stack();

/* task heap modification */
void*                   myrtos_get_heap_bp();

#endif