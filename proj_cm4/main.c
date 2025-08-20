/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for CM4 in the the Dual CPU Empty 
*              Application for ModusToolbox.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2020-2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#include "main.h"
#include "test_heap.h"

int main(void)
{
    cy_rslt_t result;
    myRTOS_return_type_e my_ret;

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    char* str = malloc(1024);
    str[0] = '\0';

    /* Initialize hardware */
    //initialize usb serial uart first for console output
    result = usb_uart_init();
    if (result != CY_RSLT_SUCCESS)
    {
        usb_uart_printf("UART INIT FAILED!!\r\n");
        for(;;){} //hang here
    }

    char* str1 = malloc(1024);
    str1[0] = '\0';

    /* Initialize myRTOS for testing */
    my_ret = myrtos_init();
    if (my_ret != MYRTOS_SUCCESS)
    {
        usb_uart_printf("MYRTOS INIT FAILED!!\r\n");
        for (;;){} //hang here
    }

    char* str2 = malloc(1024);
    str2[0] = '\0';

    CY_ASSERT(str[0] == str1[0] == str2[0]);

    heap_test();

    for (;;){}
}

/* [] END OF FILE */
