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
#include <stdio.h>

void setUp(void) {}
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
    return UNITY_END();
}