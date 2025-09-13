/**
 * @file myRTOS_sched.c
 * @author your name (you@domain.com)
 * @brief Handles task scheduling based on myRTOS_config file
 *              Uses a timer interrupt to determine when context needs switching
 * @version 0.1
 * @date 2025-06-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "myRTOS.h"
#include "myRTOS_sched.h"
#include "myRTOS_task_queue.h"
#include "myRTOS_HAL.h"
#include <stdio.h>

static volatile myRTOS_int_task_type_s  s_curr_task;
static myRTOS_int_task_type_s           s_idle_task;
myRTOS_int_task_type_s*                 s_curr_task_p = &s_idle_task;
static uint8_t                          s_idle_task_stack_arr[128]; //hard coding stack size here since we don't need much at all
static volatile bool                    s_fatal = false;            //bool flag to determine if fatal error occurs and set schedule to idle

/**
 * @brief Get the next task in the scheduler
 * 
 *          If using dynamic scheduling need to check if popped task has used too many time slots
 *              If this is the case, then the effective priority will be demoted
 *          If the task has not used the whole time slice and is at a lower priority than its initialized value 
 *              increase the priority of the task one level
 * 
 */
void myrtos_schedule(void)
{
    myRTOS_return_type_e ret;

    myrtos_hal_disable_interrupts();

    //if a fatal event has occured, we are hanging scheduler
    if (s_fatal)
    {
        s_curr_task_p = &s_idle_task;
        myrtos_hal_enable_interrupts();
        return;
    }

    if ((s_curr_task_p != NULL) && (s_curr_task_p != &s_idle_task))
    {
        #ifdef MYRTOS_DYNAMIC_PRIORITY
        //this task has used entire time slice, increment values
        s_curr_task.trig++;
        if (s_curr_task.trig > MYRTOS_PRIO_LOWER_THRESH)
        {
            s_curr_task.trig = 0;
            s_curr_task.t.priority += (s_curr_task.t.priority < MYRTOS_PRIORITY_LEVELS) ? 1 : 0;
        }
        #endif

        //push task back onto queue
        ret = myrtos_push_task(s_curr_task_p);
        if (ret != MYRTOS_SUCCESS) 
        {
            printf("FATAL:TASK PUSH FAILED DURING SCHEDULER CALL - %s\r\n", myrtos_debug_print(ret));
            s_curr_task_p = &s_idle_task;
            s_fatal = true;
            myrtos_hal_enable_interrupts();
            return;
        }
    }

    s_curr_task_p = (myRTOS_int_task_type_s*)&s_curr_task;
    ret = myrtos_request_task(s_curr_task_p);
    if (ret == MYRTOS_TASK_QUEUE_EMPTY)
    {
        //no tasks left to pull, swap to internal idle task at lowest priority
        s_curr_task_p = &s_idle_task;
    }
    //failed task request
    else if (ret != MYRTOS_SUCCESS)
    {
        printf("FATAL:TASK REQUEST FAILED DURING SCHEDULER CALL - %s\r\n", myrtos_debug_print(ret));
        s_curr_task_p = &s_idle_task;
        s_fatal = true;
    }

    myrtos_hal_enable_interrupts();
}

/**
 * @brief Sets fatal flag to true and hangs scheduler
 * 
 *          WARNING: If this is called, system restart is required to exit fatal mode
 * 
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_set_fatal()
{
    s_fatal = true;
    return MYRTOS_SUCCESS;
}

/**
 * @brief Interrupt service routine for handling task context switching
 * 
 *          If this has been triggered, then the current task has used its entire time slice
 *              this only matters if we are using a dynamic scheduling protocol
 * 
 */
static void myrtos_task_timer_isr(void)
{
    myrtos_hal_set_hard_isr();  //inline void with no overhead
}

/**
 * @brief Idle task for scheduler
 * 
 *          If the scheduler is unable to get the next task
 * 
 * @param args 
 */
static void myrtos_idle_task(void* args)
{
    //infinite loop
    for (;;){};
}

/**
 * @brief Start myRTOS task scheduler
 * 
 *          Grabs first task, enables timer interrupt, then context switches to task
 * 
 * @return myRTOS_return_type_e should not return
 */
myRTOS_return_type_e myrtos_schedule_start()
{
    //start by setting up idle task, only need to set stack pointer and handle
    //      since this task doesn't live in the task array and wont be touched by queue
    s_idle_task.sp = &s_idle_task_stack_arr[0];
    s_idle_task.t.handle = myrtos_idle_task;
    if (!myrtos_hal_stack_setup(&s_idle_task))                  return MYRTOS_SCHED_INIT_FAIL;

    //register timer
    if (!myrtos_hal_timer_init())                               return MYRTOS_TIMER_INIT_FAIL;
    if (!myrtos_hal_timer_configure())                          return MYRTOS_TIMER_INIT_FAIL;
    if (!myrtos_hal_register_callback(myrtos_task_timer_isr))   return MYRTOS_TIMER_INIT_FAIL;

    //grab first task, and set hardware registers to allow task execution
    myrtos_schedule();
    if (!myrtos_hal_schedule_init(s_curr_task_p))               return MYRTOS_SCHED_INIT_FAIL;

    //enable timer
    if (!myrtos_hal_timer_start())                              return MYRTOS_TIMER_INIT_FAIL;

    //set the interrupt to pending to start scheduling
    myrtos_hal_set_hard_isr();

    //hang here, this fucntion should not return
    for (;;){}

    return MYRTOS_FAIL;
}
myRTOS_return_type_e myrtos_schedule_disable()
{
    //disable timer interrupt
    myrtos_hal_timer_stop();
    return MYRTOS_SUCCESS;
}
myRTOS_return_type_e myrtos_schedule_enable()
{
    //enable timer interrupt
    myrtos_hal_timer_start();
    return MYRTOS_SUCCESS;
}
myRTOS_return_type_e myrtos_enable_interupts()
{
    myrtos_hal_enable_interrupts();
    return MYRTOS_SUCCESS;
}
myRTOS_return_type_e myrtos_disable_interupts()
{
    myrtos_hal_disable_interrupts();
    return MYRTOS_SUCCESS;
}