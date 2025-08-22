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

 void                   myrtos_init_task_queue(void* p);
 void*                  myrtos_consolidate_tasks();
 myRTOS_return_type_e   myrtos_set_task_stacks();
 myRTOS_return_type_e   myrtos_request_task(myRTOS_int_task_type_s* t);
 myRTOS_return_type_e   myrtos_peek_task(myRTOS_int_task_type_s* t);
 myRTOS_return_type_e   myrtos_push_task(myRTOS_int_task_type_s* t);

 #endif