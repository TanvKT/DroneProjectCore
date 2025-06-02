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
typedef enum myRTOS_bool_type_e {
    MYRTOS_FALSE,
    MYRTOS_TRUE,
};

typedef enum myRTOS_return_type_e {
    MYRTOS_SUCCESS,
    MYRTOS_FAIL
};

/**
 * @brief Task Types
 * 
 */
typedef struct myRTOS_task_type_s {
    const char* name;
    myrtos_uint8 priority;
    void* handle;
    void* args;
    myrtos_uint8 num_args;
};

/**
 * @brief Scheduling Types
 * 
 */
typedef enum myRTOS_schedule_type_e {
    MYRTOS_ROUND_ROBIN
};

/**
 * @brief Main myrtos config type
 * 
 */
 typedef struct myRTOS_config_type_s {
    myRTOS_schedule_type_e sched;
 };

 /**
  * @brief Inter task communication types
  * 
  */
 typedef struct myRTOS_mutex_handle_s {
    myRTOS_bool_type_e taken;
 };

typedef struct myRTOS_semaphore_handle_s {
    myrtos_uint8 v;
};

typedef struct myRTOS_queue_handle_s {
    int len;
    size_t size;
};