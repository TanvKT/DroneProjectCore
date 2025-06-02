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
 * 
 * If the array is filled then the array is re-allocated with double length
 */
myRTOS_task_arr_s tasks[PRIORITY_LEVELS];

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
        a->arr = myrtos_realloc(a->arr, sizeof(myRTOS_task_type_s) * a->len * 2);
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

    //allocate a dynamic array for holding registered tasks with priority
    for (int i = 0; i < PRIORITY_LEVELS; i++)
    {
        tasks[i].arr = myrtos_alloc(sizeof(myRTOS_task_type_s) * INIT_TASK_ARR_SIZE);
        tasks[i].len = INIT_TASK_ARR_SIZE;
        tasks[i].entries = 0;
    }
}
myRTOS_return_type_e myrtos_register_task(myRTOS_task_type_s* t)
{

}
myRTOS_return_type_e myrtos_modify_task(myRTOS_task_type_s* t)
{

}
