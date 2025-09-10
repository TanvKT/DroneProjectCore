/**
 * @file myRTOS_HAL.c
 * @author your name (you@domain.com)
 * @brief   Provides an abstraction layer to allow myRTOS to run on multiple different chipsets
 * 
 *              Currently only the PSOC6 is implemented, however the goal of this file
 *                  is to make the implementation of other chipsets easy
 * @version 0.1
 * @date 2025-09-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "myRTOS_config.h"
#include "myRTOS_HAL.h"

static myRTOS_callback_t s_cb;

//PSOC6 specific implentation
#ifdef MYRTOS_USE_CYHAL

/* Global Objects */
static cyhal_uart_t myRTOS_uart;
static cyhal_timer_t myRTOS_task_timer;

int myrtos_hal_uart_init()
{
    cy_rslt_t cy_ret;

    cy_ret = cyhal_uart_init(&myRTOS_uart, MYRTOS_UART_TX, MYRTOS_UART_RX, NC, NC, NULL, NULL);
    if (cy_ret != CY_RSLT_SUCCESS) return -1;
    return 0;
}
int myrtos_hal_uart_set_baud()
{
    cy_rslt_t cy_ret;

    cy_ret = cyhal_uart_set_baud(&myRTOS_uart, MYRTOS_UART_BAUD, NULL);
    if (cy_ret != CY_RSLT_SUCCESS) return -1;
    return 0;
}
int myrtos_hal_uart_getc(uint8_t* c)
{
    cy_rslt_t cy_ret;

    cy_ret = cyhal_uart_getc(&myRTOS_uart, c, 0);
    if (cy_ret != CY_RSLT_SUCCESS) return -1;
    return 0;
}
int myrtos_hal_uart_putc(uint32_t c)
{
    cy_rslt_t cy_ret;

    cy_ret = cyhal_uart_putc(&myRTOS_uart, c);
    if (cy_ret != CY_RSLT_SUCCESS) return -1;
    return 0;
}

int myrtos_hal_delay_ms(uint32_t ms)
{
    Cy_SysLib_Delay(ms);
    return 0;
}

static void timer_callback_adapter(void* arg, cyhal_timer_event_t event)
{
    if (s_cb != NULL)
        s_cb();
}

int myrtos_hal_timer_init()
{
    cy_rslt_t cy_ret;

    cy_ret = cyhal_timer_init(&myRTOS_task_timer, NC, NULL);
    if (cy_ret != CY_RSLT_SUCCESS) return -1;
    return 0;
}
int myrtos_hal_timer_configure()
{
    cy_rslt_t cy_ret;

    const cyhal_timer_cfg_t c = {
        .compare_value = 0,                 /* Timer compare value, not used */
        .period = MYRTOS_TASK_TIMER_PERIOD, /* Timer Period (number to count to before rollover) */
        .direction = CYHAL_TIMER_DIR_UP,    /* Timer counts up */
        .is_compare = false,                /* Don't use compare mode */
        .is_continuous = true,              /* Run timer indefinitely */
        .value = 0                          /* Start value */
    };

    cy_ret = cyhal_timer_configure(&myRTOS_task_timer, &c);
    if (cy_ret != CY_RSLT_SUCCESS) return -1;

    cy_ret = cyhal_timer_set_frequency(&myRTOS_task_timer, MYRTOS_TASK_TIMER_FREQ);
    if (cy_ret != CY_RSLT_SUCCESS) return -1;

    return 0;
}
int myrtos_hal_register_callback(myRTOS_callback_t f)
{
    s_cb = f;
    cyhal_timer_register_callback(&myRTOS_task_timer, timer_callback_adapter, NULL);
    //enable ISR event with priority 0 (ISR will be handled with highest priority compared to other GPIO interrupts)
    cyhal_timer_enable_event(&myRTOS_task_timer, CYHAL_TIMER_IRQ_TERMINAL_COUNT, 0, true);
    return 0;
}
int myrtos_hal_timer_start()
{
    cy_rslt_t cy_ret;

    cy_ret = cyhal_timer_start(&myRTOS_task_timer);
    if (cy_ret != CY_RSLT_SUCCESS) return -1;
    return 0;
}
int myrtos_hal_timer_reset()
{
    cy_rslt_t cy_ret;

    cy_ret = cyhal_timer_reset(&myRTOS_task_timer);
    if (cy_ret != CY_RSLT_SUCCESS) return -1;
    return 0;
}
int myrtos_hal_timer_stop()
{
    cy_rslt_t cy_ret;

    cy_ret = cyhal_timer_stop(&myRTOS_task_timer);
    if (cy_ret != CY_RSLT_SUCCESS) return -1;
    return 0;
}

#else   //add code here for device specific configuration
        //ensure all functions in myRTOS_HAL.h are implemented
        //running provided tests provide assistance in correct implementation
#endif