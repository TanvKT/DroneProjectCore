/**
 * @file myRTOS_task_queue.c
 * @author your name (you@domain.com)
 * @brief Functions for controlling task queue
 *          Using a binary heap for task sorting based on priority
 * @version 0.1
 * @date 2025-08-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 /* Includes */
#include "myRTOS_task_queue.h"
#include "myRTOS_config.h"
#include "myRTOS_types.h"
#include "myRTOS_sched.h"
#include "myRTOS_memory.h"
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include <string.h>

/**
 * TASK HOLDING ARRAY
 * 
 * Memory block is preallocated with the worst case scenario in terms of maximum tasks possible
 *      The number of tasks possible is calculated using the minimum stack size for a task
 *      and the allocated stack space for those tasks
 */
static myRTOS_queue_arr_s* tasks;
static size_t n_tasks = 0;

/**
 * @brief Simple getter function for task queue, useful for testing
 * 
 * @return myRTOS_task_queue_s* 
 */
myRTOS_queue_arr_s* myrtos_get_task_queue()
{
    return tasks;
}

/**
 * @brief Initialize task queue to start at given reserved memory address
 * 
 * @param p start address of task heap block
 */
myRTOS_return_type_e myrtos_init_task_queue(void* p)
{
    if (!p) return MYRTOS_MEMORY_INVALID;
    tasks = (myRTOS_queue_arr_s*)p;
    n_tasks = 0;
    
    for (uint8_t i=0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        tasks->level[i].len = 0; //current length
        tasks->level[i].st  = 0; //current start index
        tasks->level[i].en  = 0; //current end index
                                 //start and end index are used to allow circular array so that tasks don't need to be shifted down
    }
    tasks->blocked.len = 0;

    return MYRTOS_SUCCESS;
}

/**
 * @brief Find first non-empty queue to remove first element
 *              Using a circular array for each queue allows us to avoid shifting contents of array
 * 
 * @param t task pointer to copy to
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_request_task(myRTOS_int_task_type_s* t)
{
    uint8_t i;
    for (i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        if (0 == tasks->level[i].len) continue;

        //not empty at this priority level, return and remove first task
        if (!memcpy(t, &tasks->level[i].arr[tasks->level[i].st], sizeof(myRTOS_int_task_type_s))) return MYRTOS_MEMCPY_FAIL;
        tasks->level[i].len--;
        tasks->level[i].st = (MYRTOS_MAX_TASKS-1 == tasks->level[i].st) ? 0 : tasks->level[i].st+1;  //ensure within ciruclar bounds
        break;
    }

    if (MYRTOS_QUEUE_ARR_LEN == i) return MYRTOS_TASK_QUEUE_EMPTY;

    return MYRTOS_SUCCESS;
}
/**
 * @brief Returns data of first task but does not remove
 *              Pretty much same logic as above
 * 
 * @param t task pointer to copy to
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_peek_task(myRTOS_int_task_type_s* t)
{
    uint8_t i;
    for (i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        if (0 == tasks->level[i].len) continue;

        //not empty at this priority level, return first task
        if (!memcpy(t, &tasks->level[i].arr[tasks->level[i].st], sizeof(myRTOS_int_task_type_s))) return MYRTOS_MEMCPY_FAIL;
        break;
    }

    if (MYRTOS_QUEUE_ARR_LEN == i) return MYRTOS_TASK_QUEUE_EMPTY;

    return MYRTOS_SUCCESS;
}

/**
 * @brief Push a registered task onto the task array
 * 
 *              No sanity checks are really done here as this should only be used with tasks
 *                  that have previously been registered
 * 
 * @param t pointer to task data
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_push_task(myRTOS_int_task_type_s* t)
{
    myRTOS_int_task_type_s* t_i = &tasks->level[t->t.priority].arr[tasks->level[t->t.priority].en];

    //copy task to end of array
    if (!memcpy(t_i, t, sizeof(myRTOS_int_task_type_s))) return MYRTOS_MEMCPY_FAIL;

    //incremement circular array values
    tasks->level[t->t.priority].len++;
    tasks->level[t->t.priority].en = (MYRTOS_MAX_TASKS-1 == tasks->level[t->t.priority].en) ? 0 : tasks->level[t->t.priority].en+1;

    return MYRTOS_SUCCESS;
}

/**
 * @brief Pushes a task into the blocked task queue
 * 
 * @param t pointer to task data to push
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_push_blocked_task(myRTOS_int_task_type_s* t)
{
    myRTOS_int_task_type_s* t_i;

    if (MYRTOS_MAX_TASKS == tasks->blocked.len) return MYRTOS_TASK_LIMIT_REACHED;  //a bit redundant here but leaving this check

    t_i = &tasks->blocked.arr[tasks->blocked.len]; //point to data at insert index of circular array 
                                                  //  at given priority level

    //copy task to end of array
    if (!memcpy(t_i, t, sizeof(myRTOS_int_task_type_s))) return MYRTOS_MEMCPY_FAIL;

    t_i->b_i = tasks->blocked.len;

    //incremement array values not using circular array values here
    tasks->blocked.len++;

    return MYRTOS_SUCCESS;
}

/**
 * @brief Simple getter for blocked list without needing to copy
 * 
 * @return myRTOS_task_queue_s* pointer to blocked list queue
 */
myRTOS_task_queue_s* myrtos_get_blocked_list_ptr()
{
    return &tasks->blocked;
}

/**
 * @brief Copies a maximum of n elements in blocked list to location pointed to by l
 * 
 * @param l pointer to array of tasks to copy to
 * @param n max number of elements to copy
 * @return size_t total number of elements copied
 */
size_t myrtos_get_blocked_list_cpy(myRTOS_int_task_type_s* l, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++)
    {
        if (tasks->blocked.len == i) return i;
        if (!memcpy(l, &tasks->blocked.arr[i], sizeof(myRTOS_int_task_type_s))) return i;
    }
    return n;
}

/**
 * @brief Remove a task from the blocked list at a given index
 * 
 * @param t pointer to location to copy data to
 * @param i index to remove at
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_rem_blocked_task(myRTOS_int_task_type_s* t, size_t i)
{
    if (tasks->blocked.len <= i) return MYRTOS_FAIL;

    if (!memcpy(t, &tasks->blocked.arr[i], sizeof(myRTOS_int_task_type_s))) return MYRTOS_MEMCPY_FAIL;

    //shift array down at index if needed
    if (((tasks->blocked.len-1) != i) || (1 == tasks->blocked.len))
    {
        if (!memcpy(&tasks->blocked.arr[i], &tasks->blocked.arr[i+1], 
            sizeof(myRTOS_int_task_type_s)*(tasks->blocked.len - i - 1))) return MYRTOS_MEMCPY_FAIL;
    }

    t->b_i = -1;

    //decrement length and return
    tasks->blocked.len--;
    return MYRTOS_SUCCESS;
}

/**
 * @brief Register task to myRTOS scheduler - This is the internal function layer
 * 
 * Here we register each task into a table based on the scheduling type
 *      -Round Robin does not consider priority levels so the allocation is linear
 *      -Priority Based requires that separate priority levels be allocated
 *              this means that we want to allocate mulitple arrays based on the given priority levels
 *      -Dynamic Priority requires that each priority level have enough space to hold every registered task
 *              with dynamic priority we are using the same storing logic as Priority Based
 * 
 * The reason for separating priority levels is to ensure that we can access tasks within a given priority while not
 *      having to search through an array or do minimal sorting while the scheduler is running
 * 
 * @param t task to register
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_register_task_i(myRTOS_task_type_s* t)
{
    myRTOS_int_task_type_s* t_i;

    if (MYRTOS_MAX_TASKS == n_tasks) return MYRTOS_TASK_LIMIT_REACHED;

    //ensure valid priority
    t->priority = (t->priority >= MYRTOS_QUEUE_ARR_LEN) ? MYRTOS_QUEUE_ARR_LEN-1 : t->priority;
    t_i = &tasks->level[t->priority].arr[tasks->level[t->priority].en]; //point to data at insert index of circular array 
                                                                        //  at given priority level

    #ifdef MYRTOS_DYNAMIC_PRIORITY
    //if we are using dynamic priority, we need to record original priority in a seperate field
    t_i->o_prio = t->priority;
    t_i->trig = 0;
    #endif

    //all tasks start unblocked
    t_i->b = false;
    t_i->b_i = -1;

    //make sure stack size at least MINIUMUM
    t->stack_size = (t->stack_size < MYRTOS_MIN_STACK_SIZE) ? MYRTOS_MIN_STACK_SIZE : t->stack_size;

    //ensure stack size is aligned with 8 bytes
    t->stack_size = (t->stack_size % 8) ? t->stack_size + (8 - (t->stack_size % 8)) : t->stack_size;

    //generate stack pointer for task
    t_i->sp = myrtos_add_stack(t->stack_size);
    if (t_i->sp == NULL) return MYRTOS_MEMORY_LIMIT_REACHED;

    //copy task to end of array
    if (!memcpy(&(t_i->t), t, sizeof(myRTOS_task_type_s))) return MYRTOS_MEMCPY_FAIL;

    if (!myrtos_hal_stack_setup(t_i)) return MYRTOS_FAIL;

    //incremement circular array values
    tasks->level[t->priority].len++;
    tasks->level[t->priority].en = (MYRTOS_MAX_TASKS-1 == tasks->level[t->priority].en) ? 0 : tasks->level[t->priority].en+1;

    n_tasks++;
    return MYRTOS_SUCCESS;
}

/**
 * @brief Register a task via parameters
 * 
 *      This is the user accesible function from myRTOS.h
 * 
 * @param name name of task for debug purposes (will be truncated if higher than MYRTOS_TASK_NAME_LEN)
 * @param priority uint8_t priority value (lower is higher priority)
 * @param handle function handle for task code execution
 * @param args pointer to args of function (must be static or global, i.e. remain in scope while task is running)
 * @param stack_size total size of stack to allocate (if lower than minimum will be set to MYRTOS_MIN_STACK_SIZE)
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_register_task
(const char* name, uint8_t priority, void* handle, void* args, size_t stack_size)
{
    myRTOS_task_type_s t = {.priority = priority, .handle = handle, .args = args, .stack_size = stack_size};
    strcpy(t.name, name);

    return myrtos_register_task_i(&t);
}