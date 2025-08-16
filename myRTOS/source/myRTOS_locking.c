/**
 * @file myRTOS_locking.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "myRTOS.h"

/**
 * MUTEX
 */
myRTOS_return_type_e    myrtos_mutex_init(myRTOS_mutex_handle_s* h){return MYRTOS_SUCCESS;}
myRTOS_return_type_e    myrtos_mutex_take(myRTOS_mutex_handle_s* h){return MYRTOS_SUCCESS;}
myRTOS_return_type_e    myrtos_mutex_give(myRTOS_mutex_handle_s* h){return MYRTOS_SUCCESS;}
/**
 * SEMAPHORE
 */
myRTOS_return_type_e    myrtos_semaphore_init(myRTOS_semaphore_handle_s* h){return MYRTOS_SUCCESS;}
myRTOS_return_type_e    myrtos_semaphore_take(myRTOS_semaphore_handle_s* h){return MYRTOS_SUCCESS;}
myRTOS_return_type_e    myrtos_semaphore_give(myRTOS_semaphore_handle_s* h){return MYRTOS_SUCCESS;}
/**
 * QUEUE
 */
myRTOS_return_type_e    myrtos_queue_set(myRTOS_queue_handle_s* h, int l, size_t s){return MYRTOS_SUCCESS;}
myRTOS_return_type_e    myrtos_queue_init(myRTOS_queue_handle_s* h){return MYRTOS_SUCCESS;}
myRTOS_return_type_e    myrtos_queue_send(myRTOS_queue_handle_s* h, void* d){return MYRTOS_SUCCESS;}
myRTOS_return_type_e    myrtos_queue_recieve(myRTOS_queue_handle_s* h, void* d){return MYRTOS_SUCCESS;}