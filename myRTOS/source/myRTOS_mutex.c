/**
 * @file myRTOS_mutex.c
 * @author your name (you@domain.com)
 * @brief Simple mutex locking structure
 * 
 *          Each mutes is initialized with a task array of an expected size by the user
 *              this expected size is the number of tasks expected to use this lock
 *          If this number is exceeded, the mutex will dynamically re-allocate more memory
 *              to allow for more tasks to use lock
 * @version 0.1
 * @date 2025-11-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "myRTOS.h"
#include "myRTOS_sched.h"
#include "myRTOS_lock_heap.h"

/**
 * @brief Initialize mutex lock
 * 
 * Defaults to size 2 (i.e. two tasks can be in the waiting list)
 * If more tasks request resource at same time, then mutex will dynamically resize and stay at the larger size
 * This can take precious time due to global interrupts being disabled, so it is best to initialize your mutex
 *          with the predetermined size of all possible tasks that may wait
 * This approach is taken so that when a resource is freed, all waiting tasks are ready to be unblocked with no
 *          search overhead or need to unblock every single other task
 * The cost of this is more memory usage but faster response in theory
 * 
 * @param h mutex handle type, must ensure this lives in static memory
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_mutex_init(myRTOS_mutex_handle_s* h)
{
    //set default fields, assume size of 2 here as default
    h->taken = 0;
    h->t = NULL;
    h->t_l = myrtos_lock_alloc(sizeof(myRTOS_int_task_type_vp) * 2);
    if (NULL == h->t_l) return MYRTOS_MEMORY_LIMIT_REACHED; 
    h->n = 2;

    return MYRTOS_SUCCESS;
}
/**
 * @brief Initialize mutex lock to predetermined size
 * 
 * Wating list initialized to n items
 * 
 * @param h mutex handle type, must ensure this lives in static memory
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_mutex_init_num(myRTOS_mutex_handle_s* h, size_t n)
{
    //set default fields
    h->taken = 0;
    h->t = NULL;
    h->t_l = myrtos_lock_alloc(sizeof(myRTOS_int_task_type_vp) * n);
    if (NULL == h->t_l) return MYRTOS_MEMORY_LIMIT_REACHED; 
    h->n = n;

    return MYRTOS_SUCCESS;
}

/**
 * @brief Takes mutex lock
 * 
 * Need to disable global interrupts here to ensure that no context switch can occur
 * 
 * If the lock is taken, we add this task to the waiting list and block it
 *      In the special case where this task has a higher priority than the holding task,
 *      we need to boost the holding task's priority (priority inheritance) to avoid priority inversion
 * 
 * @param h 
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_mutex_take(myRTOS_mutex_handle_s* h)
{
    // myrtos_disable_interupts();

    // //check lock status
    // if (0 == h->taken)
    // {
    //     //not taken
    //     //don't need atomic instructions here since we are disabling interrupts for short critical section
    //     h->taken = 1;
    //     h->t = s_curr_task_p;
    // }
    // else //taken
    // {

    // }

    return MYRTOS_SUCCESS;
}
myRTOS_return_type_e myrtos_mutex_give(myRTOS_mutex_handle_s* h)
{return MYRTOS_SUCCESS;}