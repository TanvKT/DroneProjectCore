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
#include "myRTOS_HAL.h"

volatile bool g_sched_active = false;

/**
 * @brief Handles register management for context switch
 * 
 *          Specify inline here to ensure lower stack overhead
 * 
 */
static inline void myrtos_context_switch(myRTOS_int_task_type_s* t)
{

}

/**
 * @brief Interrupt service routine for handling task context switching
 * 
 */
static void myrtos_task_timer_isr(void)
{

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
    //register timer

    //grab first task

    //enable timer

    //context switch

    //hang here, this fucntion should not return
    for (;;){}

    return MYRTOS_SUCCESS;
}
myRTOS_return_type_e myrtos_schedule_stop()
{
    //disable timer interrupt
    
    return MYRTOS_SUCCESS;
}
myRTOS_return_type_e myrtos_enable_interupts(){return MYRTOS_SUCCESS;}
myRTOS_return_type_e myrtos_disable_interupts(){return MYRTOS_SUCCESS;}