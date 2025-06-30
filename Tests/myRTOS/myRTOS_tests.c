/**
 * @file myRTOS_tests.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "myRTOS_tests.h"

void heap_test()
{
    printf("--STARTING MYRTOS HEAP TEST--\r\n");
    char str[1024];
    /* Testing code for heap */
    myrtos_init();
    void* p0 = myrtos_alloc(8);
    myrtos_print_heap(&str);
    printf(str);
    str[0] = '\0';

    void* p1 = myrtos_alloc(16);
    myrtos_print_heap(&str);
    printf(str);
    str[0] = '\0';

    void* p2 = myrtos_alloc(30);
    void* p3 = myrtos_alloc(6);
    myrtos_print_heap(&str);
    printf(str);
    str[0] = '\0';
}