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

#ifndef __MYRTOS_H__
#define __MYRTOS_H__

#include "myRTOS_types.h"
#include "myRTOS_config.h"

/**
 * @brief MyRTOS initialization and registration and debug functions
 * 
 */
myRTOS_return_type_e    myrtos_init();
myRTOS_return_type_e    myrtos_register_task(myRTOS_task_type_s* t);
const char*             myrtos_debug_print(myRTOS_return_type_e r);

/**
 * @brief MyRTOS scheduling
 * 
 */
myRTOS_return_type_e    myrtos_schedule_start();
myRTOS_return_type_e    myrtos_schedule_stop();
myRTOS_return_type_e    myrtos_enable_interupts();
myRTOS_return_type_e    myrtos_disable_interupts();

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
myRTOS_return_type_e    myrtos_queue_set(myRTOS_queue_handle_s* h, int l, size_t s);
myRTOS_return_type_e    myrtos_queue_init(myRTOS_queue_handle_s* h);
myRTOS_return_type_e    myrtos_queue_send(myRTOS_queue_handle_s* h, void* d);
myRTOS_return_type_e    myrtos_queue_recieve(myRTOS_queue_handle_s* h, void* d);

/**
 * @brief MyRTOS memory management
 * 
 */
myRTOS_return_type_e    myrtos_heap_init();
void*                   myrtos_alloc(size_t s);
void*                   myrtos_realloc(void* p, size_t c, size_t s);
myRTOS_return_type_e    myrtos_free(void* p);
void                    myrtos_print_heap(char** str);

#endif