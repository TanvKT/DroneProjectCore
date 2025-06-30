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

#ifndef __UART_H__
#define __UART_H__

//inlcudes
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include "cybsp.h"
#include "cyhal_uart.h"

/* Overwrite Defs */
#define printf              usb_uart_printf
#define getline(p,n,f)      usb_uart_getline(p,n)
#define getchar             usb_uart_getchar

//general defs
#define USB_UART_BAUD           115200
#define USB_UART_BUFF_SIZE      128

//globals
extern cyhal_uart_t g_usb_uart;

//function declarations
cy_rslt_t usb_uart_init();
int     usb_uart_printf(const char* f, ...);
ssize_t usb_uart_getline(char** sp, size_t len);
int     usb_uart_getchar();


#endif
