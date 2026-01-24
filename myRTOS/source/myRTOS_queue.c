/**
 * @file myRTOS_queue.c
 * @author your name (you@domain.com)
 * @brief Queue implementation
 * 
 *          Queues can be resized within running tasks
 *          Similar to semaphores and mutexes, interrupts are disabled when operations regarding queue modifcation
 *              or task array modification (blocking/unblocking)
 *          We keep track of blocked producer, consumer tasks in dynamic array similar to the other locking systems
 *          This array will dynamically re-alloc if needed, but this could result in slow performance, so is not advised
 * 
 *          No priority inheritance occurs here, and tasks will infinitely block when waiting
 *              There is no timeout for producers or consumers in myRTOS
 * 
 *          The internal structure of the queue is a FIFO circular array
 *          
 * 
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
 * @brief Initialize queue
 * 
 * Initializes with a specific length and data size
 * 
 * Default size of waiting lists to 2
 * 
 * @param h pointer queue handle type
 * @param l length of queue
 * @param d size of individual data object
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_queue_init(myRTOS_queue_handle_s* h, size_t l, size_t d)
{
    if (NULL == h) return MYRTOS_FAIL;

    h->d = myrtos_lock_alloc(l * d);
    if (NULL == h->d) return MYRTOS_MEMORY_LIMIT_REACHED;
    h->st = 0;
    h->en = 0;
    h->cnt = 0;
    h->len = l;
    h->inc = d;

    h->t_c = myrtos_alloc(sizeof(myRTOS_int_task_type_vp) * 2);
    if (NULL == h->t_c) return MYRTOS_MEMORY_LIMIT_REACHED;
    h->t_c_i = 0;
    h->t_c_n = 2;

    h->t_p = myrtos_alloc(sizeof(myRTOS_int_task_type_vp) * 2);
    if (NULL == h->t_p) return MYRTOS_MEMORY_LIMIT_REACHED;
    h->t_p_i = 0;
    h->t_p_n = 2;

    return MYRTOS_SUCCESS;
}
/**
 * @brief Initialize queue
 * 
 *          Initialize with specific waiting list sizes
 * 
 * @param h pointer queue handle type
 * @param l length of queue
 * @param d size of individual data object
 * @param wc consumer task waiting list length
 * @param wp producer task waiting list length
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_queue_init_num(myRTOS_queue_handle_s* h, size_t l, size_t d, size_t wc, size_t wp)
{
    if (NULL == h) return MYRTOS_FAIL;

    h->d = myrtos_lock_alloc(l * d);
    if (NULL == h->d) return MYRTOS_MEMORY_LIMIT_REACHED;
    h->st = 0;
    h->en = 0;
    h->cnt = 0;
    h->len = l;
    h->inc = d;

    h->t_c = myrtos_alloc(sizeof(myRTOS_int_task_type_vp) * wc);
    if (NULL == h->t_c) return MYRTOS_MEMORY_LIMIT_REACHED;
    h->t_c_i = 0;
    h->t_c_n = wc;

    h->t_p = myrtos_alloc(sizeof(myRTOS_int_task_type_vp) * wp);
    if (NULL == h->t_p) return MYRTOS_MEMORY_LIMIT_REACHED;
    h->t_p_i = 0;
    h->t_p_n = wp;

    return MYRTOS_SUCCESS;
}

/**
 * @brief Dynamically resize the queue
 * 
 * @param h pointer queue handle type
 * @param l length of queue
 * @param d size of individual data object
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_queue_resize(myRTOS_queue_handle_s* h, size_t l, size_t d)
{
    if (NULL == h) return MYRTOS_FAIL;

    //need to disable interrupts to avoid queue being modified during re-allocation
    myrtos_disable_interupts();
    h->d = myrtos_realloc(h->d, l * d);
    if (NULL == h->d)
    {
        h->st = 0;
        h->en = 0;
        h->len = 0;
        myrtos_enable_interupts();
        return MYRTOS_MEMCPY_FAIL;
    }

    h->len = l;
    h->inc = d;
    myrtos_enable_interupts();
    return MYRTOS_SUCCESS;
}

/**
 * @brief Send data to queue
 * 
 *          If queue full, block indefinitely
 * 
 * @param h pointer to queue handle
 * @param d pointer to data to copy from
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_queue_send(myRTOS_queue_handle_s* h, void* d)
{
    if (NULL == h) return MYRTOS_FAIL;

    //infinite loop until data sent
    for (;;)
    {
        myrtos_disable_interupts();
        
        //check if full (en is at index before start)
        if (h->cnt == h->len)
        {
            //queue is full, need to block task until queue has space for add
            if (myrtos_block_task(s_curr_task_p) != MYRTOS_SUCCESS)
            {
                myrtos_enable_interupts();
                return MYRTOS_FAIL;
            }

            //check for need of dynamic re-alloc on producer waiting list
            if (h->t_p_i == h->t_p_n)
            {
                void* tmp = myrtos_realloc((void*)h->t_p, sizeof(myRTOS_int_task_type_vp) * 2 * h->t_p_n);
                if (NULL == tmp)
                {
                    myrtos_enable_interupts();
                    return MYRTOS_MEMORY_LIMIT_REACHED;
                }
                h->t_p = tmp;
                h->t_p_n = h->t_p_n * 2;
            }
            h->t_p[h->t_p_i] = s_curr_task_p;
            h->t_p_i++;

            //invoke scheduler and continue looping
            #if 0 == MYRTOS_TESTING
            myrtos_hal_set_hardware_timer_flag();
            myrtos_hal_timer_reset();
            #endif
            myrtos_enable_interupts();
            #if 1 == MYRTOS_TESTING
            return MYRTOS_SUCCESS;
            #endif
            continue;
        }
        //else we break out of loop
        break;
    }

    //add to queue
    if (NULL == memcpy(&(h->d[h->en]), d, h->inc))
    {
        myrtos_enable_interupts();
        return MYRTOS_MEMCPY_FAIL;
    }
    h->en = (h->en == (h->len-1)) ? 0 : h->en + 1;
    h->cnt++;

    //if there are waiting consumer tasks we need to unblock highest priority and invoke scheduler
    if (h->t_c_i != 0)
    {
        size_t highest_idx = 0;
        uint8_t highest_prio = MYRTOS_PRIORITY_LEVELS;
        for (size_t i = 0; i < h->t_c_i; i++)
        {
            if (highest_prio > h->t_c[i]->t.priority)
            {
                highest_prio = h->t_c[i]->t.priority;
                highest_idx = i;
            }
        }
        //unblock and repair
        if (myrtos_unblock_task(h->t_c[highest_idx]) != MYRTOS_SUCCESS)
        {
            myrtos_enable_interupts();
            return MYRTOS_FAIL;
        }
        for (size_t i = highest_idx; i < h->t_c_i - 1; i++)
        {
            h->t_c[i] = h->t_c[i+1];
        }
        //set scheduler flag
        #if 0 == MYRTOS_TESTING
        myrtos_hal_set_hardware_timer_flag();
        myrtos_hal_timer_reset();
        #endif
    }

    //enable interrupts and continue
    myrtos_enable_interupts();
    return MYRTOS_SUCCESS;
}

/**
 * @brief Recieve data from queue
 * 
 *          If queue empty, block indefinitely
 * 
 * @param h pointer to queue handle
 * @param d pointer to data to copy into
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_queue_recieve(myRTOS_queue_handle_s* h, void* d)
{
    if (NULL == h) return MYRTOS_FAIL;

    //infinite loop until data sent
    for (;;)
    {
        myrtos_disable_interupts();
        
        //check if empty (en is at same index as start)
        if (0 == h->cnt)
        {
            //queue is empty, need to block task until queue has space for add
            if (myrtos_block_task(s_curr_task_p) != MYRTOS_SUCCESS)
            {
                myrtos_enable_interupts();
                return MYRTOS_FAIL;
            }

            //check for need of dynamic re-alloc on consumer waiting list
            if (h->t_c_i == h->t_c_n)
            {
                void* tmp = myrtos_realloc((void*)h->t_c, sizeof(myRTOS_int_task_type_vp) * 2 * h->t_c_n);
                if (NULL == tmp)
                {
                    myrtos_enable_interupts();
                    return MYRTOS_MEMORY_LIMIT_REACHED;
                }
                h->t_c = tmp;
                h->t_c_n = h->t_c_n * 2;
            }
            h->t_p[h->t_c_i] = s_curr_task_p;
            h->t_c_i++;

            //invoke scheduler and continue looping
            #if 0 == MYRTOS_TESTING
            myrtos_hal_set_hardware_timer_flag();
            myrtos_hal_timer_reset();
            #endif
            myrtos_enable_interupts();
            #if 1 == MYRTOS_TESTING
            return MYRTOS_SUCCESS;
            #endif
            continue;
        }
        //else we break out of loop
        break;
    }

    //remove from queue
    if (NULL == memcpy(d, &(h->d[h->st]), h->inc))
    {
        myrtos_enable_interupts();
        return MYRTOS_MEMCPY_FAIL;
    }
    h->st = (h->st == (h->len - 1)) ? 0 : h->st + 1;
    h->cnt--;

    //if there are waiting producer tasks we need to unblock highest priority and invoke scheduler
    if (h->t_p_i != 0)
    {
        size_t highest_idx = 0;
        uint8_t highest_prio = MYRTOS_PRIORITY_LEVELS;
        for (size_t i = 0; i < h->t_p_i; i++)
        {
            if (highest_prio > h->t_p[i]->t.priority)
            {
                highest_prio = h->t_p[i]->t.priority;
                highest_idx = i;
            }
        }
        //unblock and repair
        if (myrtos_unblock_task(h->t_p[highest_idx]) != MYRTOS_SUCCESS)
        {
            myrtos_enable_interupts();
            return MYRTOS_FAIL;
        }
        for (size_t i = highest_idx; i < h->t_p_i - 1; i++)
        {
            h->t_p[i] = h->t_p[i+1];
        }
        //set scheduler flag
        #if 0 == MYRTOS_TESTING
        myrtos_hal_set_hardware_timer_flag();
        myrtos_hal_timer_reset();
        #endif
    }

    //enable interrupts and continue
    myrtos_enable_interupts();
    return MYRTOS_SUCCESS;
}