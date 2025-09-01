/**
 * @file test_task_arr.c
 * @author your name (you@domain.com)
 * @brief Tests task array
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
#include "myRTOS_sched.h"
#include <string.h>

void task_array_test()
{
    myRTOS_queue_arr_s* tasks;
    myRTOS_return_type_e ret;
    char str[64];


    /* Test initialization of task array */
    tasks = myrtos_get_task_queue();
    TEST_ASSERT_EQUAL_PTR_MESSAGE(myrtos_get_task_arr_bp(), tasks, "Value of task queue pointer not equal to start of memory region");
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        printf("Priority Level [%d/%d]\r\n", i, MYRTOS_QUEUE_ARR_LEN-1);
        TEST_ASSERT_EQUAL_size_t_MESSAGE(0, tasks->level[i].len, "Task queue len not 0");
        TEST_ASSERT_EQUAL_size_t_MESSAGE(0, tasks->level[i].st,  "Task queue start index not 0");
        TEST_ASSERT_EQUAL_size_t_MESSAGE(0, tasks->level[i].en,  "Task queue end index not 0");
    }
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, tasks->blocked.len, "Blocked array len not 0");

    //register a task
    myRTOS_task_type_s t1 = {.name = "t1", .priority=0, .stack_size=MYRTOS_MIN_STACK_SIZE};
    ret = myrtos_register_task_i(&t1);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t1", tasks->level[0].arr[0].t.name);
    TEST_ASSERT_EQUAL_UINT8(0, tasks->level[0].arr[0].t.priority);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, tasks->level[0].arr[0].t.stack_size);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].en);

    //ensure all other priority levels remain unchanged
    for (uint8_t i = 1; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    TEST_ASSERT_EQUAL_size_t(0, tasks->blocked.len);


    //priority level should be truncated to MYRTOS_QUEUE_ARR_LEN
    myRTOS_task_type_s t2 = {.name = "t2", .priority=MYRTOS_PRIORITY_LEVELS-1, .stack_size=MYRTOS_MIN_STACK_SIZE/2}; //pushing lower stack size, should be ceil'd to MIN                                                                                         
    ret = myrtos_register_task_i(&t2);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    #ifdef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_STRING("t2", tasks->level[MYRTOS_QUEUE_ARR_LEN-1].arr[1].t.name);
    TEST_ASSERT_EQUAL_UINT8(MYRTOS_QUEUE_ARR_LEN-1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].arr[1].t.priority);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].arr[1].t.stack_size);
    TEST_ASSERT_EQUAL_size_t(2, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].st);
    TEST_ASSERT_EQUAL_size_t(2, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].en);
    #else
    TEST_ASSERT_EQUAL_STRING("t2", tasks->level[MYRTOS_QUEUE_ARR_LEN-1].arr[0].t.name);
    TEST_ASSERT_EQUAL_UINT8(MYRTOS_QUEUE_ARR_LEN-1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].arr[0].t.priority);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].arr[0].t.stack_size);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].en);
    //ensure all other priority levels remain unchanged
    for (uint8_t i = 1; i < MYRTOS_QUEUE_ARR_LEN-1; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif
    TEST_ASSERT_EQUAL_size_t(0, tasks->blocked.len);

    //test pushing task with different stack size
    myRTOS_task_type_s t3 = {.name = "t3", .priority=0, .stack_size=MYRTOS_MIN_STACK_SIZE*6};
    ret = myrtos_register_task_i(&t3);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    #ifdef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_STRING("t3", tasks->level[0].arr[2].t.name);
    TEST_ASSERT_EQUAL_UINT8(0, tasks->level[0].arr[2].t.priority);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE*6, tasks->level[0].arr[2].t.stack_size);
    TEST_ASSERT_EQUAL_size_t(3, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(3, tasks->level[0].en);
    #else
    TEST_ASSERT_EQUAL_STRING("t3", tasks->level[0].arr[1].t.name);
    TEST_ASSERT_EQUAL_UINT8(0, tasks->level[0].arr[1].t.priority);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE*6, tasks->level[0].arr[1].t.stack_size);
    TEST_ASSERT_EQUAL_size_t(2, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(2, tasks->level[0].en);
    //ensure all other priority levels remain unchanged
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].en);
    for (uint8_t i = 1; i < MYRTOS_QUEUE_ARR_LEN-1; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif
    TEST_ASSERT_EQUAL_size_t(0, tasks->blocked.len);

    //test adding task at a different priority level (only if MYRTOS_ROUND_ROBIN not enabled)
    myRTOS_task_type_s t4 = {.name = "t4", .priority=4, .stack_size=MYRTOS_MIN_STACK_SIZE*6};
    ret = myrtos_register_task_i(&t4);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    #ifdef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_STRING("t4", tasks->level[0].arr[3].t.name);
    TEST_ASSERT_EQUAL_UINT8(0, tasks->level[0].arr[3].t.priority);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE*6, tasks->level[0].arr[3].t.stack_size);
    TEST_ASSERT_EQUAL_size_t(4, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(4, tasks->level[0].en);
    #else
    TEST_ASSERT_EQUAL_STRING("t4", tasks->level[4].arr[0].t.name);
    TEST_ASSERT_EQUAL_UINT8(4, tasks->level[4].arr[0].t.priority);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE*6, tasks->level[4].arr[0].t.stack_size);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[4].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[4].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[4].en);
    //ensure all other priority levels remain unchanged
    TEST_ASSERT_EQUAL_size_t(2, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(2, tasks->level[0].en);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].en);
    for (uint8_t i = 1; i < MYRTOS_QUEUE_ARR_LEN-1; i++)
    {
        if (4==i) continue;
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif
    TEST_ASSERT_EQUAL_size_t(0, tasks->blocked.len);

    //test stack pointers
    void* sp = myrtos_get_stack_bp();
    #ifdef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE,    tasks->level[0].arr[0].sp,                        "Task1 PTR didn't match");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE*2,  tasks->level[0].arr[1].sp,                        "Task2 PTR didn't match");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE*8,  tasks->level[0].arr[2].sp,                        "Task3 PTR didn't match");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE*14, tasks->level[0].arr[3].sp,                        "Task4 PTR didn't match");
    #else
    TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE,    tasks->level[0].arr[0].sp,                        "Task1 PTR didn't match");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE*2,  tasks->level[MYRTOS_QUEUE_ARR_LEN-1].arr[0].sp,   "Task2 PTR didn't match");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE*8,  tasks->level[0].arr[1].sp,                        "Task3 PTR didn't match");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE*14, tasks->level[4].arr[0].sp,                        "Task4 PTR didn't match");
    #endif

    //test allocating with too large of stack size
    myRTOS_task_type_s t5 = {.name = "t5", .priority=0, .stack_size=MYRTOS_STACK_SIZE};
    ret = myrtos_register_task_i(&t5);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_MEMORY_LIMIT_REACHED, ret, str);
    //ensure no changes made to task queue
    #ifdef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_size_t(4, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(4, tasks->level[0].en);
    #else
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[4].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[4].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[4].en);
    TEST_ASSERT_EQUAL_size_t(2, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(2, tasks->level[0].en);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].en);
    for (uint8_t i = 1; i < MYRTOS_QUEUE_ARR_LEN-1; i++)
    {
        if (4==i) continue;
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif
    TEST_ASSERT_EQUAL_size_t(0, tasks->blocked.len);

    //test that we can still allocate a task
    ret = myrtos_register_task_i(&t1);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);

    //just checking value of new task added
    //at this point we have tested that the array doesn't get modified incorrectly
    #ifdef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_STRING("t1", tasks->level[0].arr[4].t.name);
    TEST_ASSERT_EQUAL_UINT8(0, tasks->level[0].arr[4].t.priority);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, tasks->level[0].arr[4].t.stack_size);
    TEST_ASSERT_EQUAL_size_t(5, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(5, tasks->level[0].en);
    #else
    TEST_ASSERT_EQUAL_STRING("t1", tasks->level[0].arr[2].t.name);
    TEST_ASSERT_EQUAL_UINT8(0, tasks->level[0].arr[2].t.priority);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, tasks->level[0].arr[2].t.stack_size);
    TEST_ASSERT_EQUAL_size_t(3, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(3, tasks->level[0].en);
    #endif

    //test allocating maximum amount of tasks at priority level 0
    myrtos_reset();
    tasks = myrtos_get_task_queue();
    sp = myrtos_get_stack_bp();
    printf("\n");
    for (size_t i = 0; i < MYRTOS_MAX_TASKS; i++)
    {
        printf("Register Iteration: [%d/%d]\n", i, MYRTOS_MAX_TASKS - 1);
        ret = myrtos_register_task_i(&t1);
        sprintf(str, myrtos_debug_print(ret));
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
        TEST_ASSERT_EQUAL_STRING_MESSAGE("t1", tasks->level[0].arr[i].t.name,                           "Name not matching within max allocate loop");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, tasks->level[0].arr[i].t.priority,                           "Priority not matching within max allocate loop");
        TEST_ASSERT_EQUAL_size_t_MESSAGE(MYRTOS_MIN_STACK_SIZE, tasks->level[0].arr[i].t.stack_size,    "Stack Size not matching within max allocate loop");
        TEST_ASSERT_EQUAL_PTR_MESSAGE(sp + MYRTOS_MIN_STACK_SIZE*(i+1), tasks->level[0].arr[i].sp,      "Task PTR within max allocate loop didn't match");
    }
    //ensure that memory at bounds does not exceed allocated stack block
    TEST_ASSERT_LESS_THAN_size_t_MESSAGE((size_t)myrtos_get_heap_bp(), (size_t)sp + MYRTOS_MIN_STACK_SIZE*MYRTOS_MAX_TASKS, "End of allocated stack block not pointing to start of heap");
    TEST_ASSERT_LESS_THAN_size_t_MESSAGE((size_t)myrtos_get_heap_bp(), (size_t)tasks->level[0].arr[MYRTOS_MAX_TASKS-1].sp,  "Stack pointer for last task does not point to correct location");

    //test allocating another task when full (testing different priority level as well for non Round Robin)
    uint8_t* sp_ui = (uint8_t*)sp;
    *sp_ui = 0xff;
    ret = myrtos_register_task_i(&t2);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_TASK_LIMIT_REACHED, ret, str);

    TEST_ASSERT_EQUAL_size_t(MYRTOS_MAX_TASKS, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 1; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif
    TEST_ASSERT_EQUAL_size_t(0, tasks->blocked.len);

    //ensure stack not modified
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0xff, *sp_ui, "Stack modfiied when allocating on full task");
    //ensure heap header not modified
    myrtos_block_header_s** headers = malloc(sizeof(myrtos_block_header_s*) * 30);
    size_t n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, n, "Number of headers returned on empty heap incorrect");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(headers[0], myrtos_get_heap_bp(), "First header address does not match starting address of heap");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0,headers[0]->size, "Top header size not zero with no allocations");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(NULL, headers[1]->next, "End header next value is not NULL");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[1]->size, "End header size not zero");

    //test task removal
    //peek, shouldn't modify
    myRTOS_int_task_type_s t_i;
    ret = myrtos_peek_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t1", t_i.t.name);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, t_i.t.stack_size);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MAX_TASKS, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 1; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif
    TEST_ASSERT_EQUAL_size_t(0, tasks->blocked.len);

    //shouldn't be able to add another task
    ret = myrtos_register_task_i(&t2);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_TASK_LIMIT_REACHED, ret, str);

    TEST_ASSERT_EQUAL_size_t(MYRTOS_MAX_TASKS, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 1; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif
    TEST_ASSERT_EQUAL_size_t(0, tasks->blocked.len);

    //test removal
    ret = myrtos_request_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t1", t_i.t.name);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, t_i.t.stack_size);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MAX_TASKS-1, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 1; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif
    TEST_ASSERT_EQUAL_size_t(0, tasks->blocked.len);

    //shouldn't be able to register another task still
    ret = myrtos_register_task_i(&t2);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_TASK_LIMIT_REACHED, ret, str);

    TEST_ASSERT_EQUAL_size_t(MYRTOS_MAX_TASKS-1, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 1; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif
    TEST_ASSERT_EQUAL_size_t(0, tasks->blocked.len);

    //test adding task again, should succeed (for non-round robin testing at different priority)
    t_i.t = t2;
    ret = myrtos_push_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);

    #ifdef MYRTOS_ROUND_ROBIN
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MAX_TASKS, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].en);
    #else
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MAX_TASKS-1, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    for (uint8_t i = 1; i < MYRTOS_QUEUE_ARR_LEN-1; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].en);
    #endif
    TEST_ASSERT_EQUAL_size_t(0, tasks->blocked.len);

    //test removing all the tasks
    #ifdef MYRTOS_ROUND_ROBIN
    printf("\n");
    for( size_t i = 0; i < MYRTOS_MAX_TASKS-1; i++)
    {
        printf("Remove Iteration: [%d/%d]\n", i, MYRTOS_MAX_TASKS - 2);

        ret = myrtos_request_task(&t_i);
        sprintf(str, myrtos_debug_print(ret));
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
        TEST_ASSERT_EQUAL_STRING("t1", t_i.t.name);
        TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, t_i.t.stack_size);
        TEST_ASSERT_EQUAL_size_t(MYRTOS_MAX_TASKS-(i+1), tasks->level[0].len);
        TEST_ASSERT_EQUAL_size_t((i+2 == MYRTOS_MAX_TASKS) ? 0 : i+2, tasks->level[0].st);
        TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].en);
    }
    ret = myrtos_request_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t2", t_i.t.name);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, t_i.t.stack_size);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].en);
    #else
    printf("\n");
    for( size_t i = 0; i < MYRTOS_MAX_TASKS-1; i++)
    {
        printf("Remove Iteration: [%d/%d]\n", i, MYRTOS_MAX_TASKS - 2);

        ret = myrtos_request_task(&t_i);
        sprintf(str, myrtos_debug_print(ret));
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
        TEST_ASSERT_EQUAL_STRING("t1", t_i.t.name);
        TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, t_i.t.stack_size);
        TEST_ASSERT_EQUAL_size_t(MYRTOS_MAX_TASKS-(i+2), tasks->level[0].len);
        TEST_ASSERT_EQUAL_size_t((i+2 == MYRTOS_MAX_TASKS) ? 0 : i+2, tasks->level[0].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    }
    ret = myrtos_request_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t2", t_i.t.name);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, t_i.t.stack_size);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].len);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].en);
    #endif
    //should still be unable to register a task
    ret = myrtos_register_task_i(&t2);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_TASK_LIMIT_REACHED, ret, str);

    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN-1; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].len);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].en);
    #endif
    TEST_ASSERT_EQUAL_size_t(0, tasks->blocked.len);

    //should not be able to remove another task
    ret = myrtos_request_task(&t_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_TASK_QUEUE_EMPTY, ret, str);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN-1; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].len);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].st);
    TEST_ASSERT_EQUAL_size_t(1, tasks->level[MYRTOS_QUEUE_ARR_LEN-1].en);
    #endif
    TEST_ASSERT_EQUAL_size_t(0, tasks->blocked.len);


    //test blocked list
    ret = myrtos_reset();
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    myRTOS_int_task_type_s t1_i = {.t = t1};
    myRTOS_int_task_type_s t2_i = {.t = t2};
    myRTOS_int_task_type_s t3_i = {.t = t3};
    myRTOS_int_task_type_s t4_i = {.t = t4};
    myRTOS_task_queue_s* blocked = myrtos_get_blocked_list_ptr();
    ret = myrtos_push_blocked_task(&t1_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t1", blocked->arr[0].t.name);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, blocked->arr[0].t.stack_size);
    TEST_ASSERT_EQUAL_UINT8(0, blocked->arr[0].t.priority);
    TEST_ASSERT_EQUAL_size_t(1, blocked->len);

    //nothing else should be modified
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif

    //test adding a few tasks
    ret = myrtos_push_blocked_task(&t2_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t2", blocked->arr[1].t.name);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE, blocked->arr[1].t.stack_size);
    TEST_ASSERT_EQUAL_UINT8(MYRTOS_QUEUE_ARR_LEN-1, blocked->arr[1].t.priority);
    TEST_ASSERT_EQUAL_size_t(2, blocked->len);
    ret = myrtos_push_blocked_task(&t3_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t3", blocked->arr[2].t.name);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE*6, blocked->arr[2].t.stack_size);
    TEST_ASSERT_EQUAL_size_t(3, blocked->len);
    ret = myrtos_push_blocked_task(&t4_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t4", blocked->arr[3].t.name);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MIN_STACK_SIZE*6, blocked->arr[3].t.stack_size);
    TEST_ASSERT_EQUAL_size_t(4, blocked->len);

    //nothing else should be modified
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif

    //remove from invalid index
    ret = myrtos_rem_blocked_task(&t_i, 4);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_FAIL, ret, str);
    TEST_ASSERT_EQUAL_size_t(4, blocked->len);
    //nothing else should be modified
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif
    ret = myrtos_rem_blocked_task(&t_i, 7);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_FAIL, ret, str);
    TEST_ASSERT_EQUAL_size_t(4, blocked->len);
    //nothing else should be modified
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif

    //remove from middle
    ret = myrtos_rem_blocked_task(&t_i, 1);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t2", t_i.t.name);
    TEST_ASSERT_EQUAL_STRING("t1", blocked->arr[0].t.name);
    TEST_ASSERT_EQUAL_STRING("t3", blocked->arr[1].t.name);
    TEST_ASSERT_EQUAL_STRING("t4", blocked->arr[2].t.name);
    TEST_ASSERT_EQUAL_size_t(3, blocked->len);
    //nothing else should be modified
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif

    //remove from end
    ret = myrtos_rem_blocked_task(&t_i, 2);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t4", t_i.t.name);
    TEST_ASSERT_EQUAL_STRING("t1", blocked->arr[0].t.name);
    TEST_ASSERT_EQUAL_STRING("t3", blocked->arr[1].t.name);
    TEST_ASSERT_EQUAL_size_t(2, blocked->len);
    //nothing else should be modified
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif

    //remove from start
    ret = myrtos_rem_blocked_task(&t_i, 0);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t1", t_i.t.name);
    TEST_ASSERT_EQUAL_STRING("t3", blocked->arr[0].t.name);
    TEST_ASSERT_EQUAL_size_t(1, blocked->len);
    //nothing else should be modified
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif

    //remove last
    ret = myrtos_rem_blocked_task(&t_i, 0);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
    TEST_ASSERT_EQUAL_STRING("t3", t_i.t.name);
    TEST_ASSERT_EQUAL_size_t(0, blocked->len);
    //nothing else should be modified
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif

    //test filling blocked list
    printf("\n");
    for (size_t i = 0; i < MYRTOS_MAX_TASKS; i++)
    {
        printf("Add Blocked Iteration: [%d/%d]\n", i, MYRTOS_MAX_TASKS - 1);

        ret = myrtos_push_blocked_task(&t1_i);
        sprintf(str, myrtos_debug_print(ret));
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
        TEST_ASSERT_EQUAL_STRING("t1", blocked->arr[i].t.name);
        TEST_ASSERT_EQUAL_size_t(i+1, blocked->len);
    }
    //nothing else should be modified
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif

    //shouldn't be able to add anything to blocked list
    ret = myrtos_push_blocked_task(&t1_i);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_TASK_LIMIT_REACHED, ret, str);
    TEST_ASSERT_EQUAL_size_t(MYRTOS_MAX_TASKS, blocked->len);
    //nothing else should be modified
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif

    //remove all from blocked list
    printf("\n");
    for (size_t i = 0; i < MYRTOS_MAX_TASKS; i++)
    {
        printf("Remove Blocked Iteration: [%d/%d]\n", i, MYRTOS_MAX_TASKS - 1);

        ret = myrtos_rem_blocked_task(&t_i, 0);
        sprintf(str, myrtos_debug_print(ret));
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_SUCCESS, ret, str);
        TEST_ASSERT_EQUAL_STRING("t1", t_i.t.name);
        TEST_ASSERT_EQUAL_size_t(MYRTOS_MAX_TASKS - i - 1, blocked->len);
    }
    //nothing else should be modified
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif

    //should not be able to remove another task
    ret = myrtos_rem_blocked_task(&t_i, 0);
    sprintf(str, myrtos_debug_print(ret));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MYRTOS_FAIL, ret, str);
    TEST_ASSERT_EQUAL_size_t(0, blocked->len);
    //nothing else should be modified
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].len);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].st);
    TEST_ASSERT_EQUAL_size_t(0, tasks->level[0].en);
    #ifndef MYRTOS_ROUND_ROBIN
    for (uint8_t i = 0; i < MYRTOS_QUEUE_ARR_LEN; i++)
    {
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].len);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].st);
        TEST_ASSERT_EQUAL_size_t(0, tasks->level[i].en);
    }
    #endif
}