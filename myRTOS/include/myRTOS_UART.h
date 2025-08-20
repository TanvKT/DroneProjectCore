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
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cyhal_uart.h"

// /* Overwrite Defs */
// #define printf              myRTOS_uart_printf
// #define getline(p,n,f)      myRTOS_uart_getline(p,n)
// #define getchar             myRTOS_uart_getchar

//general defs
#define MYRTOS_UART_BAUD           115200
#define MYRTOS_UART_BUFF_SIZE      2048
#define MYRTOS_UART_TX             CYBSP_DEBUG_UART_TX
#define MYRTOS_UART_RX             CYBSP_DEBUG_UART_RX

//globals
extern cyhal_uart_t g_myRTOS_uart;

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
