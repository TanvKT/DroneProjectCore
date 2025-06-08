/**
 * @file myRTOS_init.c
 * @author Tanvin Thiagarajan (tthiagarajan@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-06-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "myRTOS.h"
#include "myRTOS_heap.h"
#include "memory.h"

/**
 * @brief array type to assist in dynamic allocation of tasks
 * 
 */
typedef struct
{
    int len;
    int entries;
    myRTOS_task_type_s* arr;
} myRTOS_task_arr_s;

/**
 * @brief GLOBALS
 * 
 */
/**
 * TASK HOLDING ARRAY
 * 
 * Structured as fixed array of dynamic arrays of tasks
 * Index in outer array is based on priority of task
 * Index in inner array is dynamically allocated based on number of tasks with said priority
 * Using C stdlib here for task allocations since tasks need to be initialized before we begin running the scheduler
 * 
 * If the array is filled then the array is re-allocated with double length
 */
myRTOS_task_arr_s tasks[PRIORITY_LEVELS - 1];

/**
 * @brief helper function to add tasks to an array
 * 
 * @param a array to add to
 * @param t task data to copy
 * 
 * @return myRTOS_return_type_e 
 */
static myRTOS_return_type_e arr_add(myRTOS_task_arr_s* a, myRTOS_task_type_s* t)
{
    //array size check
    if (a->entries == a->len)
    {
        a->arr = realloc(a->arr, sizeof(myRTOS_task_type_s) * a->len * 2);
        a->len = a->len * 2;
    }

    //copy over values
    memcpy(&(a->arr[a->entries]), t, sizeof(myRTOS_task_type_s));
    a->entries++;
}

/**
 * @brief default initialization function for myrtos
 * 
 * Initialize MyRTOS stack and management data
 * 
 * @param c config values for MyRTOS program, if null will use defaults
 * 
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_init()
{
    //initialize heap for dynamic task allocation
    myrtos_heap_init(HEAP_SIZE);

    //allocate a dynamic array using c stdlib for holding registered tasks with priority
    for (int i = 0; i < PRIORITY_LEVELS; i++)
    {
        tasks[i].arr = malloc(sizeof(myRTOS_task_type_s) * INIT_TASK_ARR_SIZE);
        tasks[i].len = INIT_TASK_ARR_SIZE;
        tasks[i].entries = 0;
    }
}

/**
 * @brief Register task to myRTOS scheduler
 * 
 * Task being passed in MUST exist in memory throughout entirety of function call
 * 
 * @param t task to register
 * @return myRTOS_return_type_e 
 */
myRTOS_return_type_e myrtos_register_task(myRTOS_task_type_s* t)
{
    switch(SCHED_TYPE)
    {
        case 0: //round robin
            //register each task to same priority level
            arr_add(&tasks[0], t);
        break;
        default: //either priority based scheduler
            //register at desired priority level (truncate if not within range)
            arr_add(&tasks[(t->priority >= PRIORITY_LEVELS) ? PRIORITY_LEVELS - 1 : t->priority], t);
        break;
    }
}
