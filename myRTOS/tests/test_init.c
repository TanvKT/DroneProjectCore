/**
 * @file test_init.c
 * @author your name (you@domain.com)
 * @brief Initialization test
 * @version 0.1
 * @date 2025-08-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "tests.h"
#include "unity.h"
#include "myRTOS.h"
#include "myRTOS_memory.h"

void init_test()
{
    TEST_ASSERT_NOT_EQUAL_size_t_MESSAGE(NULL, myrtos_get_heap_bp(), "Pointer to MYRTOS_HEAP is NULL");
    TEST_ASSERT_NOT_EQUAL_size_t_MESSAGE(NULL, myrtos_get_lock_heap_bp(), "Pointer to MYRTOS_LOCK_HEAP is NULL");
    TEST_ASSERT_NOT_EQUAL_size_t_MESSAGE(NULL, myrtos_get_stack_bp(), "Pointer to MYRTOS_STACK is NULL");
    TEST_ASSERT_NOT_EQUAL_size_t_MESSAGE(NULL, myrtos_get_task_arr_bp(), "Pointer to MYRTOS_TASK_ARR is NULL");
    TEST_ASSERT_NOT_EQUAL_size_t_MESSAGE(NULL, myrtos_get_task_queue_bp(), "Pointer to MYRTOS_TASK_QUEUE is NULL");

    //quick checks for some debug message variables, assuming if some work, all work
    TEST_ASSERT_EQUAL_STRING_MESSAGE("MYRTOS_MEMORY_LIMIT_REACHED", myrtos_debug_print(MYRTOS_MEMORY_LIMIT_REACHED), 
            "Debug print string on MYRTOS_MEMORY_LIMIT_REACHED returned incorrect string");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("MYRTOS_SUCCESS", myrtos_debug_print(MYRTOS_SUCCESS), 
            "Debug print string on MYRTOS_SUCCESS returned incorrect string");
}
