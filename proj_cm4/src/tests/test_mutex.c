/**
 * @file test_locks.c
 * @author your name (you@domain.com)
 * @brief Barebones testing of locks to ensure function calls execute correctly
 * 
 *              Proper testing has to be done in a running RTOS environement and will occur after the
 *                  Unity tests complete
 * 
 * @version 0.1
 * @date 2025-09-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "tests.h"
#include "unity.h"
#include "myRTOS.h"
#include "myRTOS_types.h"
#include "myRTOS_task_queue.h"
#include "myRTOS_sched.h"

void mutex_test()
{
    #if MYRTOS_PRIORITY_LEVELS != 7 || (MYRTOS_DYNAMIC_PRIORITY && MYRTOS_PRIO_LOWER_THRESH != 3)
    printf("MYRTOS_PRIORITY_LEVELS != 7 or MYRTOS_DYNAMIC_PRIORITY enabled and MYRTOS_PRIO_LOWER_THRESH not 3\r\n");
    printf("Mutex Test is deterministic so it needs these values to be set accordingly, skipping\r\n");
    #else

    myRTOS_mutex_handle_s mutex1;
    myRTOS_mutex_handle_s mutex2;
    myRTOS_return_type_e ret;

    // Test default init (size 2)
    ret = myrtos_mutex_init(&mutex1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex init with default size failed");

    // Test init with custom size
    ret = myrtos_mutex_init_num(&mutex2, 5);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex init with custom size failed");

    // Test if take and give possible when s_curr_task_p set to NULL
    ret = myrtos_mutex_take(&mutex1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Mutex take on NULL task didn't fail");
    ret = myrtos_mutex_take(&mutex2);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Mutex take on NULL task didn't fail");
    ret = myrtos_mutex_give(&mutex1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Mutex give on NULL task didn't fail");
    ret = myrtos_mutex_give(&mutex2);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Mutex give on NULL task didn't fail");

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

    s_curr_task_p = t1;
    // Test take and give
    ret = myrtos_mutex_take(&mutex1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex take failed");
    TEST_ASSERT_EQUAL_MESSAGE(t1, mutex1.t, "Mutex1 should be held by t1");

    ret = myrtos_mutex_take(&mutex2);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex take failed");
    TEST_ASSERT_EQUAL_MESSAGE(t1, mutex2.t, "Mutex2 should be held by t1");

    ret = myrtos_mutex_give(&mutex1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex give failed");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, mutex1.t, "Mutex1 holder should be NULL after give");

    ret = myrtos_mutex_give(&mutex2);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex give failed");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, mutex2.t, "Mutex2 holder should be NULL after give");

    // Test giving a mutex not held by current task
    ret = myrtos_mutex_give(&mutex1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Giving free mutex should fail");

    // Test taking already taken mutex (should block the task and boost priority if inheritance)
    ret = myrtos_mutex_take(&mutex1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "First take should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(t1, mutex1.t, "Mutex1 held by t1");

    s_curr_task_p = t2;  // Switch to t2 (lower priority)
    ret = myrtos_mutex_take(&mutex1);  // This should block t2 and not boost t1's priority

    TEST_ASSERT_EQUAL_MESSAGE(true, t2->b, "t2 should be blocked after trying to take held mutex");

    // Check priority inheritance: t1 (priority 0) should be 0
    TEST_ASSERT_EQUAL_MESSAGE(0, t1->t.priority, "t1 priority should remain at 0");
    #ifdef MYRTOS_DYNAMIC_PRIORITY
    TEST_ASSERT_EQUAL_MESSAGE(0, t1->o_prio, "t1 original priority should remain 0");
    #endif

    // Check mutex waiting list
    TEST_ASSERT_EQUAL_MESSAGE(1, mutex1.t_i, "Mutex1 waiting index should be 1");
    TEST_ASSERT_EQUAL_MESSAGE(t2, mutex1.t_l[0], "t2 should be in mutex1 waiting list");

    // Now, switch back to t1 and give the mutex
    s_curr_task_p = t1;
    ret = myrtos_mutex_give(&mutex1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Give should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(NULL, mutex1.t, "Mutex1 holder should be NULL");

    // After give, t1 priority should be restored
    TEST_ASSERT_EQUAL_MESSAGE(0, t1->t.priority, "t1 priority should be restored to 0");

    // t2 should be unblocked
    TEST_ASSERT_EQUAL_MESSAGE(false, t2->b, "t2 should be unblocked after give");

    // Blocked queue should be empty
    TEST_ASSERT_EQUAL_MESSAGE(0, tasks->blocked.len, "Blocked queue should be empty after unblock");

    // Mutex waiting list should be empty
    TEST_ASSERT_EQUAL_MESSAGE(0, mutex1.t_i, "Mutex1 waiting index should be 0");

    // Test mutex with size 1 (only one waiting slot)
    myRTOS_mutex_handle_s mutex_small;
    ret = myrtos_mutex_init_num(&mutex_small, 1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex init with size 1 failed");
    TEST_ASSERT_EQUAL_MESSAGE(1, mutex_small.n, "Mutex small should have n=1");

    // Test priority inheritance and multiple tasks waiting on single lock
    s_curr_task_p = t4;
    ret = myrtos_mutex_take(&mutex_small);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex failed take on t4");

    s_curr_task_p = t2;
    ret = myrtos_mutex_take(&mutex_small);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex failed take on t2");
    #ifdef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_MESSAGE(0, t4->t.priority, "Priority of t4 after t2 attempts take incorrect");
    #else
    TEST_ASSERT_EQUAL_MESSAGE(4, t4->t.priority, "Priority of t4 after t2 attempts take incorrect");
    #endif

    s_curr_task_p = t5;
    ret = myrtos_mutex_take(&mutex_small);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex failed take on t5");
    #ifdef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_MESSAGE(0, t4->t.priority, "Priority of t4 after t5 attempts take incorrect");
    #else
    TEST_ASSERT_EQUAL_MESSAGE(2, t4->t.priority, "Priority of t4 after t5 attempts take incorrect");
    #endif

    s_curr_task_p = t1;
    ret = myrtos_mutex_take(&mutex_small);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex failed take on t1");
    TEST_ASSERT_EQUAL_MESSAGE(0, t4->t.priority, "Priority of t4 after t1 attempts take incorrect");

    s_curr_task_p = t3;
    ret = myrtos_mutex_take(&mutex_small);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex failed take on t3");
    TEST_ASSERT_EQUAL_MESSAGE(0, t4->t.priority, "Priority of t4 after t3 attempts take incorrect");

    s_curr_task_p = t6;
    ret = myrtos_mutex_take(&mutex_small);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex failed take on t6");
    TEST_ASSERT_EQUAL_MESSAGE(0, t4->t.priority, "Priority of t4 after t6 attempts take incorrect");

    s_curr_task_p = t7;
    ret = myrtos_mutex_take(&mutex_small);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex failed take on t7");
    TEST_ASSERT_EQUAL_MESSAGE(0, t4->t.priority, "Priority of t4 after t7 attempts take incorrect");

    //ensure blocked list has correct length
    TEST_ASSERT_EQUAL_MESSAGE(8, mutex_small.n, "Mutex waiting list total size should be 8");
    TEST_ASSERT_EQUAL_MESSAGE(6, mutex_small.t_i, "Mutex waiting list length not 6");
    TEST_ASSERT_EQUAL_MESSAGE(true, t1->b, "t1 not blocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t2->b, "t2 not blocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t3->b, "t3 not blocked");
    TEST_ASSERT_EQUAL_MESSAGE(false, t4->b, "t4 blocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t5->b, "t5 not blocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t6->b, "t6 not blocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t7->b, "t7 not blocked");

    // Give up mutex resource
    s_curr_task_p = t4;
    ret = myrtos_mutex_give(&mutex_small);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Give failed on t4 after all tasks waiting");
    // Ensure t4 has correct priority
    #ifdef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_MESSAGE(0, t4->t.priority, "Priority of t4 after give incorrect");
    #else
    TEST_ASSERT_EQUAL_MESSAGE(4, t4->t.priority, "Priority of t4 after give incorrect");
    #endif
    // Ensure blocked fields correct
    TEST_ASSERT_EQUAL_MESSAGE(8, mutex_small.n, "Mutex waiting list total size should be 8 after give");
    TEST_ASSERT_EQUAL_MESSAGE(0, mutex_small.t_i, "Mutex waiting list length not 0 after give");
    TEST_ASSERT_EQUAL_MESSAGE(false, t1->b, "t1 blocked");
    TEST_ASSERT_EQUAL_MESSAGE(false, t2->b, "t2 blocked");
    TEST_ASSERT_EQUAL_MESSAGE(false, t3->b, "t3 blocked");
    TEST_ASSERT_EQUAL_MESSAGE(false, t4->b, "t4 blocked");
    TEST_ASSERT_EQUAL_MESSAGE(false, t5->b, "t5 blocked");
    TEST_ASSERT_EQUAL_MESSAGE(false, t6->b, "t6 blocked");
    TEST_ASSERT_EQUAL_MESSAGE(false, t7->b, "t7 blocked");

    // Test with the scheduler if using dynamic priority to ensure that tasks blocked by a lock return to correct priority
    myrtos_reset();
    myrtos_register_task("t1", 0, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t2", 0, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t3", 1, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    tasks = myrtos_get_task_queue();
    #ifdef MYRTOS_ROUND_ROBIN
    t1 = tasks->level[0].arr[0];
    t2 = tasks->level[0].arr[1];
    t3 = tasks->level[0].arr[2];
    #else
    t1 = tasks->level[0].arr[0];
    t2 = tasks->level[0].arr[1];
    t3 = tasks->level[1].arr[0];
    #endif
    ret = myrtos_mutex_init_num(&mutex_small, 1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex init with size 1 failed");
    TEST_ASSERT_EQUAL_MESSAGE(1, mutex_small.n, "Mutex small should have n=1");

    // t3 will take mutex first then t1 will be blocked
    myrtos_block_task(t1);
    myrtos_block_task(t2);
    myrtos_schedule();

    // curr task should be t3
    TEST_ASSERT_EQUAL_MESSAGE(t3, s_curr_task_p, "t3 not curr task after t1 and t2 blocked");
    ret = myrtos_mutex_take(&mutex_small);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex take failed");

    // unblock t1 and t2
    myrtos_unblock_all();

    myrtos_schedule();
    //curr task should now be t1
    TEST_ASSERT_EQUAL_MESSAGE(t1, s_curr_task_p, "curr task not t1");
    ret = myrtos_mutex_take(&mutex_small);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex take failed");

    //t3 should now be priority 0
    TEST_ASSERT_EQUAL_MESSAGE(0, t3->t.priority, "Priority of t3 after t1 take not 0");

    #ifdef MYRTOS_DYNAMIC_PRIORITY
    t3->trig = 0;  //doing this just to make this test a little easier
    #endif

    //scheduler should only see t2 and t3
    for (int i = 0; i < 7; i++)
    {
        myrtos_schedule();
        TEST_ASSERT_EQUAL_MESSAGE(t2, s_curr_task_p, "curr task not t2");
        myrtos_schedule();
        TEST_ASSERT_EQUAL_MESSAGE(t3, s_curr_task_p, "curr task not t3");
    }

    //if using dynamic scheduling t2 and t3 priority should now be 2
    #ifdef MYRTOS_DYNAMIC_PRIORITY
    TEST_ASSERT_EQUAL_MESSAGE(2, t2->t.priority, "t2 priority incorrect");
    TEST_ASSERT_EQUAL_MESSAGE(2, t3->t.priority, "t3 priority incorrect");
    #endif

    //when t3 gives up the lock, t1 should be unblocked
    //  and t3 priority should be 2 if using dynamic scheduling, 1 if priority based, and 0 if round robin
    ret = myrtos_mutex_give(&mutex_small);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex give failed");
    #ifdef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_MESSAGE(0, t3->t.priority, "Priority of t3 after give incorrect");
    #elif MYRTOS_PRIORITY_BASED
    TEST_ASSERT_EQUAL_MESSAGE(1, t3->t.priority, "Priority of t3 after give incorrect");
    #else
    TEST_ASSERT_EQUAL_MESSAGE(2, t3->t.priority, "Priority of t3 after give incorrect");
    TEST_ASSERT_EQUAL_MESSAGE(0, t1->trig, "t1 trigger not set to 0"); //trigger value should also be reset
    #endif


    // Test invalid inputs
    ret = myrtos_mutex_init(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Mutex init with NULL handle should fail");

    ret = myrtos_mutex_init_num(NULL, 5);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Mutex init_num with NULL handle should fail");

    ret = myrtos_mutex_take(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Mutex take with NULL handle should fail");

    ret = myrtos_mutex_give(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Mutex give with NULL handle should fail");

    // Test init with size 0 (should still work, but no waiting list)
    myRTOS_mutex_handle_s mutex_zero;
    ret = myrtos_mutex_init_num(&mutex_zero, 0);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Mutex init with size 0 failed");
    TEST_ASSERT_EQUAL_MESSAGE(1, mutex_zero.n, "Mutex zero should have n=1");

    #endif
}