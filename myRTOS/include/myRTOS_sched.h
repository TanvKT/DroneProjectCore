/**
 * @file myRTOS_sched.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-08-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __MYRTOS_SCHED_H__
#define __MYRTOS_SCHED_H__

#include "myRTOS_types.h"

/**
 * @brief Internal use task type
 * 
 */
typedef struct MYRTOS_INT_TASK_TYPE_S {
    myRTOS_task_type_s t;   //task
    void* sp;               //stack pointer
} myRTOS_int_task_type_s;

#endif