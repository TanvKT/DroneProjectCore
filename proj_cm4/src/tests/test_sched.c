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
    char* test3[] = {"t8", "t1", "t4", "t12",
                     "t8", "t1", "t4", "t12",
                     "t8", "t1", "t4", "t12"};
    #define TEST3 12
    char* test4[] = {"t8", "t1", "t4", "t12", "t2", "t3", "t5", "t6", "t7", "t9", "t10", "t11", "t13", "t14", "t15", "t16",
                     "t8", "t1", "t4", "t12", "t2", "t3", "t5", "t6", "t7", "t9", "t10", "t11", "t13", "t14", "t15", "t16",
                     "t8", "t1", "t4", "t12", "t2", "t3", "t5", "t6", "t7", "t9", "t10", "t11", "t13", "t14", "t15", "t16",
                     "t8", "t1", "t4", "t12", "t2", "t3", "t5", "t6", "t7", "t9", "t10", "t11", "t13", "t14", "t15", "t16",
                     "t8", "t1", "t4", "t12", "t2", "t3", "t5", "t6", "t7", "t9", "t10", "t11", "t13", "t14", "t15", "t16"};
    #define TEST4 80
    #elif MYRTOS_PRIORITY_BASED
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
    char* test4[] = {"t1", "t12", "t2", "t3",
                     "t1", "t12", "t2", "t3",
                     "t1", "t12", "t2", "t3",
                     "t1", "t12", "t2", "t3",
                     "t1", "t12", "t2", "t3"};
    #define TEST4 20
    #else   //DYNAMIC_PRIORITY
    char* test1[] = {"t1", "t2", "t3", "t12",
                     "t1", "t2", "t3", "t12",
                     "t1", "t2", "t3", "t12",
                     "t1", "t2", "t3", "t12",
                     "t4", "t5", "t16", "t1", "t2", "t3", "t12"}; //priority should lower here
    #define TEST1 23
    char* test2[] = {"t4", "t5", "t16", "t2", "t3",
                     "t4", "t5", "t16", "t2", "t3",
                     "t4", "t5", "t16", "t2", "t3",
                     "t14", "t14", "t4", "t5", "t16", "t2", "t3"};
    #define TEST2 22
    char* test3[] = {"t1", "t12", //priority should have been reset here
                     "t1", "t12",
                     "t1", "t12",
                     "t1", "t12"}; //priority lowers, but no other tasks avaliable
    #define TEST3 4*2
    char* test4[] = {"t2", "t3", //priority 0
                     "t2", "t3",
                     "t2", "t3",
                     "t2", "t3",
                     "t1", "t12", "t4", "t5", "t16", "t2", "t3", //priority 1
                     "t1", "t12", "t4", "t5", "t16", "t2", "t3"};
    #define TEST4 22
    #endif

    //test pulling tasks, functionality depends on current scheduler
    for (int i = 0; i < TEST1; i++)
    {
        myrtos_schedule();
        TEST_ASSERT_EQUAL_STRING(s_curr_task_p->t.name, test1[TEST1]); //simply testing identifier name here as proxy for full task
    }

    //block a couple tasks
    myRTOS_queue_arr_s* tasks = myrtos_get_task_queue();
    myRTOS_int_task_type_s* t1 =  tasks->level[0].arr[0];
    myRTOS_int_task_type_s* t4 =  tasks->level[0].arr[3];
    myRTOS_int_task_type_s* t8 =  tasks->level[0].arr[7];
    myRTOS_int_task_type_s* t12 = tasks->level[0].arr[11];

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
        myrtos_schedule();
        TEST_ASSERT_EQUAL_STRING(s_curr_task_p->t.name, test2[i]); //simply testing identifier name here as proxy for full task
    }

    //block all tasks
    ret = myrtos_block_all();
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    
    //running through scheduler now should only return the idle task
    myRTOS_int_task_type_s* idle = myrtos_get_idle_task();
    for (int i = 0; i < 200; i++)
    {
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
        myrtos_schedule();
        TEST_ASSERT_EQUAL_STRING(s_curr_task_p->t.name, test3[i]);
    }

    // Unblock all
    ret = myrtos_unblock_all();
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    for (int i = 0; i < TEST4; i++)
    {
        myrtos_schedule();
        TEST_ASSERT_EQUAL_STRING(s_curr_task_p->t.name, test4[i]);
    }

    // Test done
    #endif
}