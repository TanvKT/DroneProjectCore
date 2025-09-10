/**
 * @file UART.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __MYRTOS_UART_H__
#define __MYRTOS_UART_H__

//inlcudes
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include "myRTOS.h"

/**
 * @brief Functions
 * 
 */
myRTOS_return_type_e    myRTOS_uart_init();
int                     myRTOS_uart_write(const char *ptr, size_t len);
int                     myRTOS_uart_read(char *ptr, size_t len);
int                     myRTOS_uart_printf(const char* f, ...);
ssize_t                 myRTOS_uart_getline(char* sp, size_t len);
int                     myRTOS_uart_getchar();

#endif
