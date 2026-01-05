/**
 * @file myRTOS_types.h
 * @author Tanvin Thiagarajan (tthiagarajan@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-05-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __MYRTOS_TYPES_H__
#define __MYRTOS_TYPES_H__

#include "myRTOS_config.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Generic Types
 * 
 */
typedef enum MYRTOS_RETURN_TYPE_E {
    MYRTOS_SUCCESS,
    MYRTOS_FAIL,
    MYRTOS_TASK_LIMIT_REACHED,
    MYRTOS_TASK_QUEUE_EMPTY,
    MYRTOS_MEMORY_LIMIT_REACHED,
    MYRTOS_MEMORY_INVALID,
    MYRTOS_MEMCPY_FAIL,
    MYRTOS_UART_INIT_FAIL,
    MYRTOS_UART_BAUD_FAIL,
    MYRTOS_MEMINIT_FAIL,
    MYRTOS_TIMER_INIT_FAIL,
    MYRTOS_SCHED_INIT_FAIL,
    MYRTOS_UNBLOCK_FAIL
} myRTOS_return_type_e;
#define MYRTOS_SUCCESS_S                "MYRTOS_SUCCESS"
#define MYRTOS_FAIL_S                   "MYRTOS_FAIL"
#define MYRTOS_TASK_LIMIT_REACHED_S     "MYRTOS_TASK_LIMIT_REACHED"
#define MYRTOS_TASK_QUEUE_EMPTY_S       "MYRTOS_TASK_QUEUE_EMPTY"
#define MYRTOS_MEMORY_LIMIT_REACHED_S   "MYRTOS_MEMORY_LIMIT_REACHED"
#define MYRTOS_MEMORY_INVALID_S         "MYRTOS_MEMORY_INVALID"
#define MYRTOS_MEMCPY_FAIL_S            "MYRTOS_MEMCPY_FAIL"
#define MYRTOS_UART_INIT_FAIL_S         "MYRTOS_UART_INIT_FAIL"
#define MYRTOS_UART_BAUD_FAIL_S         "MYRTOS_UART_BAUD_FAIL"
#define MYRTOS_MEMINIT_FAIL_S           "MYRTOS_MEMINIT_FAIL"
#define MYRTOS_TIMER_INIT_FAIL_S        "MYRTOS_TIMER_INIT_FAIL"
#define MYRTOS_SCHED_INIT_FAIL_S        "MYRTOS_SCHED_INIT_FAIL"
#define MYRTOS_UNBLOCK_FAIL_S           "MYRTOS_UNBLOCK_FAIL"

/**
 * @brief Task Types
 * 
 */
typedef struct MYRTOS_TASK_TYPE_S {
    char name[MYRTOS_TASK_NAME_LEN];
    uint8_t priority;
    void* handle;
    void* args;
    size_t stack_size;
} myRTOS_task_type_s;

/**
 * @brief Internal use task type
 * 
 */
typedef volatile struct MYRTOS_INT_TASK_TYPE_S {
    void* sp;               //stack pointer (This needs to be first element so memory addressing is consistent)
    size_t b_i;             //index in blocked list (set to -1(max val) when not in blocked list)
    myRTOS_task_type_s t;   //task
    #ifdef MYRTOS_DYNAMIC_PRIORITY
    uint8_t o_prio;         //original priority
    uint8_t trig;           //number of times full time slice used
    #endif
    bool b;                 //is task blocked
} myRTOS_int_task_type_s;
typedef myRTOS_int_task_type_s * volatile myRTOS_int_task_type_vp;

 /**
  * @brief Inter task communication types
  * 
  */
 typedef volatile struct MYRTOS_MUTEX_HANDLE_S {
    myRTOS_int_task_type_vp t;      //currently holding task (also used as taken flag)
    myRTOS_int_task_type_vp* t_l;   //points to region in memory where blocked tasks lie
    size_t t_i;                     //waiting list add index
    size_t n;                       //number of tasks being blocked by this lock
    uint8_t in_prio;                //inherited priority of lock
 } myRTOS_mutex_handle_s;

typedef struct MYRTOS_SEMAPHORE_HANDLE_S {
    uint8_t v;
} myRTOS_semaphore_handle_s;

typedef struct MYRTOS_QUEUE_HANDLE_S {
    int len;
    size_t size;
} myRTOS_queue_handle_s;

#endif