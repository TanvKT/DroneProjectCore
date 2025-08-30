/**
 * @file tests.c
 * @author your name (you@domain.com)
 * @brief Run all unit tests for myRTOS
 * @version 0.1
 * @date 2025-08-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "tests.h"
#include "unity.h"
#include "myRTOS.h"
#include <stdio.h>

void setUp(void) 
{
    myrtos_reset();
}
void tearDown(void) {}

int test_all()
{
    printf("Enter anything to start the tests...\r\n");
    getchar();

    UNITY_BEGIN();
    printf("Enter anything to start the init test...\r\n");
    getchar();
    RUN_TEST(init_test);

    printf("Enter anything to start the heap test...\r\n");
    getchar();
    RUN_TEST(heap_test);

    printf("Enter anything to start the task array test...\r\n");
    getchar();
    RUN_TEST(task_array_test);

    #ifdef MYRTOS_ROUND_ROBIN
    printf("\n\nMYRTOS_ROUND_ROBIN enabled, re-define this as MYRTOS_PRIORITY_BASED or MYRTOS_DYNAMIC_PRIORITY to test priority queue\r\n");
    #else
    printf("Enter anything to start the priority queue test...\r\n");
    getchar();
    RUN_TEST(priority_queue_test);
    #endif

    return UNITY_END();
}