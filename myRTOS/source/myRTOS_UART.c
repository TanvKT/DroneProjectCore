/**
 * @file UART.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "myRTOS_UART.h"

//globals
cyhal_uart_t g_myRTOS_uart;
static myRTOS_mutex_handle_s s_myRTOS_uart_lock;


//function declarations
/**
 * @brief initialize uart object on defined pins
 * 
 * @return cy_rslt_t
 */
myRTOS_return_type_e myRTOS_uart_init()
{
    cy_rslt_t cy_ret;
    myRTOS_return_type_e my_ret;

    cy_ret = cyhal_uart_init(&g_myRTOS_uart, MYRTOS_UART_TX, MYRTOS_UART_RX, NC, NC, NULL, NULL);
    if (cy_ret != CY_RSLT_SUCCESS)
        return MYRTOS_UART_INIT_FAIL;
    
    cy_ret = cyhal_uart_set_baud(&g_myRTOS_uart, MYRTOS_UART_BAUD, NULL);
    if (cy_ret != CY_RSLT_SUCCESS)
        return MYRTOS_UART_BAUD_FAIL;

    my_ret = myrtos_mutex_init(&s_myRTOS_uart_lock);
    if (my_ret != MYRTOS_SUCCESS)
        return my_ret;
    
    return MYRTOS_SUCCESS;
}

/**
 * @brief write to UART
 * 
 * @param ptr string to print
 * @param len length of string to read
 * @return int length of string printed
 */
int myRTOS_uart_write(const char *ptr, size_t len)
{
    //grab lock to become sole controller of uart
    myrtos_mutex_take(&s_myRTOS_uart_lock);

    for (int i = 0; i < len; i++)
    {
        //check if we are at end of string
        if (ptr[i] == '\0')
        {
            //give up lock so that other tasks can print
            myrtos_mutex_give(&s_myRTOS_uart_lock);
            return i;
        }

        // Write string to UART
        cyhal_uart_putc(&g_myRTOS_uart, ptr[i]);
    }

    //give up lock so that other tasks can print
    myrtos_mutex_give(&s_myRTOS_uart_lock);
    return len;
}

/**
 * @brief read a string from UART
 * 
 * @param ptr pointer string to print to
 * @param len length of string to read
 * @return int length actually read
 */
int myRTOS_uart_read(char *ptr, size_t len)
{
    //grab lock to become sole controller of uart
    myrtos_mutex_take(&s_myRTOS_uart_lock);

    for (int i = 0; i < len; ++i)
    {
        uint8_t ch;
        // Blocking receive
        cyhal_uart_getc(&g_myRTOS_uart, &ch, 0);
        ptr[i] = ch;

        // Echo back the input
        cyhal_uart_putc(&g_myRTOS_uart, ch);

        if (ch == '\r' || ch == '\n')  // End on newline
        {
            ptr[i + 1] = '\0';
            //give up lock so that other tasks can print
            myrtos_mutex_give(&s_myRTOS_uart_lock);
            return i + 1;
        }
    }

    //give up lock so that other tasks can print
    myrtos_mutex_give(&s_myRTOS_uart_lock);
    return len;
}

/**
 * @brief Construct a new usb uart printf object
 * 
 * @param f format string
 * @param ... format args
 */
int myRTOS_uart_printf(const char* f, ...)
{
    static char buf[MYRTOS_UART_BUFF_SIZE];  // You can increase this if needed
    va_list args;
    va_start(args, f);
    int len = vsnprintf(buf, sizeof(buf), f, args);
    va_end(args);

    // Send buffer over UART
    return myRTOS_uart_write(buf, len);
}

/**
 * @brief Get a line of chars from the UART
 * 
 * @param sp pointer to string
 *                  In this implementation we assume sp is a non-null string
 * @param len max length to read
 * @return ssize_t length of string read
 */
ssize_t myRTOS_uart_getline(char* sp, size_t len)
{
    //not implementing for non null string, so if given as arg return fail
    if (sp == NULL)
        return -1;

    return myRTOS_uart_read(sp, len);
}

/**
 * @brief Get a single char from UART
 * 
 * @return int char read in int form 
 */
int myRTOS_uart_getchar()
{
    uint8_t c;

    //grab lock to become sole controller of uart
    myrtos_mutex_take(&s_myRTOS_uart_lock);

    cyhal_uart_getc(&g_myRTOS_uart, &c, 0);

    //give up lock so that other tasks can print
    myrtos_mutex_give(&s_myRTOS_uart_lock);

    return (int)c;
}