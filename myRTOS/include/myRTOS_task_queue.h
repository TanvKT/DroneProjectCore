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

#ifdef      MYRTOS_ROUND_ROBIN
#define     MYRTOS_QUEUE_ARR_LEN       1
#else
#define     MYRTOS_QUEUE_ARR_LEN       MYRTOS_PRIORITY_LEVELS
#endif

 /**
 * @brief array type to assist in priority queue allocation of tasks
 * 
 */
typedef struct
{
    size_t len;
    size_t st;
    size_t en;
    myRTOS_int_task_type_s* arr[MYRTOS_MAX_TASKS];
} myRTOS_task_queue_s;

typedef struct MYRTOS_QUEUE_ARR_S {
    myRTOS_task_queue_s level[MYRTOS_QUEUE_ARR_LEN];
    myRTOS_task_queue_s blocked;
} myRTOS_queue_arr_s;


 myRTOS_queue_arr_s*    myrtos_get_task_queue();
 myRTOS_return_type_e   myrtos_init_task_arr(void* p);
 myRTOS_return_type_e   myrtos_init_task_queue(void* p);
 myRTOS_return_type_e   myrtos_register_task_i(myRTOS_task_type_s* t);
 myRTOS_return_type_e   myrtos_request_task(myRTOS_int_task_type_s** t);
 myRTOS_return_type_e   myrtos_peek_task(myRTOS_int_task_type_s** t);
 myRTOS_return_type_e   myrtos_push_task(myRTOS_int_task_type_s* t);
 myRTOS_return_type_e   myrtos_push_blocked_task(myRTOS_int_task_type_s* t);
 myRTOS_task_queue_s*   myrtos_get_blocked_list_ptr();
 size_t                 myrtos_get_blocked_list_cpy(myRTOS_int_task_type_s* l, size_t n);
 myRTOS_return_type_e   myrtos_rem_blocked_task(myRTOS_int_task_type_s** t, size_t i);

 #endif