/**
 * @file myRTOS_stdio.c
 * @author your name (you@domain.com)
 * @brief Overwrite stdio functions
 * @version 0.1
 * @date 2025-08-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <sys/stat.h>
#include <errno.h>
#include "myRTOS_UART.h"

int _write(int fd, const void *buf, size_t count)
{
    return myRTOS_uart_write(buf, count);
}

int _read(int fd, void *buf, size_t count)
{
    return myRTOS_uart_read(buf, count);
}

int _isatty(int fd) { return 1; }
int _fstat(int fd, struct stat *st) { st->st_mode = S_IFCHR; return 0; }
int _close(int fd) { return -1; }
int _lseek(int fd, int ptr, int dir) { return 0; }