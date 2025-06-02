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

#include "stdlib.h"

/**
 * @brief Generic Types
 * 
 */
typedef char myrtos_uint8;
typedef enum MYRTOS_BOOL_TYPE_E {
    MYRTOS_FALSE,
    MYRTOS_TRUE,
} myRTOS_bool_type_e;

typedef enum MYRTOS_RETURN_TYPE_E {
    MYRTOS_SUCCESS,
    MYRTOS_FAIL
} myRTOS_return_type_e;

/**
 * @brief Task Types
 * 
 */
typedef struct mMYRTOS_TASK_TYPE_S {
    const char* name;
    myrtos_uint8 priority;
    void* handle;
    void* args;
    myrtos_uint8 num_args;
} myRTOS_task_type_s;

/**
 * @brief Scheduling Types
 * 
 */
typedef enum MYRTOS_SCHEDULE_TYPE_E {
    MYRTOS_ROUND_ROBIN
} myRTOS_schedule_type_e;

 /**
  * @brief Inter task communication types
  * 
  */
 typedef struct MYRTOS_MUTEX_HANDLE_S {
    myRTOS_bool_type_e taken;
 } myRTOS_mutex_handle_s;

typedef struct MYRTOS_SEMAPHORE_HANDLE_S {
    myrtos_uint8 v;
} myRTOS_semaphore_handle_s;

typedef struct MYRTOS_QUEUE_HANDLE_S {
    int len;
    size_t size;
} myRTOS_queue_handle_s;