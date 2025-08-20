/**
 * @file test_heap.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "myRTOS.h"
#include "test_heap.h"
#include <stdlib.h>

void heap_test()
{
    // char* str;

    // printf("--STARTING MYRTOS HEAP TEST--\r\n");

    // str = malloc(sizeof(char) * 2048);
    // if (!str)
    // {
    //     printf("STR MALLOC FAILED\r\n");
    //     return;
    // }
    // str[0] = '\0';
    // /* Testing code for heap */

    // /**
    //  * @brief TEST 1 : allocate smallest block
    //  * 
    //  */
    // printf("\rTEST 1\n");
    // void* p0 = myrtos_alloc(8);
    // myrtos_print_heap(&str);
    // printf(str);
    // str[0] = '\0';
    // printf("\rEnter anything to contine...\n");
    // getchar();

    // /**
    //  * @brief TEST 2 : allocate larger block
    //  * 
    //  */
    // printf("\rTEST 2\n");
    // void* p1 = myrtos_alloc(16);
    // myrtos_print_heap(&str);
    // printf(str);
    // str[0] = '\0';
    // printf("\rEnter anything to contine...\n");
    // getchar();

    // /**
    //  * @brief TEST 3 : allocate larger, unaligned block
    //  * 
    //  */
    // usb_uart_printf("\rTEST 3\n");
    // void* p2 = myrtos_alloc(18);
    // myrtos_print_heap(&str);
    // usb_uart_printf(str);
    // str[0] = '\0';
    // usb_uart_printf("\rEnter anything to contine...\n");
    // usb_uart_getchar();

    // /**
    //  * @brief TEST 4 : allocate two larger, unaligned blocks
    //  * 
    //  */
    // usb_uart_printf("\rTEST 4\n");
    // void* p3 = myrtos_alloc(30);
    // void* p4 = myrtos_alloc(6);
    // myrtos_print_heap(&str);
    // usb_uart_printf(str);
    // str[0] = '\0';
    // usb_uart_printf("\rEnter anything to contine...\n");
    // usb_uart_getchar();

    // free(str);
}