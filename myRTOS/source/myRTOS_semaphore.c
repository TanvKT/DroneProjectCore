/**
 * @file myRTOS_semaphore.c
 * @author your name (you@domain.com)
 * @brief 
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
 * SEMAPHORE
 */

/**
 * @brief Initialize a semaphore object
 * 
 * Waiting list is initialized to half of holding list length
 * 
 * The semaphore take count can be negative
 * 
 * @param h pointer to handle
 * @param n semaphore take count
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_semaphore_init(myRTOS_semaphore_handle_s* h, int8_t n)
{
    if (NULL == h) return MYRTOS_FAIL;
    h->c = n;
    n = (n <= 0) ? 1 : n;

    //initialize holding list
    h->t = myrtos_lock_alloc(sizeof(myRTOS_int_task_type_vp) * n);
    if (NULL == h->t) return MYRTOS_MEMORY_LIMIT_REACHED;
    h->in_prio = myrtos_lock_alloc(sizeof(uint8_t) * n);
    if (NULL == h->in_prio) return MYRTOS_MEMORY_LIMIT_REACHED;
    h->t_i = 0;
    h->t_n = n;

    //initialize waiting list to double number of tasks of holding list by default
    h->t_l = myrtos_lock_alloc(sizeof(myRTOS_int_task_type_vp) * n * 2);
    if (NULL == h->t) return MYRTOS_MEMORY_LIMIT_REACHED;
    h->t_l_i = 0;
    h->t_l_n = 2*n;

    return MYRTOS_SUCCESS;
}

/**
 * @brief Initialize a semaphore object
 * 
 * The semaphore take count can be negative
 * 
 * @param h pointer to handle
 * @param n semaphore take count
 * @param w waiting list initial size
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_semaphore_init_num(myRTOS_semaphore_handle_s* h, int8_t n, size_t w)
{
    if (NULL == h) return MYRTOS_FAIL;
    h->c = n;
    n = (n <= 0) ? 1 : n;

    //initialize holding list
    h->t = myrtos_lock_alloc(sizeof(myRTOS_int_task_type_vp) * n);
    if (NULL == h->t) return MYRTOS_MEMORY_LIMIT_REACHED;
    h->in_prio = myrtos_lock_alloc(sizeof(uint8_t) * n);
    if (NULL == h->in_prio) return MYRTOS_MEMORY_LIMIT_REACHED;
    h->t_i = 0;
    h->t_n = n;

    //initialize waiting list to double number of tasks of holding list by default
    h->t_l = myrtos_lock_alloc(sizeof(myRTOS_int_task_type_vp) * w);
    if (NULL == h->t) return MYRTOS_MEMORY_LIMIT_REACHED;
    h->t_l_i = 0;
    h->t_l_n = w;

    return MYRTOS_SUCCESS;
}

/**
 * @brief Takes the semaphore resource
 * 
 * Disabled global interrupts during this function
 *      this can result in latency which can be mitigated by ensuring waiting list is properly sized
 * 
 * If the count value is zero or less, the task attempting the take will be blocked
 * 
 * Priority inheritance is also enabled such that if a high priorty task ends up blocked, all holding tasks get their priority boosted if
 *          their priority is less than the requesting task
 * 
 * @param h pointer to semaphore handle
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_semaphore_take(myRTOS_semaphore_handle_s* h)
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

        //check semaphore status
        if (h->c > 0)
        {
            //semaphore can be taken
            //don't need atomic instructions here since we are disabling interrupts for short critical section
            h->t[h->t_i] = s_curr_task_p;
            h->in_prio[h->t_i] = s_curr_task_p->t.priority;
            h->t_i++;   //note: there are no checks for overflow here since count should never allow more tasks than possible to be added
            h->c--;     //decrement count on successful take

            //success so we return
            myrtos_enable_interupts();
            return MYRTOS_SUCCESS;
        }
        else //semaphore cannot be taken
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

            //check if priority of this waiting task is greater than any holding task
            #ifndef MYRTOS_ROUND_ROBIN
            for (uint8_t i = 0; i < h->t_n; i++)
            {
                if (s_curr_task_p->t.priority < h->t[i]->t.priority)
                {
                    //any edits to the task queue need to be atomic so interrupts must remain disabled
                    myRTOS_return_type_e my_ret;
                    //need to remove and replace
                    my_ret = myrtos_remove_task(h->t[i]);
                    if (my_ret != MYRTOS_SUCCESS) return my_ret;
                    //inherit priority value
                    h->t[i]->t.priority = s_curr_task_p->t.priority;
                    //push back onto queue
                    my_ret = myrtos_push_task(h->t);
                    if (my_ret != MYRTOS_SUCCESS) return my_ret;
                }
            }
            #endif

            //add to waiting list
            if (h->t_l_i == h->t_l_n)
            {
                //need to increase list size (this is slow and we want to avoid it because interrupts must still be disabled)
                myRTOS_int_task_type_vp* tmp = myrtos_lock_realloc((void*)h->t_l, sizeof(myRTOS_int_task_type_vp) * h->t_l_n * 2);
                if (NULL == tmp)
                {
                    myrtos_enable_interupts();
                    return MYRTOS_MEMORY_LIMIT_REACHED;
                }

                //if memory successfully realloc'd continue
                h->t_l = tmp;
                h->t_l_n = h->t_l_n*2;
            }
            h->t_l[h->t_l_i] = s_curr_task_p;
            h->t_l_i++;

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
 * @brief Give up semaphore
 * 
 * Interrupts disabled throughout entire process
 * Semaphore can be given by non-holding task if and only if count is negative
 *      This only occurs if semaphore initialized to a negative value
 * If priority inheritance has occured, all holding tasks will remain at higher priority in scheduler until they give up the resource
 *      even if the task that caused the priority increase is no longer in the waiting list
 * 
 * @param h pointer to semaphore handle
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_semaphore_give(myRTOS_semaphore_handle_s* h)
{
    if (NULL == h) return MYRTOS_FAIL;

    //edge case where count is not zero, we just increment and return
    if (h->c != 0)
    {
        h->c++;
        return MYRTOS_SUCCESS;
    }

    myrtos_disable_interupts();

    //iterate through holding tasks to check if giver is current task
    uint8_t give_i;
    for (give_i = 0; give_i < h->t_n; give_i++)
    {
        if (h->t[give_i] == s_curr_task_p) break;
    }
    if (give_i == h->t_n)
    {
        myrtos_enable_interupts();
        return MYRTOS_FAIL;
    }

    //iterate through waiting list to find highest priority task and unblock
    uint8_t highest_p = MYRTOS_PRIORITY_LEVELS;
    size_t highest_i = 0;
    for (size_t i = 0; i < h->t_l_i; i++)
    {
        if (highest_p > h->t_l[i]->t.priority)
        {
            highest_p = h->t_l[i]->t.priority;
            highest_i = i;
        }
    }
    if (myrtos_unblock_task(h->t_l[highest_i]) != MYRTOS_SUCCESS)
    {
        myrtos_enable_interupts();
        return MYRTOS_FAIL;
    }
    //need to repair waiting list
    for (size_t i = highest_i; i < h->t_l_i - 1; i++)
    {
        h->t_l[i] = h->t_l[i+1];
    }
    h->t_l_i--;

    //ensure giving task returns to correct priority if inheritance occured
    //if the priority has lowered due to dynamic schedule we need to ensure that we don't increase when giving up mutex
    //important to note that if priority inheritance did occur, then all holding tasks will be held at inherited priority until they give up the semaphore

    #ifdef MYRTOS_DYNAMIC_PRIORITY
    h->t[give_i]->t.priority = (h->in_prio[give_i] < h->t[give_i]->t.priority) ? h->t[give_i]->t.priority : h->in_prio[give_i];
    #else
    h->t[give_i]->t.priority = h->in_prio[give_i];
    #endif
    //remove task from semaphore list
    for (uint8_t i = give_i; i < h->t_n - 1; i++)
    {
        h->t[i] = h->t[i+1];
    }
    h->t_i--;

    //increment semaphore to be taken again
    h->c++;
    
    myrtos_enable_interupts();
    return MYRTOS_SUCCESS;
}