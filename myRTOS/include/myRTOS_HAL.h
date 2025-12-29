/**
 * @file myRTOS_HAL.h
 * @author your name (you@domain.com)
 * @brief   Provides an abstraction layer to a chip specific HAL
 * 
 *              Currently only supporting the PSOC6 microcontroller, however
 *                  the use of this file in conjunction with myRTOS_HAL.c
 *                  allows for implementation specific code
 * @version 0.1
 * @date 2025-09-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __MYRTOS_HAL_H__
#define __MYRTOS_HAL_H__
#include "myRTOS_config.h"
#include "myRTOS_types.h"
#include <stdint.h>

/* All these functions must be implemented using desired HAL in myRTOS_HAL.c */

/**
 * @brief These funtions are architecture specific implementations for myRTOS
 * 
 *          These are needed to set task scheduler interrupt priorities
 *          This is also needed to set stack pointers and save registers properly
 * 
 * @return int 0 if fail 1 if success
 */
int         myrtos_hal_schedule_init(myRTOS_int_task_type_vp t);    //Initialize scheduler (ISR flags, Stacks, etc..)
int         myrtos_hal_stack_setup(myRTOS_int_task_type_vp t);      //Initialize stack data per task
void        myrtos_hal_set_hardware_timer_flag(void);               //Set Hardware Timer Flag
void        myrtos_hal_enable_interrupts(void);                     //enable global interrupts
void        myrtos_hal_disable_interrupts(void);                    //disable global interrupts
uint8_t     myrtos_hal_get_lowest_priority(void);                   //get lowest hardware priority
uint8_t     myrtos_hal_atomic_read_write(volatile uint8_t* v);      //atomicly read and write to a uint8_t, used for lock correctness

/**
 * @brief Initialize UART on defined RX and TX pins using defined baud rate
 * 
 *          NOTE: Need to implement PIN defs for following:
 *                - MYRTOS_UART_TX
 *                - MYRTOS_UART_RX
 * 
 * @return int 0 if fail 1 if success
 */
int myrtos_hal_uart_init();
int myrtos_hal_uart_set_baud();
int myrtos_hal_uart_getc(uint8_t* c);
int myrtos_hal_uart_putc(uint32_t c);

/**
 * @brief Delays processor for a set peroid of milliseconds
 * 
 * @return int 0 if fail 1 if success
 */
int myrtos_hal_delay_ms(uint32_t ms);

/**
 * @brief Initialize a hardware timer on an internal clock
 * 
 *          Use defined period and frequency values
 * 
 * 
 * 
 * @return int 0 if fail 1 if success
 */
typedef void (*myRTOS_callback_t)(void);
int myrtos_hal_timer_init();
int myrtos_hal_timer_configure();
int myrtos_hal_register_callback(myRTOS_callback_t f);
int myrtos_hal_timer_start();
int myrtos_hal_timer_reset();
int myrtos_hal_timer_stop();

#ifdef MYRTOS_USE_CYHAL
/* Device Specific Includes */
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cyhal_uart.h"

//device specific defs
#define MYRTOS_UART_TX             CYBSP_DEBUG_UART_TX
#define MYRTOS_UART_RX             CYBSP_DEBUG_UART_RX

#else

#endif
#endif