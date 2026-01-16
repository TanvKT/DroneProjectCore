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
#include "myRTOS_task_queue.h"

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
    if (NULL == h) return MYRTOS_FAIL;
    //set default fields, assume size of 2 here as default
    h->t = NULL;
    h->t_l = myrtos_lock_alloc(sizeof(myRTOS_int_task_type_vp) * 2);
    if (NULL == h->t_l) return MYRTOS_MEMORY_LIMIT_REACHED; 
    h->n = 2;
    h->t_i = 0;
    h->in_prio = 0;

    return MYRTOS_SUCCESS;
}
/**
 * @brief Initialize mutex lock to predetermined size
 * 
 * Wating list initialized to n items
 * If n is passed as zero, will default to 1
 * 
 * @param h mutex handle type, must ensure this lives in static memory
 * @param n number of items to initialize waiting list to
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_mutex_init_num(myRTOS_mutex_handle_s* h, size_t n)
{
    if (NULL == h) return MYRTOS_FAIL;
    n = (0 == n) ? 1 : n; //default n to 1
    //set default fields
    h->t = NULL;
    h->t_l = myrtos_lock_alloc(sizeof(myRTOS_int_task_type_vp) * n);
    if (NULL == h->t_l) return MYRTOS_MEMORY_LIMIT_REACHED; 
    h->n = n;
    h->t_i = 0;
    h->in_prio = 0;

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
 * @param h pointer to mutex handle object
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_mutex_take(myRTOS_mutex_handle_s* h)
{
    if (NULL == h) return MYRTOS_FAIL;

    //attempt will stall until lock is taken
    for (;;)
    {
        myrtos_disable_interupts();

        //current task should never be null in this case, but adding a sanity check
        if (NULL == s_curr_task_p)
        {
            myrtos_enable_interupts();
            return MYRTOS_FAIL;
        }

        //check lock status
        if (NULL == h->t)
        {
            //not taken
            //don't need atomic instructions here since we are disabling interrupts for short critical section
            h->t = s_curr_task_p;
            h->in_prio = s_curr_task_p->t.priority;

            //success, return
            myrtos_enable_interupts();
            return MYRTOS_SUCCESS;
        }
        else //taken
        {
            //If using dynamic priority, need to ensure that blocked task returns to correct priority
            #ifdef MYRTOS_DYNAMIC_PRIORITY
            s_curr_task_p->t.priority = s_curr_task_p->o_prio;
            #endif

            //block the current task
            if (myrtos_block_task(s_curr_task_p) != MYRTOS_SUCCESS)
            {
                myrtos_enable_interupts();
                return MYRTOS_FAIL;
            }

            //check if priority of this waiting task is greater than holding task
            #ifndef MYRTOS_ROUND_ROBIN
            if (s_curr_task_p->t.priority < h->t->t.priority)
            {
                //any edits to the task queue need to be atomic so interrupts must remain disabled
                myRTOS_return_type_e my_ret;
                //need to remove and replace
                my_ret = myrtos_remove_task(h->t);
                if (my_ret != MYRTOS_SUCCESS) return my_ret;
                //inherit priority value
                h->t->t.priority = s_curr_task_p->t.priority;
                //push back onto queue
                my_ret = myrtos_push_task(h->t);
                if (my_ret != MYRTOS_SUCCESS) return my_ret;
            }
            #endif

            //add to waiting list
            if (h->t_i == h->n)
            {
                //need to increase list size (this is slow and we want to avoid it because interrupts must still be disabled)
                myRTOS_int_task_type_vp* tmp = myrtos_lock_realloc((void*)h->t_l, sizeof(myRTOS_int_task_type_vp) * h->n * 2);
                if (NULL == tmp)
                {
                    myrtos_enable_interupts();
                    return MYRTOS_MEMORY_LIMIT_REACHED;
                }

                //if memory successfully realloc'd continue
                h->t_l = tmp;
                h->n = h->n*2;
            }
            h->t_l[h->t_i] = s_curr_task_p;
            h->t_i++;

            //flag scheduler interrupt
            myrtos_hal_set_hardware_timer_flag();
            myrtos_hal_timer_reset();
            myrtos_enable_interupts();

            #if 1 == MYRTOS_TESTING
            return MYRTOS_SUCCESS;
            #endif
        }
    }

    //should never reach
    return MYRTOS_FAIL;
}

/**
 * @brief Free mutex resource to be taken by other tasks
 * 
 * Mutex holds a list of waiting tasks, need to unblock all
 * Also need to ensure that original priority is returned on holding task
 * 
 * @param h pointer to mutex handle object
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_mutex_give(myRTOS_mutex_handle_s* h)
{
    if (NULL == h) return MYRTOS_FAIL;

    myrtos_disable_interupts();

    //ensure current task is not null, it should never actually be null here
    if (NULL == h->t || h->t != s_curr_task_p)
    {
        myrtos_enable_interupts();
        return MYRTOS_FAIL;
    }

    //iterate through waiting list and unblock all tasks
    for (size_t i = 0; i < h->t_i; i++)
    {
        if (myrtos_unblock_task(h->t_l[i]) != MYRTOS_SUCCESS)
        {
            myrtos_enable_interupts();
            return MYRTOS_FAIL;
        }
    }

    //ensure giving task returns to correct priority if inheritance occured
    //if the priority has lowered due to dynamic schedule we need to ensure that we don't increase when giving up mutex
    #ifdef MYRTOS_DYNAMIC_PRIORITY
    h->t->t.priority = (h->in_prio < h->t->t.priority) ? h->t->t.priority : h->in_prio;
    #else
    h->t->t.priority = h->in_prio;
    #endif

    //reset mutex to be taken again
    h->t = NULL;
    h->t_i = 0;

    myrtos_enable_interupts();
    return MYRTOS_SUCCESS;
}