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
static myRTOS_task_queue_s* tasks;
#ifndef MYRTOS_ROUND_ROBIN
static bool sorted = false;
#else
static int task_idx = 0;
#endif

/**
 * @brief Simple getter function for task queue, useful for testing
 * 
 * @return myRTOS_task_queue_s* 
 */
myRTOS_task_queue_s* myrtos_get_task_queue()
{
    return tasks;
}

/**
 * @brief Initialize task queue to start at given reserved memory address
 * 
 * @param p start address of task heap block
 */
void myrtos_init_task_queue(void* p)
{
    tasks = (myRTOS_task_queue_s*)p;
    tasks->len = 0;
    tasks->arr = (myRTOS_int_task_type_s*)((void*)tasks + sizeof(myRTOS_queue_handle_s));
}

/**
 * @brief If not using round robin need to sort the array based on the priority to use it as a binary-heap priority queue
 * 
 */
#ifndef MYRTOS_ROUND_ROBIN
/**
 * @brief Used to pass to qsort for quick sort comparison
 * 
 * @param a task1
 * @param b task2
 * @return int difference
 */
static int compare_priority(const void* a, const void* b)
{
    myRTOS_int_task_type_s* t1 = (myRTOS_int_task_type_s*)a;
    myRTOS_int_task_type_s* t2 = (myRTOS_int_task_type_s*)b;
    return t1->t.priority - t2->t.priority;
}
void myrtos_sort_tasks()
{
    //sort the array using qsort
    qsort(tasks->arr, tasks->len, sizeof(myRTOS_int_task_type_s), compare_priority);
}
#else
void myrtos_sort_tasks()
{
    //do nothing
    return;
}
#endif

/**
* @brief
*       With a simple round robin scheduling we are just going to linearly move through array
*       The overhead cost of simply skipping a blocked task is less than if we used a queue to keep track of tasks
*           since everything has the same priority * 
*/
#ifndef MYRTOS_ROUND_ROBIN
/**
 * @brief Helper function to swap two tasks in memory
 * 
 * @param t1 task 1
 * @param t2 task 2
 */
static void swap(myRTOS_int_task_type_s* t1, myRTOS_int_task_type_s* t2)
{
    myRTOS_int_task_type_s tmp = *t1;
    *t1 = *t2;
    *t2 = tmp;
}
/**
 * @brief Used to insert new node at root of priority queue
 * 
 * Assumes task is inserted at end of queue
 * Compare priority with parent until proper placement found
 * Need to ensure that when popping and replacing a task, if there are tasks of the same priority
 *          at the top of the queue, we place it at the end of that priority level
 * 
 * @param i index to insert at (used for recursive calls)
 */
static void heap_insert(int i)
{
    int p;

    if (i == 0) return; //at root
    p = (i - 1) / 2; //get parent index
    if (tasks->arr[i].t.priority < tasks->arr[p].t.priority)
    {
        //if the priority value is lower (higher priority) then we swap and check again
        swap(&tasks->arr[i], &tasks->arr[p]);
        heap_insert(p);
    }
}

/**
 * @brief Used to sort the binary heap after the root of the priority queue is removed
 * 
 * @param i index of node to check
 */
static void heap_remove(int i)
{
    int l = i;
    int left = 2*i + 1;
    int right = 2*i + 2;

    //if either the left or right child has a lower priority value (higher priority) than the root, swap and recurse
    if ((left < tasks->len) && (tasks->arr[left].t.priority < tasks->arr[l].t.priority))
        l = left;  
    if ((right < tasks->len) && (tasks->arr[right].t.priority < tasks->arr[l].t.priority))
        l = right;

    if (l != i)
    {
        swap(&tasks->arr[i], &tasks->arr[l]);
        heap_remove(l);
    }
}

/**
 * @brief Push to priority queue
 * 
 * @param t task to add
 * @return myRTOS_return_type_e 
 */
static inline myRTOS_return_type_e push(myRTOS_int_task_type_s* t)
{
    if (MYRTOS_MAX_TASKS == tasks->len) return MYRTOS_TASK_LIMIT_REACHED;  //redundancy here to ensure no stack corruptions

    myRTOS_int_task_type_s* t_i = &tasks->arr[tasks->len];
    if (!memcpy(t_i, t, sizeof(myRTOS_int_task_type_s))) return MYRTOS_MEMCPY_FAIL;
    //quick check to ensure valid priority
    t_i->t.priority = (t_i->t.priority >= MYRTOS_PRIORITY_LEVELS) ? MYRTOS_PRIORITY_LEVELS : t_i->t.priority;
    //reconstruct heap
    heap_insert(tasks->len);
    tasks->len++;

    return MYRTOS_SUCCESS;
}

/**
 * @brief Pop the root off the queue
 * 
 * @param t pointer to task memory to copy to
 * @return myRTOS_return_type_e 
 */
static inline myRTOS_return_type_e pop(myRTOS_int_task_type_s* t)
{
    if (0 == tasks->len) return MYRTOS_TASK_QUEUE_EMPTY;

    //copy the root into return value then swap last to the root
    if (!memcpy(t, &tasks->arr[0], sizeof(myRTOS_int_task_type_s))) return MYRTOS_MEMCPY_FAIL;
    if (!memcpy(&tasks->arr[0], &tasks->arr[tasks->len - 1], sizeof(myRTOS_int_task_type_s))) return MYRTOS_MEMCPY_FAIL;

    //consolidate heap after removal
    heap_remove(0);
    return MYRTOS_SUCCESS;
}

/**
 * @brief Peek the top of the priority queue
 * 
 * @param t pointer to task memory to copy to
 * @return myRTOS_return_type_e 
 */
static inline myRTOS_return_type_e peek(myRTOS_int_task_type_s* t)
{
    if (0 == tasks->len) return MYRTOS_TASK_QUEUE_EMPTY;
    if (!memcpy(t, &tasks->arr[0], sizeof(myRTOS_int_task_type_s))) return MYRTOS_MEMCPY_FAIL;

    return MYRTOS_SUCCESS;
}

/**
 * @brief Pops the first task on the priority queue
 * 
 * @param t task pointer to copy to
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_request_task(myRTOS_int_task_type_s* t)
{
    return pop(t);
}
/**
 * @brief Peek first task on priority queue
 * 
 * @param t task pointer to copy to
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_peek_task(myRTOS_int_task_type_s* t)
{
    return peek(t);
}
/**
 * @brief Push a task back onto the priority queue
 * 
 * @param t task pointer to copy to
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_push_task(myRTOS_int_task_type_s* t)
{
    return push(t);
}
#else
/**
 * @brief grab current task in round robin schedule
 * 
 * @param t task pointer to copy to
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_request_task(myRTOS_int_task_type_s* t)
{
    if (!memcpy(t, &tasks->arr[task_idx], sizeof(myRTOS_int_task_type_s))) return MYRTOS_MEMCPY_FAIL;
    task_idx = (task_idx == tasks->len - 1) ? 0 : task_idx + 1;
    return MYRTOS_SUCCESS;
}
/**
 * @brief same functionality as request
 * 
 * @param t task pointer to copy to
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_peek_task(myRTOS_int_task_type_s* t)
{
    if (!memcpy(t, &tasks->arr[task_idx], sizeof(myRTOS_int_task_type_s))) return MYRTOS_MEMCPY_FAIL;
    task_idx = (task_idx == tasks->len - 1) ? 0 : task_idx + 1;
    return MYRTOS_SUCCESS;
}
/**
 * @brief unused
 * 
 * @param t task pointer to copy to
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_push_task(myRTOS_int_task_type_s* t)
{
    return MYRTOS_SUCCESS;  //unused
}
#endif

/**
 * @brief Register task to myRTOS scheduler
 * 
 * Here we register each task into a table based on the scheduling type
 *      -Round robin does not consider priority levels so the allocation is linear
 *      -Priority based requires that separate priority levels be allocated
 *              this means that we want to allocate mulitple arrays based on the given priority levels
 *      -Dynamic priority requires that each priority level have enough space to hold every registered task
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

    if (MYRTOS_MAX_TASKS == tasks->len) return MYRTOS_TASK_LIMIT_REACHED;
    t_i = &tasks->arr[tasks->len];

    //make sure stack size at least MINIUMUM
    t->stack_size = (t->stack_size < MYRTOS_MIN_STACK_SIZE) ? MYRTOS_MIN_STACK_SIZE : t->stack_size;

    //generate stack pointer for task
    t_i->sp = myrtos_add_stack(t->stack_size);
    if (t_i->sp == NULL) return MYRTOS_MEMORY_LIMIT_REACHED;

    #ifdef MYRTOS_ROUND_ROBIN
    //register tasks in linear array
    if (!memcpy(t_i, t, sizeof(myRTOS_task_type_s))) return MYRTOS_MEMCPY_FAIL;
    #else
    //quick check to ensure valid priority
    t_i->t.priority = (t_i->t.priority >= MYRTOS_PRIORITY_LEVELS) ? MYRTOS_PRIORITY_LEVELS : t_i->t.priority;
    //functionality depends on if the array has been sorted or not
    if (!sorted)
    {
        //register tasks in linear array, sorting will be done on invocation of scheduler
        if (!memcpy(t_i, t, sizeof(myRTOS_task_type_s))) return MYRTOS_MEMCPY_FAIL;
        tasks->len++;
    }
    else
    {
        //scheduler has been started once already, need to maintain sorted queue for tasks added during runtime
        return push(t_i);
    }
    #endif

    tasks->len++;
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