/**
 * @file test_priority_queue.c
 * @author your name (you@domain.com)
 * @brief Tests priority queue
 * @version 0.1
 * @date 2025-08-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "tests.h"
#include "unity.h"
#include "myRTOS.h"
#include "myRTOS_types.h"
#include "myRTOS_task_queue.h"
#include "myRTOS_memory.h"
#include "myRTOS_heap.h"
#include "myRTOS_sched.h"
#include <string.h>

static void test_task_equal(int i, myRTOS_task_type_s t, myRTOS_task_queue_s* tasks)
{
    TEST_ASSERT_EQUAL_STRING(t.name, tasks->arr[i].t.name);
    TEST_ASSERT_EQUAL_UINT8(t.priority, tasks->arr[i].t.priority);
    TEST_ASSERT_EQUAL_size_t(t.stack_size, tasks->arr[i].t.stack_size);
}

void priority_queue_test()
{
    myRTOS_task_queue_s* tasks;
    myRTOS_return_type_e ret;
    char str[64];
    myRTOS_task_type_s t1 = {.name = "t1", .priority=0, .stack_size=MYRTOS_MIN_STACK_SIZE};
    myRTOS_task_type_s t2 = {.name = "t2", .priority=100, .stack_size=MYRTOS_MIN_STACK_SIZE/2};
    myRTOS_task_type_s t3 = {.name = "t3", .priority=4, .stack_size=MYRTOS_MIN_STACK_SIZE*6};
    myRTOS_task_type_s t4 = {.name = "t4", .priority=4, .stack_size=MYRTOS_MIN_STACK_SIZE*6};

    /* Test Priority Based and Dynamic */
    /* Mostly just going to test priority queue functionality here as the functionality for testing stack size and everything is the same */
    tasks = myrtos_get_task_queue();

    //register a couple different tasks
    ret = myrtos_register_task_i(&t1);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    ret = myrtos_register_task_i(&t2);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    ret = myrtos_register_task_i(&t3);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    ret = myrtos_register_task_i(&t4);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);

    //need to swap t2 priority to correct value to check against
    //          at this point we have already tested to ensure that modifications to tasks
    //          locally do not change tasks in task array
    t2.priority = MYRTOS_PRIORITY_LEVELS-1;

    //initially array should not be sorted
    test_task_equal(0, t1, tasks);
    test_task_equal(1, t2, tasks);
    test_task_equal(2, t3, tasks);
    test_task_equal(3, t4, tasks);

    //sort tasks and test
    myrtos_sort_tasks();
    test_task_equal(0, t1, tasks);
    test_task_equal(1, t3, tasks);
    test_task_equal(2, t4, tasks);
    test_task_equal(3, t2, tasks);

    /* Test peek, pop, and push */
    //peek
    myRTOS_int_task_type_s t_i;
    ret = myrtos_peek_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t1", t_i.t.name);
    TEST_ASSERT_EQUAL_UINT8(0, t_i.t.priority);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, t_i.t.stack_size);
    t_i.t.priority = 5; //modifications to t_i should not modify task array
    TEST_ASSERT_EQUAL_UINT8(tasks->arr[0].t.priority, 0);
    TEST_ASSERT_EQUAL_size_t(4, tasks->len);
    test_task_equal(0, t1, tasks);
    test_task_equal(1, t3, tasks);
    test_task_equal(2, t4, tasks);
    test_task_equal(3, t2, tasks);

    //pop
    ret = myrtos_request_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t1", t_i.t.name);
    TEST_ASSERT_EQUAL_UINT8(0, t_i.t.priority);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, t_i.t.stack_size);
    TEST_ASSERT_EQUAL_size_t(3, tasks->len);
    test_task_equal(0, t4, tasks); //order gets jumbled here a little, but that's fine
    test_task_equal(1, t3, tasks); //order this is the expected order from the specific algorithm used at this step
    test_task_equal(2, t2, tasks);

    //push - here we push multiple tasks to test if they end up in the correct place
    myRTOS_task_type_s t6 = {.name = "t6", .priority = MYRTOS_PRIORITY_LEVELS-1, .stack_size = MYRTOS_MIN_STACK_SIZE};
    myRTOS_task_type_s t7 = {.name = "t7", .priority = 5, .stack_size = MYRTOS_MIN_STACK_SIZE};
    ret = myrtos_register_task_i(&t6);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    ret = myrtos_register_task_i(&t7);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    ret = myrtos_register_task_i(&t1);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);

    TEST_ASSERT_EQUAL_size_t(6, tasks->len);
    test_task_equal(0, t1, tasks); //this is the expected composition of the binary heap array at this point
    test_task_equal(1, t3, tasks); //note it is not in order, but that doesn't matter as long as the lowest priority task is on top
    test_task_equal(2, t4, tasks);
    test_task_equal(3, t6, tasks);
    test_task_equal(4, t7, tasks);
    test_task_equal(5, t2, tasks);

    /* Test popping every task */
    ret = myrtos_request_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_UINT8(0, t_i.t.priority);
    ret = myrtos_request_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_UINT8(4, t_i.t.priority);
    ret = myrtos_request_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_UINT8(4, t_i.t.priority);
    ret = myrtos_request_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_UINT8(5, t_i.t.priority);
    ret = myrtos_request_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_UINT8(6, t_i.t.priority);
    ret = myrtos_request_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_UINT8(6, t_i.t.priority);
    //should fail here with empty task queue
    ret = myrtos_request_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_TASK_QUEUE_EMPTY, ret, str);
}