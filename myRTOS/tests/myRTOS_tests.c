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
#include "malloc.h"

void heap_test()
{
    printf("--STARTING MYRTOS HEAP TEST--\r\n");
    char* str = malloc(sizeof(char) * 2048);
    if (!str)
    {
        printf("--HEAP TEST: FAILED STRING MALLOC--");
        return;
    }
    str[0] = '\0';
    myrtos_init();
    /* Testing code for heap */

    /**
     * @brief TEST 1 : allocate smallest block
     * 
     */
    printf("\rTEST 1\n");
    void* p0 = myrtos_alloc(8);
    myrtos_print_heap(&str);
    printf(str);
    str[0] = '\0';
    printf("\rEnter anything to contine...\n");
    getchar();

    /**
     * @brief TEST 2 : allocate larger block
     * 
     */
    printf("\rTEST 2\n");
    void* p1 = myrtos_alloc(16);
    myrtos_print_heap(&str);
    printf(str);
    str[0] = '\0';
    printf("\rEnter anything to contine...\n");
    getchar();

    /**
     * @brief TEST 3 : allocate larger, unaligned block
     * 
     */
    printf("\rTEST 3\n");
    void* p2 = myrtos_alloc(18);
    myrtos_print_heap(&str);
    printf(str);
    str[0] = '\0';
    printf("\rEnter anything to contine...\n");
    getchar();

    /**
     * @brief TEST 4 : allocate two larger, unaligned blocks
     * 
     */
    printf("\rTEST 4\n");
    void* p3 = myrtos_alloc(30);
    void* p4 = myrtos_alloc(6);
    myrtos_print_heap(&str);
    printf(str);
    str[0] = '\0';
    printf("\rEnter anything to contine...\n");
    getchar();

    free(str);
}