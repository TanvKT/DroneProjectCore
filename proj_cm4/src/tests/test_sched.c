/**
 * @file test_sched.c
 * @author your name (you@domain.com)
 * @brief Test calls to the myrtos_schedule function to ensure proper tasks scheduled
 *              Also testing blocking tasks here
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
#include "myRTOS_sched.h"
#include "myRTOS_task_queue.h"

#include <string.h>

void sched_test()
{
    myRTOS_return_type_e ret;

    //to start test scheduling tasks on the same priority level
    //      this functionality should be shared by all scheduling types

    //register a couple tasks at varying priority levels
    #if MYRTOS_PRIORITY_LEVELS != 7 || (MYRTOS_DYNAMIC_PRIORITY && MYRTOS_PRIO_LOWER_THRESH != 3)
    printf("MYRTOS_PRIORITY_LEVELS != 7 or MYRTOS_DYNAMIC_PRIORITY enabled and MYRTOS_PRIO_LOWER_THRESH not 3\r\n");
    printf("Schedule Test is deterministic so it needs these values to be set correctly, skipping\r\n");
    #else
    myrtos_register_task("t1", 0, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t2", 0, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t3", 0, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t4", 1, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t5", 1, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t6", 4, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t7", 5, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t8", 5, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t9", 5, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t10", MYRTOS_PRIORITY_LEVELS-1, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t11", MYRTOS_PRIORITY_LEVELS-1, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t12", 0, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t13", 4, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t14", 2, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t15", 2, NULL, NULL, MYRTOS_MIN_STACK_SIZE);
    myrtos_register_task("t16", 1, NULL, NULL, MYRTOS_MIN_STACK_SIZE);

    char str[32];
    #ifdef MYRTOS_ROUND_ROBIN
    char* test1[] = {"t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9", "t10", "t11", "t12", "t13", "t14", "t15", "t16"};
    #define TEST1 16
    char* test2[] = {"t2", "t3", "t4", "t5", "t6", "t7", "t9", "t10", "t11", "t13", "t14", "t15", "t16",
                     "t2", "t3", "t4", "t5", "t6", "t7", "t9", "t10", "t11", "t13", "t14", "t15", "t16",
                     "t2", "t3", "t4", "t5", "t6", "t7", "t9", "t10", "t11", "t13", "t14", "t15", "t16"};
    #define TEST2 39
    char* test3[] = {"t1", "t4", "t12", "t8",
                     "t1", "t4", "t12", "t8",
                     "t1", "t4", "t12", "t8"};
    #define TEST3 12
    char* test4[] = {"t1", "t4", "t12", "t16", "t2", "t3", "t5", "t6", "t7", "t9", "t10", "t11", "t13", "t14", "t15", "t8", 
                     "t1", "t4", "t12", "t16", "t2", "t3", "t5", "t6", "t7", "t9", "t10", "t11", "t13", "t14", "t15", "t8", 
                     "t1", "t4", "t12", "t16", "t2", "t3", "t5", "t6", "t7", "t9", "t10", "t11", "t13", "t14", "t15", "t8", 
                     "t1", "t4", "t12", "t16", "t2", "t3", "t5", "t6", "t7", "t9", "t10", "t11", "t13", "t14", "t15", "t8", 
                     "t1", "t4", "t12", "t16", "t2", "t3", "t5", "t6", "t7", "t9", "t10", "t11", "t13", "t14", "t15", "t8"};
    #define TEST4 80
    #endif
    #ifdef MYRTOS_PRIORITY_BASED
    char* test1[] = {"t1", "t2", "t3", "t12",
                     "t1", "t2", "t3", "t12",
                     "t1", "t2", "t3", "t12",
                     "t1", "t2", "t3", "t12",
                     "t1", "t2", "t3", "t12"};
    #define TEST1 20
    char* test2[] = {"t2", "t3",
                     "t2", "t3",
                     "t2", "t3",
                     "t2", "t3"};
    #define TEST2 8
    char* test3[] = {"t1", "t12",
                     "t1", "t12",
                     "t1", "t12",
                     "t1", "t12"};
    #define TEST3 8
    char* test4[] = {"t1", "t3", "t2", "t12",
                     "t1", "t3", "t2", "t12",
                     "t1", "t3", "t2", "t12",
                     "t1", "t3", "t2", "t12",
                     "t1", "t3", "t2", "t12"};
    #define TEST4 20
    #endif
    #ifdef MYRTOS_DYNAMIC_PRIORITY
    char* test1[] = {"t1", "t2", "t3", "t12",
                     "t1", "t2", "t3", "t12",
                     "t1", "t2", "t3", "t12",
                     "t4", "t5", "t16", "t1", "t2", "t3", "t12", //priority should lower here
                     "t4", "t5", "t16", "t1", "t2", "t3", "t12"}; 
    #define TEST1 26
    char* test2[] = {"t4", "t5", "t16", "t2", "t3",                           //t4 blocked, then unblocked
                     "t4", "t4", "t14", "t15", "t5", "t16", "t2", "t3",       //should reset counter
                     "t4", "t14", "t15", "t5", "t16", "t2", "t3",
                     "t4", "t14", "t15", "t5", "t16", "t2", "t3"};
    #define TEST2 27
    char* test3[] = {"t1", "t12", //priority should have been reset here
                     "t1", "t12",
                     "t1", "t12", //priority lowers
                     "t4", "t1", "t12"}; 
    #define TEST3 9
    char* test4[] = {"t3", "t2", //priority 0
                     "t3", "t2",
                     "t3", "t2", //lowers here
                     "t4", "t1", "t5", "t16", "t12", "t3", "t2", //priority 1
                     "t4", "t1", "t5", "t16", "t12", "t3", "t2", //t1, t4, t12 lower
                     "t5", "t16", "t3", "t2"};
    #define TEST4 24
    #endif

    //test pulling tasks, functionality depends on current scheduler
    for (int i = 0; i < TEST1; i++)
    {
        #ifdef MYRTOS_DEBUG_MODE
        printf("SCHED 1 [%d/%d]\r\n", i, TEST1-1);
        #endif
        myrtos_schedule();
        TEST_ASSERT_EQUAL_STRING(test1[i], s_curr_task_p->t.name); //simply testing identifier name here as proxy for full task
    }

    //block a couple tasks
    myRTOS_queue_arr_s* tasks = myrtos_get_task_queue();
    #ifdef MYRTOS_ROUND_ROBIN
    myRTOS_int_task_type_s* t1 =  tasks->level[0].arr[0];
    myRTOS_int_task_type_s* t4 =  tasks->level[0].arr[3];
    myRTOS_int_task_type_s* t8 =  tasks->level[0].arr[7];
    myRTOS_int_task_type_s* t12 = tasks->level[0].arr[11];
    #else
    myRTOS_int_task_type_s* t1 =  tasks->level[0].arr[0];
    myRTOS_int_task_type_s* t4 =  tasks->level[1].arr[0];
    myRTOS_int_task_type_s* t8 =  tasks->level[5].arr[1];
    myRTOS_int_task_type_s* t12 = tasks->level[0].arr[3];
    #endif

    ret = myrtos_block_task(t1);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL(true, t1->b);
    TEST_ASSERT_EQUAL_size_t(-1, t1->b_i);

    ret = myrtos_block_task(t4);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL(true, t4->b);
    TEST_ASSERT_EQUAL_size_t(-1, t4->b_i);

    ret = myrtos_block_task(t8);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL(true, t8->b);
    TEST_ASSERT_EQUAL_size_t(-1, t8->b_i);

    ret = myrtos_block_task(t12);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL(true, t12->b);
    TEST_ASSERT_EQUAL_size_t(-1, t12->b_i);

    //blocked queue should still be empty at this point
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, tasks->blocked.len, "Blocked Task Arr Len not zero");

    //should be unblock a task before schedule
    ret = myrtos_unblock_task(t4);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);

    //run through scheduler again and make sure blocked tasks never returned
    for (int i = 0; i < TEST2; i++)
    {
        #ifdef MYRTOS_DEBUG_MODE
        printf("SCHED 2 [%d/%d]\r\n", i, TEST2-1);
        #endif
        myrtos_schedule();
        TEST_ASSERT_EQUAL_STRING(test2[i], s_curr_task_p->t.name); //simply testing identifier name here as proxy for full task
    }

    //block all tasks
    ret = myrtos_block_all();
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    
    //running through scheduler now should only return the idle task
    myRTOS_int_task_type_s* idle = myrtos_get_idle_task();
    for (int i = 0; i < 200; i++)
    {
        #ifdef MYRTOS_DEBUG_MODE
        printf("SCHED IDLE 1 [%d/%d]\r\n", i, 199);
        #endif
        myrtos_schedule();
        TEST_ASSERT_EQUAL_PTR(idle, s_curr_task_p);
    }

    //unblock a task
    ret = myrtos_unblock_task(t8);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    //should only get this one task
    for (int i = 0; i < 200; i++)
    {
        #ifdef MYRTOS_DEBUG_MODE
        printf("SCHED T8 [%d/%d]\r\n", i, 199);
        #endif
        myrtos_schedule();
        TEST_ASSERT_EQUAL_PTR(t8, s_curr_task_p);
    }

    //unblock other tasks
    ret = myrtos_unblock_task(t1);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    ret = myrtos_unblock_task(t4);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    ret = myrtos_unblock_task(t12);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);

    // Ensure only scheduling those tasks
    for (int i = 0; i < TEST3; i++)
    {
        #ifdef MYRTOS_DEBUG_MODE
        printf("SCHED 3 [%d/%d]\r\n", i, TEST3-1);
        #endif
        myrtos_schedule();
        TEST_ASSERT_EQUAL_STRING(test3[i], s_curr_task_p->t.name);
    }

    // Unblock all
    ret = myrtos_unblock_all();
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    for (int i = 0; i < TEST4; i++)
    {
        #ifdef MYRTOS_DEBUG_MODE
        printf("SCHED 4 [%d/%d]\r\n", i, TEST4-1);
        #endif
        myrtos_schedule();
        TEST_ASSERT_EQUAL_STRING(test4[i], s_curr_task_p->t.name);
    }

    //set fatal flag
    ret = myrtos_set_fatal();
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    //running through scheduler now should only return the idle task
    for (int i = 0; i < 200; i++)
    {
        #ifdef MYRTOS_DEBUG_MODE
        printf("SCHED IDLE 2 [%d/%d]\r\n", i, 199);
        #endif
        myrtos_schedule();
        TEST_ASSERT_EQUAL_PTR(idle, s_curr_task_p);
    }

    // Test done
    #endif
}