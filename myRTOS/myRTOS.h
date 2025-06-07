/**
 * @file myRTOS.h
 * @author Tanvin Thiagarajan (tthiagarajan@wisc.edu)
 * @brief 
 * @version 0.1
 * @date 2025-05-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/**
 * @brief Includes
 * 
 */
#include "myRTOS_types.h"

/**
 * @brief MyRTOS config MACROS
 * 
 */
 #define    HEAP_SIZE           2048    //default heap size 2KB
 #define    SCHED_TYPE          0       //0 - ROUND ROBIN       -       Every task is allotted equal CPU time
                                        //1 - PRIORITY-BASED    -       Tasks of higher priority will get more CPU time
                                        //                              If two tasks have the same priority, round robin is used
                                        //2 - DYNAMIC-PRIORITY  -       Tasks taking larger chunks of slice will have their priority lowered
#define     PRIORITY_LEVELS     7       //default to 7 levels of priority (0 - 6) anything larger is truncated down
#define     INIT_TASK_ARR_SIZE  4       //default allocate array of size 4 tasks

/**
 * @brief MyRTOS initialization and registration functions
 * 
 */
myRTOS_return_type_e    myrtos_init();
myRTOS_return_type_e    myrtos_register_task(myRTOS_task_type_s* t);

/**
 * @brief MyRTOS scheduling
 * 
 */
myRTOS_return_type_e    myrtos_schedule_start();
myRTOS_return_type_e    myrtos_schedule_stop();

/**
 * @brief MyRTOS inter task communication
 * 
 */
/**
 * MUTEX
 */
myRTOS_return_type_e    myrtos_mutex_init(myRTOS_mutex_handle_s* h);
myRTOS_return_type_e    myrtos_mutex_take(myRTOS_mutex_handle_s* h);
myRTOS_return_type_e    myrtos_mutex_give(myRTOS_mutex_handle_s* h);
/**
 * SEMAPHORE
 */
myRTOS_return_type_e    myrtos_semaphore_init(myRTOS_semaphore_handle_s* h);
myRTOS_return_type_e    myrtos_semaphore_take(myRTOS_semaphore_handle_s* h);
myRTOS_return_type_e    myrtos_semaphore_give(myRTOS_semaphore_handle_s* h);
/**
 * QUEUE
 */
myRTOS_queue_handle_s   myrtos_queue_set(int l, size_t s);
myRTOS_return_type_e    myrtos_queue_init(myRTOS_queue_handle_s* h);
myRTOS_return_type_e    myrtos_queue_send(myRTOS_queue_handle_s* h, void* d);
myRTOS_return_type_e    myrtos_queue_recieve(myRTOS_queue_handle_s* h, void* d);

/**
 * @brief MyRTOS memory management
 * 
 */
void*                   myrtos_alloc(size_t s);
void*                   myrtos_realloc(void* p, size_t s);
myRTOS_return_type_e    myrtos_free(void* p);