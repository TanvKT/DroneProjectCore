/**
 * @file test_queue.c
 * @author your name (you@domain.com)
 * @brief 
 * 
 *      Tests queue logic for blocking behavior and priority ordering
 * 
 * @version 0.1
 * @date 2026-01-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "tests.h"
#include "unity.h"
#include "myRTOS.h"
#include "myRTOS_types.h"
#include "myRTOS_task_queue.h"
#include "myRTOS_sched.h"

void queue_test()
{
    #if MYRTOS_PRIORITY_LEVELS != 7 || (MYRTOS_DYNAMIC_PRIORITY && MYRTOS_PRIO_LOWER_THRESH != 3)
    printf("MYRTOS_PRIORITY_LEVELS != 7 or MYRTOS_DYNAMIC_PRIORITY enabled and MYRTOS_PRIO_LOWER_THRESH not 3\r\n");
    printf("Queue Test is deterministic so it needs these values to be set accordingly, skipping\r\n");
    #else

    myRTOS_queue_handle_s queue1;
    myRTOS_queue_handle_s queue2;
    myRTOS_return_type_e ret;

    // Test default init (queue length 10, data size 4, waiting lists size 2)
    ret = myrtos_queue_init(&queue1, 10, sizeof(uint32_t));
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Queue init with default size failed");
    TEST_ASSERT_EQUAL_MESSAGE(10, queue1.len, "Queue length should be 10");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(uint32_t), queue1.inc, "Queue data size should be sizeof(uint32_t)");
    TEST_ASSERT_EQUAL_MESSAGE(0, queue1.cnt, "Queue count should be 0");
    TEST_ASSERT_EQUAL_MESSAGE(0, queue1.st, "Queue start index should be 0");
    TEST_ASSERT_EQUAL_MESSAGE(0, queue1.en, "Queue end index should be 0");

    // Test init with custom sizes
    ret = myrtos_queue_init_num(&queue2, 5, sizeof(uint16_t), 3, 4);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Queue init with custom size failed");
    TEST_ASSERT_EQUAL_MESSAGE(5, queue2.len, "Queue length should be 5");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(uint16_t), queue2.inc, "Queue data size should be sizeof(uint16_t)");
    TEST_ASSERT_EQUAL_MESSAGE(3, queue2.t_c_n, "Consumer waiting list size should be 3");
    TEST_ASSERT_EQUAL_MESSAGE(4, queue2.t_p_n, "Producer waiting list size should be 4");

    // Test NULL pointer checks
    ret = myrtos_queue_init(NULL, 10, sizeof(uint32_t));
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Queue init with NULL handle should fail");

    ret = myrtos_queue_init_num(NULL, 10, sizeof(uint32_t), 2, 2);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Queue init_num with NULL handle should fail");

    ret = myrtos_queue_send(NULL, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Queue send with NULL handle should fail");

    ret = myrtos_queue_recieve(NULL, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Queue recieve with NULL handle should fail");

    // Test send/receive on NULL current task
    ret = myrtos_queue_send(&queue1, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Queue send on NULL task should fail");

    ret = myrtos_queue_recieve(&queue1, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Queue recieve on NULL task should fail");

    // Register tasks with different priorities
    myrtos_register_task("t1", 0, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t2", 4, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t3", 0, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t4", 4, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t5", 2, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t6", 2, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t7", 1, NULL, NULL, MYRTOS_MIN_STACK_SIZE);

    myRTOS_queue_arr_s* tasks = myrtos_get_task_queue();
    #ifdef MYRTOS_ROUND_ROBIN
    myRTOS_int_task_type_vp t1 = tasks->level[0].arr[0];
    myRTOS_int_task_type_vp t2 = tasks->level[0].arr[1];
    myRTOS_int_task_type_vp t3 = tasks->level[0].arr[2];
    myRTOS_int_task_type_vp t4 = tasks->level[0].arr[3];
    myRTOS_int_task_type_vp t5 = tasks->level[0].arr[4];
    myRTOS_int_task_type_vp t6 = tasks->level[0].arr[5];
    myRTOS_int_task_type_vp t7 = tasks->level[0].arr[6];
    #else
    myRTOS_int_task_type_vp t1 = tasks->level[0].arr[0];
    myRTOS_int_task_type_vp t2 = tasks->level[4].arr[0];
    myRTOS_int_task_type_vp t3 = tasks->level[0].arr[1];
    myRTOS_int_task_type_vp t4 = tasks->level[4].arr[1];
    myRTOS_int_task_type_vp t5 = tasks->level[2].arr[0];
    myRTOS_int_task_type_vp t6 = tasks->level[2].arr[1];
    myRTOS_int_task_type_vp t7 = tasks->level[1].arr[0];
    #endif

    // Test basic send and receive
    myRTOS_queue_handle_s test_queue;
    ret = myrtos_queue_init(&test_queue, 3, sizeof(uint32_t));
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Queue init failed");

    s_curr_task_p = t1;
    uint32_t data1 = 100;
    ret = myrtos_queue_send(&test_queue, &data1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Queue send failed");
    TEST_ASSERT_EQUAL_MESSAGE(1, test_queue.cnt, "Queue count should be 1 after send");

    uint32_t recv_data = 0;
    ret = myrtos_queue_recieve(&test_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Queue recieve failed");
    TEST_ASSERT_EQUAL_MESSAGE(100, recv_data, "Received data should be 100");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_queue.cnt, "Queue count should be 0 after receive");

    // Test multiple sends and receives (FIFO ordering)
    uint32_t data2 = 200;
    uint32_t data3 = 300;

    s_curr_task_p = t1;
    ret = myrtos_queue_send(&test_queue, &data1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "First send failed");

    s_curr_task_p = t2;
    ret = myrtos_queue_send(&test_queue, &data2);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Second send failed");

    s_curr_task_p = t3;
    ret = myrtos_queue_send(&test_queue, &data3);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Third send failed");
    TEST_ASSERT_EQUAL_MESSAGE(3, test_queue.cnt, "Queue count should be 3");

    s_curr_task_p = t4;
    ret = myrtos_queue_recieve(&test_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "First receive failed");
    TEST_ASSERT_EQUAL_MESSAGE(100, recv_data, "First received should be 100 (FIFO)");

    ret = myrtos_queue_recieve(&test_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Second receive failed");
    TEST_ASSERT_EQUAL_MESSAGE(200, recv_data, "Second received should be 200 (FIFO)");

    ret = myrtos_queue_recieve(&test_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Third receive failed");
    TEST_ASSERT_EQUAL_MESSAGE(300, recv_data, "Third received should be 300 (FIFO)");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_queue.cnt, "Queue count should be 0 after all receives");

    // Test circular buffer wrapping
    myRTOS_queue_handle_s circ_queue;
    ret = myrtos_queue_init(&circ_queue, 4, sizeof(uint32_t));
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Circular queue init failed");

    s_curr_task_p = t1;
    for (uint32_t i = 0; i < 4; i++)
    {
        ret = myrtos_queue_send(&circ_queue, &i);
        TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Send in circular queue failed");
    }
    TEST_ASSERT_EQUAL_MESSAGE(4, circ_queue.cnt, "Circular queue count should be 4");

    // Remove first two items to create gap at start
    s_curr_task_p = t2;
    for (uint32_t i = 0; i < 2; i++)
    {
        ret = myrtos_queue_recieve(&circ_queue, &recv_data);
        TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Receive in circular queue failed");
    }
    TEST_ASSERT_EQUAL_MESSAGE(2, circ_queue.cnt, "Circular queue count should be 2");

    // Send more items to wrap around
    s_curr_task_p = t3;
    uint32_t val5 = 100;
    uint32_t val6 = 101;
    ret = myrtos_queue_send(&circ_queue, &val5);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Send wrap 1 failed");
    ret = myrtos_queue_send(&circ_queue, &val6);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Send wrap 2 failed");
    TEST_ASSERT_EQUAL_MESSAGE(4, circ_queue.cnt, "Circular queue should be full again");

    // Receive to verify order
    s_curr_task_p = t4;
    ret = myrtos_queue_recieve(&circ_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(2, recv_data, "Should receive 2 (FIFO wrapping)");

    ret = myrtos_queue_recieve(&circ_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(3, recv_data, "Should receive 3 (FIFO wrapping)");

    ret = myrtos_queue_recieve(&circ_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(100, recv_data, "Should receive 100 (wrapped around)");

    ret = myrtos_queue_recieve(&circ_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(101, recv_data, "Should receive 101 (wrapped around)");
    TEST_ASSERT_EQUAL_MESSAGE(0, circ_queue.cnt, "Circular queue should be empty");

    // Test producer blocking when queue is full
    myRTOS_queue_handle_s block_queue;
    ret = myrtos_queue_init(&block_queue, 2, sizeof(uint32_t));
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Block queue init failed");

    s_curr_task_p = t1;
    uint32_t val1 = 111;
    uint32_t val2 = 222;
    ret = myrtos_queue_send(&block_queue, &val1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Send 1 to full queue failed");

    ret = myrtos_queue_send(&block_queue, &val2);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Send 2 to full queue failed");
    TEST_ASSERT_EQUAL_MESSAGE(2, block_queue.cnt, "Queue should be full");

    // This should block t2 (priority 4)
    s_curr_task_p = t2;
    ret = myrtos_queue_send(&block_queue, &val1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Send to full queue should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t2->b, "t2 should be blocked when queue full");
    TEST_ASSERT_EQUAL_MESSAGE(1, block_queue.t_p_i, "Producer waiting list should have 1 task");
    TEST_ASSERT_EQUAL_MESSAGE(t2, block_queue.t_p[0], "t2 should be in producer waiting list");

    // When consumer receives, highest priority producer should be unblocked
    s_curr_task_p = t1;
    ret = myrtos_queue_recieve(&block_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Receive from queue failed");
    TEST_ASSERT_EQUAL_MESSAGE(false, t2->b, "t2 should be unblocked after receive");
    TEST_ASSERT_EQUAL_MESSAGE(0, block_queue.t_p_i, "Producer waiting list should be empty");

    // Test consumer blocking when queue is empty
    myRTOS_queue_handle_s empty_queue;
    ret = myrtos_queue_init(&empty_queue, 2, sizeof(uint32_t));
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Empty queue init failed");

    // This should block t3 (priority 0)
    s_curr_task_p = t3;
    ret = myrtos_queue_recieve(&empty_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Receive from empty queue should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t3->b, "t3 should be blocked when queue empty");
    TEST_ASSERT_EQUAL_MESSAGE(1, empty_queue.t_c_i, "Consumer waiting list should have 1 task");
    TEST_ASSERT_EQUAL_MESSAGE(t3, empty_queue.t_c[0], "t3 should be in consumer waiting list");

    // When producer sends, highest priority consumer should be unblocked
    s_curr_task_p = t1;
    uint32_t producer_data = 555;
    ret = myrtos_queue_send(&empty_queue, &producer_data);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Send to empty queue failed");
    TEST_ASSERT_EQUAL_MESSAGE(false, t3->b, "t3 should be unblocked after send");
    TEST_ASSERT_EQUAL_MESSAGE(0, empty_queue.t_c_i, "Consumer waiting list should be empty");

    // Test priority ordering with multiple waiting consumers
    myRTOS_queue_handle_s prio_queue;
    ret = myrtos_queue_init_num(&prio_queue, 1, sizeof(uint32_t), 5, 5);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Priority queue init failed");

    // t1 (priority 0) tries to receive from empty queue
    s_curr_task_p = t1;
    ret = myrtos_queue_recieve(&prio_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "t1 receive should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t1->b, "t1 should be blocked");

    // t5 (priority 2) tries to receive
    s_curr_task_p = t5;
    ret = myrtos_queue_recieve(&prio_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "t5 receive should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t5->b, "t5 should be blocked");

    // t2 (priority 4) tries to receive
    s_curr_task_p = t2;
    ret = myrtos_queue_recieve(&prio_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "t2 receive should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t2->b, "t2 should be blocked");

    // t7 (priority 1) tries to receive
    s_curr_task_p = t7;
    ret = myrtos_queue_recieve(&prio_queue, &recv_data);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "t7 receive should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t7->b, "t7 should be blocked");

    TEST_ASSERT_EQUAL_MESSAGE(4, prio_queue.t_c_i, "Consumer waiting list should have 4 tasks");

    // Producer sends - highest priority (t1, priority 0) should be unblocked first
    s_curr_task_p = t3;
    uint32_t prio_val = 777;
    ret = myrtos_queue_send(&prio_queue, &prio_val);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Send to priority queue failed");
    TEST_ASSERT_EQUAL_MESSAGE(false, t1->b, "t1 (highest priority) should be unblocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t5->b, "t5 should still be blocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t2->b, "t2 should still be blocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t7->b, "t7 should still be blocked");
    TEST_ASSERT_EQUAL_MESSAGE(3, prio_queue.t_c_i, "Consumer waiting list should have 3 tasks");

    // Test priority ordering with multiple waiting producers
    myRTOS_queue_handle_s prod_prio_queue;
    ret = myrtos_queue_init_num(&prod_prio_queue, 1, sizeof(uint32_t), 5, 5);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Producer priority queue init failed");

    // Fill queue completely
    s_curr_task_p = t4;
    uint32_t fill_val = 999;
    ret = myrtos_queue_send(&prod_prio_queue, &fill_val);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Initial send failed");
    TEST_ASSERT_EQUAL_MESSAGE(1, prod_prio_queue.cnt, "Queue should have 1 item");

    // t5 (priority 2) tries to send
    s_curr_task_p = t5;
    ret = myrtos_queue_send(&prod_prio_queue, &fill_val);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "t5 send should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t5->b, "t5 should be blocked");

    // t1 (priority 0) tries to send
    s_curr_task_p = t1;
    ret = myrtos_queue_send(&prod_prio_queue, &fill_val);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "t1 send should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t1->b, "t1 should be blocked");

    // t2 (priority 4) tries to send
    s_curr_task_p = t2;
    ret = myrtos_queue_send(&prod_prio_queue, &fill_val);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "t2 send should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t2->b, "t2 should be blocked");

    TEST_ASSERT_EQUAL_MESSAGE(3, prod_prio_queue.t_p_i, "Producer waiting list should have 3 tasks");

    // Consumer receives - highest priority producer (t1, priority 0) should be unblocked
    s_curr_task_p = t3;
    ret = myrtos_queue_recieve(&prod_prio_queue, &recv_data);
    #ifndef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Receive from producer priority queue failed");
    TEST_ASSERT_EQUAL_MESSAGE(false, t1->b, "t1 (highest priority) should be unblocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t5->b, "t5 should still be blocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t2->b, "t2 should still be blocked");
    TEST_ASSERT_EQUAL_MESSAGE(2, prod_prio_queue.t_p_i, "Producer waiting list should have 2 tasks");
    TEST_ASSERT_EQUAL_MESSAGE(t5, prod_prio_queue.t_p[0], "t5 should be at index 0 of producer waiting list");
    TEST_ASSERT_EQUAL_MESSAGE(t2, prod_prio_queue.t_p[1], "t2 should be at index 1 of producer waiting list");
    #else
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Receive from producer priority queue failed");
    TEST_ASSERT_EQUAL_MESSAGE(false, t5->b, "t5 should be unblocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t1->b, "t1 should still be blocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t2->b, "t2 should still be blocked");
    TEST_ASSERT_EQUAL_MESSAGE(2, prod_prio_queue.t_p_i, "Producer waiting list should have 2 tasks");
    TEST_ASSERT_EQUAL_MESSAGE(t1, prod_prio_queue.t_p[0], "t1 should be at index 0 of producer waiting list");
    TEST_ASSERT_EQUAL_MESSAGE(t2, prod_prio_queue.t_p[1], "t2 should be at index 1 of producer waiting list");
    #endif

    // Test queue resize
    myRTOS_queue_handle_s resize_queue;
    ret = myrtos_queue_init(&resize_queue, 2, sizeof(uint32_t));
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Resize queue init failed");

    s_curr_task_p = t1;
    uint32_t resize_val = 42;
    ret = myrtos_queue_send(&resize_queue, &resize_val);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Send to resize queue failed");
    TEST_ASSERT_EQUAL_MESSAGE(1, resize_queue.cnt, "Queue count should be 1");

    // Resize the queue
    ret = myrtos_queue_resize(&resize_queue, 5, sizeof(uint32_t));
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Queue resize failed");
    TEST_ASSERT_EQUAL_MESSAGE(5, resize_queue.len, "Queue length should be 5 after resize");

    // Send more items after resize
    for (uint32_t i = 0; i < 4; i++)
    {
        ret = myrtos_queue_send(&resize_queue, &i);
        TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Send after resize failed");
    }
    TEST_ASSERT_EQUAL_MESSAGE(5, resize_queue.cnt, "Queue should be full after sends");

    // Test resize with NULL handle
    ret = myrtos_queue_resize(NULL, 10, sizeof(uint32_t));
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Resize with NULL handle should fail");

    #endif
}