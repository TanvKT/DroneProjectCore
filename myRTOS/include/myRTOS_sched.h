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
#include "myRTOS_HAL.h"
#include <stdbool.h>

extern myRTOS_int_task_type_s* s_curr_task_p;
void                 myrtos_schedule(void);
myRTOS_return_type_e myrtos_block_task(myRTOS_int_task_type_s* t);
myRTOS_return_type_e myrtos_unblock_task(myRTOS_int_task_type_s* t);

//testing functions
myRTOS_return_type_e myrtos_block_all();
myRTOS_return_type_e myrtos_unblock_all();
myRTOS_int_task_type_s* myrtos_get_idle_task();

#endif