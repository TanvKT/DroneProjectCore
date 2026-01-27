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

#include "myRTOS_config.h"
#include "myRTOS_types.h"

/**
 * @brief MyRTOS initialization and registration and debug functions
 * 
 */
myRTOS_return_type_e    myrtos_reset();
myRTOS_return_type_e    myrtos_init();
myRTOS_return_type_e    myrtos_register_task(const char* name, uint8_t priority, void* handle, void* args, size_t stack_size);
const char*             myrtos_debug_print(myRTOS_return_type_e r);
myRTOS_return_type_e    myrtos_set_fatal();

/**
 * @brief MyRTOS scheduling
 * 
 */
myRTOS_return_type_e    myrtos_schedule_start();
myRTOS_return_type_e    myrtos_schedule_disable();
myRTOS_return_type_e    myrtos_schedule_enable();
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
myRTOS_return_type_e    myrtos_mutex_init_num(myRTOS_mutex_handle_s* h, size_t n);
myRTOS_return_type_e    myrtos_mutex_take(myRTOS_mutex_handle_s* h);
myRTOS_return_type_e    myrtos_mutex_give(myRTOS_mutex_handle_s* h);
/**
 * SEMAPHORE
 */
myRTOS_return_type_e    myrtos_semaphore_init(myRTOS_semaphore_handle_s* h, int8_t n);
myRTOS_return_type_e    myrtos_semaphore_init_num(myRTOS_semaphore_handle_s* h, int8_t n, size_t w);
myRTOS_return_type_e    myrtos_semaphore_take(myRTOS_semaphore_handle_s* h);
myRTOS_return_type_e    myrtos_semaphore_give(myRTOS_semaphore_handle_s* h);
/**
 * QUEUE
 */
myRTOS_return_type_e    myrtos_queue_init(myRTOS_queue_handle_s* h, size_t l, size_t d);
myRTOS_return_type_e    myrtos_queue_init_num(myRTOS_queue_handle_s* h, size_t l, size_t d, size_t wc, size_t wp);
myRTOS_return_type_e    myrtos_queue_resize(myRTOS_queue_handle_s* h, size_t l, size_t d);
myRTOS_return_type_e    myrtos_queue_send(myRTOS_queue_handle_s* h, void* d);
myRTOS_return_type_e    myrtos_queue_recieve(myRTOS_queue_handle_s* h, void* d);

/**
 * @brief MyRTOS memory management
 * 
 */
myRTOS_return_type_e    myrtos_heap_init();
void*                   myrtos_alloc(size_t s);
void*                   myrtos_realloc(void* p, size_t s);
myRTOS_return_type_e    myrtos_free(void* p);
void                    myrtos_print_heap(char** str);

#endif