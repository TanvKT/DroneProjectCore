/**
 * @file myRTOS_task_queue.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-08-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #ifndef __MYRTOS_TASK_QUEUE_H__
 #define __MYRTOS_TASK_QUEUE_H__

 #include "myRTOS_types.h"
 #include "myRTOS_sched.h"
 #include <stdint.h>

#define     MYRTOS_MAX_TASKS           (MYRTOS_STACK_SIZE / (MYRTOS_MIN_STACK_SIZE))                                        //max possible tasks used for memory allocation of task info array

 /**
 * @brief array type to assist in priority queue allocation of tasks
 * 
 */
typedef struct
{
    size_t len;
    myRTOS_int_task_type_s* arr;
} myRTOS_task_queue_s;


 myRTOS_task_queue_s*   myrtos_get_task_queue();
 void                   myrtos_init_task_queue(void* p);
 void                   myrtos_sort_tasks();
 myRTOS_return_type_e   myrtos_register_task_i(myRTOS_task_type_s* t);
 myRTOS_return_type_e   myrtos_request_task(myRTOS_int_task_type_s* t);
 myRTOS_return_type_e   myrtos_peek_task(myRTOS_int_task_type_s* t);
 myRTOS_return_type_e   myrtos_push_task(myRTOS_int_task_type_s* t);

 #endif