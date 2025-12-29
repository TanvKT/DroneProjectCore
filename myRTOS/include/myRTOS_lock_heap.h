/**
 * @file myRTOS_lock_heap.h
 * @author your name (you@domain.com)
 * @brief Header for dynamic memory needed for locking implementation
 * @version 0.1
 * @date 2025-12-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef __MYRTOS_LOCK_HEAP_H__
#define __MYRTOS_LOCK_HEAP_H__

 #include "myRTOS_types.h"

 myRTOS_return_type_e myrtos_zeroize_lock_heap();
 myRTOS_return_type_e myrtos_lock_heap_init();
 void* myrtos_lock_alloc(size_t s);
 void* myrtos_lock_realloc(void* p, size_t s);
 myRTOS_return_type_e myrtos_lock_free(void* p);

#endif