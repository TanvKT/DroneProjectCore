/**
 * @file test_semaphore.c
 * @author your name (you@domain.com)
 * @brief 
 * 
 *      Tests semaphore logic, relatively basic tests due to sharing a lot of functionality with mutex system
 * 
 * @version 0.1
 * @date 2026-01-15
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

void semaphore_test()
{
    #if MYRTOS_PRIORITY_LEVELS != 7 || (MYRTOS_DYNAMIC_PRIORITY && MYRTOS_PRIO_LOWER_THRESH != 3)
    printf("MYRTOS_PRIORITY_LEVELS != 7 or MYRTOS_DYNAMIC_PRIORITY enabled and MYRTOS_PRIO_LOWER_THRESH not 3\r\n");
    printf("Semaphore Test is deterministic so it needs these values to be set accordingly, skipping\r\n");
    #else

    myRTOS_semaphore_handle_s sem1;
    myRTOS_semaphore_handle_s sem2;
    myRTOS_return_type_e ret;

    // Test default init (count 1, holding list size 1, waiting list size 2)
    ret = myrtos_semaphore_init(&sem1, 1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Semaphore init with default size failed");
    TEST_ASSERT_EQUAL_MESSAGE(1, sem1.c, "Semaphore count should be 1");

    // Test init with custom size
    ret = myrtos_semaphore_init_num(&sem2, 2, 5);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Semaphore init with custom size failed");
    TEST_ASSERT_EQUAL_MESSAGE(2, sem2.c, "Semaphore count should be 2");

    // Test if take and give possible when s_curr_task_p set to NULL
    ret = myrtos_semaphore_take(&sem1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Semaphore take on NULL task didn't fail");
    ret = myrtos_semaphore_take(&sem2);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Semaphore take on NULL task didn't fail");
    ret = myrtos_semaphore_give(&sem1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Semaphore give on NULL task didn't fail");
    ret = myrtos_semaphore_give(&sem2);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Semaphore give on NULL task didn't fail");

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

    t6->b = true;  //adding this in to supress a warning

    // Test basic take and give
    s_curr_task_p = t1;
    ret = myrtos_semaphore_take(&sem1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Semaphore take failed");
    TEST_ASSERT_EQUAL_MESSAGE(0, sem1.c, "Semaphore count should be 0 after take");
    TEST_ASSERT_EQUAL_MESSAGE(1, sem1.t_i, "Semaphore holding list should have 1 task");
    TEST_ASSERT_EQUAL_MESSAGE(t1, sem1.t[0], "t1 should be in holding list");

    ret = myrtos_semaphore_take(&sem2);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Semaphore take failed");
    TEST_ASSERT_EQUAL_MESSAGE(1, sem2.c, "Semaphore count should be 1 after take");
    TEST_ASSERT_EQUAL_MESSAGE(1, sem2.t_i, "Semaphore holding list should have 1 task");

    ret = myrtos_semaphore_give(&sem1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Semaphore give failed");
    TEST_ASSERT_EQUAL_MESSAGE(1, sem1.c, "Semaphore count should be 1 after give");
    TEST_ASSERT_EQUAL_MESSAGE(0, sem1.t_i, "Semaphore holding list should be empty after give");

    ret = myrtos_semaphore_give(&sem2);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Semaphore give failed");
    TEST_ASSERT_EQUAL_MESSAGE(2, sem2.c, "Semaphore count should be 2 after give");
    TEST_ASSERT_EQUAL_MESSAGE(0, sem2.t_i, "Semaphore holding list should be empty after give");

    // Test giving a semaphore when task doesn't hold it
    ret = myrtos_semaphore_give(&sem1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Semaphore give on free resource should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(2, sem1.c, "Semaphore count should increment even when count != 0");

    // Test taking semaphore when count > 0 (multiple holders)
    myRTOS_semaphore_handle_s sem3;
    ret = myrtos_semaphore_init_num(&sem3, 2, 5);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Semaphore init failed");

    s_curr_task_p = t1;
    ret = myrtos_semaphore_take(&sem3);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "First take should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(1, sem3.c, "Count should be 1");
    TEST_ASSERT_EQUAL_MESSAGE(1, sem3.t_i, "Holding list should have 1 task");

    s_curr_task_p = t3;
    ret = myrtos_semaphore_take(&sem3);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Second take should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(0, sem3.c, "Count should be 0");
    TEST_ASSERT_EQUAL_MESSAGE(2, sem3.t_i, "Holding list should have 2 tasks");
    TEST_ASSERT_EQUAL_MESSAGE(t1, sem3.t[0], "t1 should be first holder");
    TEST_ASSERT_EQUAL_MESSAGE(t3, sem3.t[1], "t3 should be second holder");

    // Test give when multiple tasks holding
    ret = myrtos_semaphore_give(&sem3);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Give should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(1, sem3.c, "Count should be 1");
    TEST_ASSERT_EQUAL_MESSAGE(1, sem3.t_i, "Holding list should have 1 task");
    TEST_ASSERT_EQUAL_MESSAGE(t1, sem3.t[0], "t1 should still be holder");

    // Test blocking on semaphore (count becomes 0)
    myRTOS_semaphore_handle_s sem_block;
    ret = myrtos_semaphore_init_num(&sem_block, 1, 5);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Semaphore init failed");

    s_curr_task_p = t4;
    ret = myrtos_semaphore_take(&sem_block);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Take on empty semaphore should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(0, sem_block.c, "Count should be 0");
    TEST_ASSERT_EQUAL_MESSAGE(1, sem_block.t_i, "Holding list should have 1 task");

    s_curr_task_p = t2;
    ret = myrtos_semaphore_take(&sem_block);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Take on full semaphore should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t2->b, "t2 should be blocked");
    TEST_ASSERT_EQUAL_MESSAGE(1, sem_block.t_l_i, "Waiting list should have 1 task");
    TEST_ASSERT_EQUAL_MESSAGE(t2, sem_block.t_l[0], "t2 should be in waiting list");

    // Test priority inheritance with semaphore (t2 priority 4 > t4 priority 4 is same, no boost needed)
    TEST_ASSERT_EQUAL_MESSAGE(4, t4->t.priority, "t4 priority should remain at 4");

    s_curr_task_p = t1;
    ret = myrtos_semaphore_take(&sem_block);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Take on full semaphore should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t1->b, "t1 should be blocked");
    TEST_ASSERT_EQUAL_MESSAGE(2, sem_block.t_l_i, "Waiting list should have 2 tasks");

    // t1 (priority 0) should cause t4 to be boosted to priority 0
    #ifndef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_MESSAGE(0, t4->t.priority, "t4 priority should be boosted to 0");
    #endif

    // Give and check if highest priority task is unblocked (t1 with priority 0)
    s_curr_task_p = t4;
    ret = myrtos_semaphore_give(&sem_block);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Give should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(1, sem_block.c, "Count should be 1");
    TEST_ASSERT_EQUAL_MESSAGE(0, sem_block.t_i, "Holding list should be empty");
    TEST_ASSERT_EQUAL_MESSAGE(1, sem_block.t_l_i, "Waiting list should have 1 task (t2)");
    TEST_ASSERT_EQUAL_MESSAGE(false, t1->b, "t1 should be unblocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t2->b, "t2 should still be blocked");

    // Test priority restoration after give
    #ifndef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_MESSAGE(4, t4->t.priority, "t4 priority should be restored to 4");
    #endif

    // Test multiple tasks waiting with priority ordering
    myRTOS_semaphore_handle_s sem_prio;
    ret = myrtos_semaphore_init_num(&sem_prio, 1, 10);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Semaphore init failed");

    s_curr_task_p = t7;  // priority 1
    ret = myrtos_semaphore_take(&sem_prio);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Take should succeed");

    s_curr_task_p = t5;  // priority 2
    ret = myrtos_semaphore_take(&sem_prio);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Take should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t5->b, "t5 should be blocked");

    s_curr_task_p = t2;  // priority 4
    ret = myrtos_semaphore_take(&sem_prio);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Take should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t2->b, "t2 should be blocked");

    s_curr_task_p = t1;  // priority 0
    ret = myrtos_semaphore_take(&sem_prio);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Take should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t1->b, "t1 should be blocked");
    TEST_ASSERT_EQUAL_MESSAGE(3, sem_prio.t_l_i, "Waiting list should have 3 tasks");

    // Give should unblock t1 (highest priority)
    s_curr_task_p = t7;
    ret = myrtos_semaphore_give(&sem_prio);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Give should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(false, t1->b, "t1 should be unblocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t5->b, "t5 should still be blocked");
    TEST_ASSERT_EQUAL_MESSAGE(true, t2->b, "t2 should still be blocked");
    TEST_ASSERT_EQUAL_MESSAGE(2, sem_prio.t_l_i, "Waiting list should have 2 tasks");

    // Test invalid inputs
    ret = myrtos_semaphore_init(NULL, 1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Semaphore init with NULL handle should fail");

    ret = myrtos_semaphore_init_num(NULL, 1, 5);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Semaphore init_num with NULL handle should fail");

    ret = myrtos_semaphore_take(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Semaphore take with NULL handle should fail");

    ret = myrtos_semaphore_give(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_FAIL, ret, "Semaphore give with NULL handle should fail");

    // Test semaphore initialized with negative count
    myRTOS_semaphore_handle_s sem_neg;
    ret = myrtos_semaphore_init(&sem_neg, -1);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Semaphore init with negative count should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(-1, sem_neg.c, "Semaphore count should be -1");

    // With negative count, can still give without taking
    s_curr_task_p = t1;
    ret = myrtos_semaphore_give(&sem_neg);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Give should succeed on negative count");
    TEST_ASSERT_EQUAL_MESSAGE(0, sem_neg.c, "Semaphore count should be 0 after give");

    // Test zero count semaphore
    myRTOS_semaphore_handle_s sem_zero;
    ret = myrtos_semaphore_init(&sem_zero, 0);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Semaphore init with zero count should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(0, sem_zero.c, "Semaphore count should be 0");

    s_curr_task_p = t1;
    ret = myrtos_semaphore_take(&sem_zero);
    TEST_ASSERT_EQUAL_MESSAGE(MYRTOS_SUCCESS, ret, "Take should block");
    TEST_ASSERT_EQUAL_MESSAGE(true, t1->b, "t1 should be blocked");

    #endif
}