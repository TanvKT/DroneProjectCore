/**
 * @file test_task_arr.c
 * @author your name (you@domain.com)
 * @brief Tests task array priority queue functionality
 * @version 0.1
 * @date 2025-08-22
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
#include <string.h>

static void test_task_equal(int i, myRTOS_task_type_s t, myRTOS_task_queue_s* tasks)
{
    TEST_ASSERT_EQUAL_STRING(t.name, tasks->arr[i].t.name);
    TEST_ASSERT_EQUAL_UINT8(t.priority, tasks->arr[i].t.priority);
    TEST_ASSERT_EQUAL_size_t(t.stack_size, tasks->arr[i].t.stack_size);
}

void task_array_test()
{
    #define MYRTOS_PRIORITY_LEVELS 7
    myRTOS_task_queue_s* tasks;
    myRTOS_return_type_e ret;
    char str[64];


    /* Test initialization of task array */
    tasks = myrtos_get_task_queue();
    TEST_ASSERT_EQUAL_PTR_MESSAGE(myrtos_get_task_arr_bp(), tasks,                          "Value of task queue pointer not equal to start of memory region");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, tasks->len,                                         "Task queue not empty on initialization");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)tasks + sizeof(myRTOS_queue_handle_s), tasks->arr, "Task Array Pointer not pointing to correct location in memory");

    /* Test Round Robin */
    #ifndef MYRTOS_ROUND_ROBIN
    #define MYRTOS_ROUND_ROBIN
    #undef  MYRTOS_PRIORITY_BASED
    #undef  MYRTOS_DYNAMIC_PRIORITY
    #endif

    //register a task
    myRTOS_task_type_s t1 = {.name = "t1", .priority=0, .stack_size=MYRTOS_MIN_STACK_SIZE};
    ret = myrtos_register_task_i(&t1);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    test_task_equal(0, t1, tasks);

    //round robin shouldn't care about priority levels
    myRTOS_task_type_s t2 = {.name = "t2", .priority=100, .stack_size=MYRTOS_MIN_STACK_SIZE/2}; //pushing lower stack size, should be ceil'd to MIN
                                                                                                //testing this later in stack testing section though
    ret = myrtos_register_task_i(&t2);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    test_task_equal(1, t2, tasks);

    //test sort (shouldn't do anything)
    myRTOS_task_type_s t3 = {.name = "t3", .priority=4, .stack_size=MYRTOS_MIN_STACK_SIZE*6};
    ret = myrtos_register_task_i(&t3);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    test_task_equal(2, t3, tasks);
    myrtos_sort_tasks();
    test_task_equal(0, t1, tasks);
    test_task_equal(1, t2, tasks);
    test_task_equal(2, t3, tasks);  //task array should not be modified

    //test allocated stack pointers
    myRTOS_task_type_s t4 = {.name = "t4", .priority=4, .stack_size=MYRTOS_MIN_STACK_SIZE*6};
    ret = myrtos_register_task_i(&t4);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    void* sp = myrtos_get_stack_bp();
    TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE - 1,    tasks->arr[0].sp, "Task PTR at index 0 didn't match");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE*2 - 1,  tasks->arr[1].sp, "Task PTR at index 1 didn't match");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE*8 - 1,  tasks->arr[2].sp, "Task PTR at index 2 didn't match");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE*14 - 1, tasks->arr[3].sp, "Task PTR at index 3 didn't match");

    //test allocating with too large of stack size
    myRTOS_task_type_s t5 = {.name = "t4", .priority=4, .stack_size=MYRTOS_STACK_SIZE};
    ret = myrtos_register_task_i(&t5);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_MEMORY_LIMIT_REACHED, ret, str);
    TEST_ASSERT_NOT_EQUAL_UINT8(t5.priority, tasks->arr[4].t.priority);
    TEST_ASSERT_NOT_EQUAL_size_t(t5.stack_size, tasks->arr[4].t.stack_size);

    //test that we can still allocate a task
    ret = myrtos_register_task_i(&t1);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    test_task_equal(4, t1, tasks);

    //test allocating maximum amount of tasks
    myrtos_reset();
    tasks = myrtos_get_task_queue();
    sp = myrtos_get_stack_bp();
    printf("\n");
    for (int i = 0; i < MYRTOS_MAX_TASKS; i++)
    {
        printf("Testing Iteration: [%d/%d]\n", i, MYRTOS_MAX_TASKS - 1);
        ret = myrtos_register_task_i(&t1);
        sprintf(str, myrtos_debug_print(ret));
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
        TEST_ASSERT_EQUAL_STRING_MESSAGE(t1.name, tasks->arr[i].t.name,                         "Name not matching within max allocate loop");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(t1.priority, tasks->arr[i].t.priority,                  "Priority not matching within max allocate loop");
        TEST_ASSERT_EQUAL_size_t_MESSAGE(t1.stack_size, tasks->arr[i].t.stack_size,             "Stack Size not matching within max allocate loop");
        TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE*(i+1) - 1, tasks->arr[i].sp,   "Task PTR within max allocate loop didn't match");
    }
    //ensure that memory at bounds does not exceed allocated stack block
    TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE*MYRTOS_MAX_TASKS, myrtos_get_heap_bp(), "End of allocated stack block not pointing to start of heap");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(tasks->arr[MYRTOS_MAX_TASKS-1].sp + 1, myrtos_get_heap_bp(), "Stack pointer for last task does not point to correct location");

    //test allocating another task when full
    ret = myrtos_register_task_i(&t2);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_TASK_LIMIT_REACHED, ret, str);
    TEST_ASSERT_NOT_EQUAL_UINT8(t2.priority, tasks->arr[MYRTOS_MAX_TASKS].t.priority);
    TEST_ASSERT_NOT_EQUAL_size_t(t2.stack_size, tasks->arr[MYRTOS_MAX_TASKS].t.stack_size);
    //ensure heap header not modified
    myrtos_block_header_s** headers = malloc(sizeof(myrtos_block_header_s*) * 30);
    size_t n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, n, "Number of headers returned on empty heap incorrect");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(headers[0], myrtos_get_heap_bp(), "First header address does not match starting address of heap");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0,headers[0]->size, "Top header size not zero with no allocations");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(NULL, headers[1]->next, "End header next value is not NULL");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[1]->size, "End header size not zero");

    /* Test Priority Based and Dynamic */
    /* Mostly just going to test priority queue functionality here as the functionality for testing stack size and everything is the same */
    #undef   MYRTOS_ROUND_ROBIN
    #define  MYRTOS_PRIORITY_BASED //priority based and dynamic priority have same functionality here
    myrtos_reset();
}