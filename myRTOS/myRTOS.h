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
#include "types/myRTOS_types.h"

/**
 * @brief MyRTOS initialization and registration functions
 * 
 */
myRTOS_return_type_e myrtos_init();
myRTOS_return_type_e myrtos_init(myRTOS_config_type_s* c);
myRTOS_return_type_e myrtos_modify(myRTOS_config_type_s* c);
myRTOS_return_type_e myrtos_register_task(myRTOS_task_type_s* t);
myRTOS_return_type_e myrtos_modify_task(myRTOS_task_type_s* t);

/**
 * @brief MyRTOS scheduling
 * 
 */
myRTOS_return_type_e myrtos_schedule_start();
myRTOS_return_type_e myrtos_schedule_stop();

/**
 * @brief MYRTOS inter task communication
 * 
 */
/**
 * MUTEX
 */
myRTOS_return_type_e myrtos_mutex_init(myRTOS_mutex_handle_s* h);
myRTOS_return_type_e myrtos_mutex_take(myRTOS_mutex_handle_s* h);
myRTOS_return_type_e myrtos_mutex_give(myRTOS_mutex_handle_s* h);
/**
 * SEMAPHORE
 */
myRTOS_return_type_e myrtos_semaphore_init(myRTOS_semaphore_handle_s* h);
myRTOS_return_type_e myrtos_semaphore_take(myRTOS_semaphore_handle_s* h);
myRTOS_return_type_e myrtos_semaphore_give(myRTOS_semaphore_handle_s* h);
/**
 * QUEUE
 */
myRTOS_queue_handle_s myrtos_queue_set(int l, size_t s);
myRTOS_return_type_e myrtos_queue_init(myRTOS_queue_handle_s* h);
myRTOS_return_type_e myrtos_queue_send(myRTOS_queue_handle_s* h, void* d);
myRTOS_return_type_e myrtos_queue_recieve(myRTOS_queue_handle_s* h, void* d);