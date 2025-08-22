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

#include <stdlib.h>
#include <stdint.h>

/**
 * @brief Generic Types
 * 
 */
typedef enum MYRTOS_BOOL_TYPE_E {
    MYRTOS_FALSE,
    MYRTOS_TRUE,
} myRTOS_bool_type_e;

typedef enum MYRTOS_RETURN_TYPE_E {
    MYRTOS_SUCCESS,
    MYRTOS_FAIL,
    MYRTOS_TASK_LIMIT_REACHED,
    MYRTOS_TASK_QUEUE_EMPTY,
    MYRTOS_MEMORY_LIMIT_REACHED,
    MYRTOS_MEMORY_INVALID,
    MYRTOS_MEMCPY_FAIL,
    MYRTOS_UART_INIT_FAIL,
    MYRTOS_UART_BAUD_FAIL
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

/**
 * @brief Task Types
 * 
 */
typedef struct MYRTOS_TASK_TYPE_S {
    const char* name;
    uint8_t priority;
    void* handle;
    void* args;
    uint8_t num_args;
    size_t stack_size;
} myRTOS_task_type_s;

 /**
  * @brief Inter task communication types
  * 
  */
 typedef struct MYRTOS_MUTEX_HANDLE_S {
    myRTOS_bool_type_e taken;
 } myRTOS_mutex_handle_s;

typedef struct MYRTOS_SEMAPHORE_HANDLE_S {
    uint8_t v;
} myRTOS_semaphore_handle_s;

typedef struct MYRTOS_QUEUE_HANDLE_S {
    int len;
    size_t size;
} myRTOS_queue_handle_s;

#endif