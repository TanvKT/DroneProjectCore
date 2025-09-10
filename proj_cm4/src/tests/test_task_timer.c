/**
 * @file test_task_timer.c
 * @author your name (you@domain.com)
 * @brief Tests timer period and frequency
 * @version 0.1
 * @date 2025-09-04
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "tests.h"
#include "unity.h"
#include "myRTOS_HAL.h"

#define TEST_TIMER_PERIOD_MS (uint32_t)(((MYRTOS_TASK_TIMER_PERIOD + 1) * 1000.0) / MYRTOS_TASK_TIMER_FREQ)

volatile bool timer_trip = false;

static void test_isr(void)
{
    timer_trip = true;
}

void task_timer_test()
{
    printf("Timer Period(ms) - %ld\r\n", TEST_TIMER_PERIOD_MS);

    TEST_ASSERT_EQUAL_INT(0, myrtos_hal_timer_init());
    TEST_ASSERT_EQUAL_INT(0, myrtos_hal_timer_configure());

    //ensure that timer_isr is not called while timer not started
    TEST_ASSERT_EQUAL_INT(0, myrtos_hal_register_callback(test_isr));
    TEST_ASSERT_EQUAL_INT(0, myrtos_hal_delay_ms(TEST_TIMER_PERIOD_MS));
    TEST_ASSERT_EQUAL(false, timer_trip);
    TEST_ASSERT_EQUAL_INT(0, myrtos_hal_delay_ms(TEST_TIMER_PERIOD_MS*20));
    TEST_ASSERT_EQUAL(false, timer_trip);

    //start timer and test timer for 1000 interrupts    
    TEST_ASSERT_EQUAL_INT(0, myrtos_hal_timer_start());
    for (int i = 0; i < 1000; i++)
    {
        TEST_ASSERT_EQUAL_INT(0, myrtos_hal_delay_ms(TEST_TIMER_PERIOD_MS));
        TEST_ASSERT_EQUAL(true, timer_trip);
        timer_trip = false;
    }

    //test timer stop
    TEST_ASSERT_EQUAL_INT(0, myrtos_hal_timer_stop());
    timer_trip = false;
    TEST_ASSERT_EQUAL_INT(0, myrtos_hal_delay_ms(TEST_TIMER_PERIOD_MS));
    TEST_ASSERT_EQUAL(false, timer_trip);
    TEST_ASSERT_EQUAL_INT(0, myrtos_hal_delay_ms(TEST_TIMER_PERIOD_MS*20));
    TEST_ASSERT_EQUAL(false, timer_trip);

    //test timer reset
    //running through different delay periods that are less than overall time silce
    //the interrupt should never trigger
    TEST_ASSERT_EQUAL_INT(0, myrtos_hal_timer_start());
    for (int i = 200; i < 1000; i++)
    {
        TEST_ASSERT_EQUAL_INT(0, myrtos_hal_timer_reset());
        uint32_t ms = (uint32_t)((double)TEST_TIMER_PERIOD_MS * (((double)i - 100.0) / 1000.0));
        TEST_ASSERT_EQUAL_INT(0, myrtos_hal_delay_ms(ms));
        TEST_ASSERT_EQUAL(false, timer_trip);
    }
}