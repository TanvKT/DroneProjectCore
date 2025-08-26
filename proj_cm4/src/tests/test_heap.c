/**
 * @file test_heap.c
 * @author your name (you@domain.com)
 * @brief Heap testing for myRTOS
 * @version 0.1
 * @date 2025-06-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "myRTOS.h"
#include "tests.h"
#include "unity.h"
#include "myRTOS_heap.h"
#include "myRTOS_memory.h"

static void confirm_bounds(myrtos_block_header_s** h, size_t n)
{
    TEST_ASSERT_EQUAL_PTR_MESSAGE(h[0], myrtos_get_heap_bp(), "First header address does not match starting address of heap");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(NULL, h[n-1]->next, "End header next value is not NULL");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, h[n-1]->size, "End header size not zero");
}

static void write_test_string(char* s, size_t n)
{
    for (int i = 0; i < n; i++)
    {
        switch(i % 8)
        {
            case 0:
                s[i] = 'd';
                break;
            case 1:
                s[i] = 'e';
                break;
            case 2:
                s[i] = 'a';
                break;
            case 3:
                s[i] = 'd';
                break;
            case 4:
                s[i] = 'b';
                break;
            case 5:
                s[i] = 'e';
                break;
            case 6:
                s[i] = 'e';
                break;
            case 7:
                s[i] = 'f';
                break;
            default:
                s[i] = 'd';
        }
    }
}

void heap_test()
{
    myrtos_block_header_s** headers = malloc(sizeof(myrtos_block_header_s*) * 30);

    /* TESTING ALLOCATE */
    //nothing should be allocated at this point
    size_t n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, n, "Number of headers returned on empty heap incorrect");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(headers[0], myrtos_get_heap_bp(), "First header address does not match starting address of heap");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0,headers[0]->size, "Top header size not zero with no allocations");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(NULL, headers[1]->next, "End header next value is not NULL");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[1]->size, "End header size not zero");

    //allocate with invalid size
    void* ptr_invalid = myrtos_alloc(0);
    TEST_ASSERT_EQUAL_PTR_MESSAGE(NULL, ptr_invalid, "Invalid alloc request with size 0 returned non NULL pointer");
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, n, "Number of headers returned after alloc request with size 0 incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[0]->size, "Size of top header not 0 bytes after alloc with size 0");

    //allocate one 8 byte block
    void* ptr8 = myrtos_alloc(8);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(3, n, "Number of headers returned after 8 byte alloc incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8, headers[0]->size, "Size of top header not 8 bytes");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[1]->size, "Size of second header block not 0!");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8 + MYRTOS_HEADER_SIZE, (size_t)headers[1] - (size_t)headers[0], "Actual allocated size not 8 bytes");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)headers[0] + MYRTOS_HEADER_SIZE, ptr8, "Returned PTR8 not pointing to correct address");

    //allocate a 16 byte block
    void* ptr16 = myrtos_alloc(16);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(4, n, "Number of headers returned after 16 byte alloc incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8, headers[0]->size, "Size of top header not 8 bytes"); //need to ensure first alloc did not get changed
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[2]->size, "Size of third header block not 0");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8 + MYRTOS_HEADER_SIZE, (size_t)headers[1] - (size_t)headers[0], "Previous allocated size not 8 bytes");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)headers[0] + MYRTOS_HEADER_SIZE, ptr8, "Returned PTR8 not pointing to correct address");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(16, headers[1]->size, "Size of second header not 16 bytes");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(16 + MYRTOS_HEADER_SIZE, (size_t)headers[2] - (size_t)headers[1], "Actual allocated size not 16 bytes");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)headers[1] + MYRTOS_HEADER_SIZE, ptr16, "Returned PTR16 not pointing to correct address");

    //allocate a small, unaligned block
    void* ptr13 = myrtos_alloc(13);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(5, n, "Number of headers returned after 13 byte alloc incorrect");
    confirm_bounds(headers, n);
    //no longer going super in depth with testing previous allocs
    TEST_ASSERT_EQUAL_size_t_MESSAGE(16, headers[2]->size, "Size of 13 byte alloc header not 16 bytes");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(16 + MYRTOS_HEADER_SIZE, (size_t)headers[3] - (size_t)headers[2], "Actual allocated size not 16 bytes");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)headers[2] + MYRTOS_HEADER_SIZE, ptr13, "Returned PTR13 not pointing to correct address");

    //allocate a smaller, unaligned block
    void* ptr5 = myrtos_alloc(5);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(6, n, "Number of headers returned after 5 byte alloc incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8, headers[3]->size, "Size of 5 byte alloc header not 8 bytes");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8 + MYRTOS_HEADER_SIZE, (size_t)headers[4] - (size_t)headers[3], "Actual allocated size not 16 bytes");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)headers[3] + MYRTOS_HEADER_SIZE, ptr5, "Returned PTR5 not pointing to correct address");

    //allocate a single very large block
    void* ptr1024 = myrtos_alloc(1024);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(7, n, "Number of headers returned after 1024 byte alloc incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(1024, headers[4]->size, "Size of 1024 byte alloc header not 8 bytes");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(1024 + MYRTOS_HEADER_SIZE, (size_t)headers[5] - (size_t)headers[4], "Actual allocated size not 1024 bytes");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)headers[4] + MYRTOS_HEADER_SIZE, ptr1024, "Returned PTR1024 not pointing to correct address");

    /* TESTING FREE */
    //test freeing large block at end
    myrtos_free(ptr1024);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(6, n, "Number of headers returned after 1024 byte free incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[4]->size, "Size of 1024 byte alloc header not 0 bytes");

    //test freeing a block in between allocated blocks
    void* ptr32 = myrtos_alloc(32);
    void* ptr50 = myrtos_alloc(50);
    void* ptr3 = myrtos_alloc(3);
    void* ptr75 = myrtos_alloc(75);
    myrtos_free(ptr3);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(10, n, "Number of headers returned after 3 byte free incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[6]->size, "Size of 3 byte alloc header not 0 bytes");

    //test free consolidation
    myrtos_free(ptr50);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(9, n, "Number of headers returned after 50 byte free incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[5]->size, "Size of 50 byte alloc header not 0 bytes");
    myrtos_free(ptr75);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(7, n, "Number of headers returned after 75 byte free incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[5]->size, "Size of 75 byte alloc header not 0 bytes");
    myrtos_free(ptr32);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(6, n, "Number of headers returned after 32 byte free incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[4]->size, "Size of 32 byte alloc header not 0 bytes");
    
    /* TESTING REALLOCATE */
    //reallocate invalid size
    ptr_invalid = myrtos_realloc(ptr8, 0);
    TEST_ASSERT_EQUAL_PTR_MESSAGE(NULL, ptr_invalid, "Invalid realloc request with size 0 returned non NULL pointer");
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(6, n, "Number of headers returned after realloc request with size 0 incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8, headers[0]->size, "Size of top header not 8 bytes after realloc with size 0");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8 + MYRTOS_HEADER_SIZE, (size_t)headers[1] - (size_t)headers[0], "Actual allocated size not 8 bytes after realloc with size 0");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)headers[0] + MYRTOS_HEADER_SIZE, ptr8, "Returned PTR8 not pointing to correct address after realloc with size 0");

    //reallocate with invalid pointer
    ptr_invalid = myrtos_realloc(ptr8 + 3, 8);
    TEST_ASSERT_EQUAL_PTR_MESSAGE(NULL, ptr_invalid, "Invalid realloc request with invalid pointer returned non NULL pointer");
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(6, n, "Number of headers returned after realloc request with invalid pointer incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8, headers[0]->size, "Size of top header not 8 bytes after realloc with invalid pointer");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8 + MYRTOS_HEADER_SIZE, (size_t)headers[1] - (size_t)headers[0], "Actual allocated size not 8 bytes after realloc with invalid pointer");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)headers[0] + MYRTOS_HEADER_SIZE, ptr8, "Returned PTR8 not pointing to correct address after realloc with invalid pointer");

    //reallocate with same size
    ptr_invalid = myrtos_realloc(ptr8, 8);
    TEST_ASSERT_EQUAL_PTR_MESSAGE(ptr8, ptr_invalid, "Realloc on pointer with request of same size returned different pointer value");
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(6, n, "Number of headers returned after realloc request with same size incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8, headers[0]->size, "Size of top header not 8 bytes after realloc with same size");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8 + MYRTOS_HEADER_SIZE, (size_t)headers[1] - (size_t)headers[0], "Actual allocated size not 8 bytes after realloc with same size");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)headers[0] + MYRTOS_HEADER_SIZE, ptr8, "Returned PTR8 not pointing to correct address after realloc with same size");

    //reallocate small to large
    write_test_string((char*)ptr8, 8);
    ptr8 = myrtos_realloc(ptr8, 16);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(7, n, "Number of headers returned after 8 byte to 16 byte realloc incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[0]->size, "Size first header not 0");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(16, headers[4]->size, "Size of 16 byte realloc not 16");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(16 + MYRTOS_HEADER_SIZE, (size_t)headers[5] - (size_t)headers[4], "Actual allocated size not 16 bytes");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)headers[4] + MYRTOS_HEADER_SIZE, ptr8, "Returned PTR8 not pointing to correct address");
    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("deadbeef", (char*)ptr8, 8, "8 byte to 16 byte realloc data not maintained");

    //reallocate large to small
    write_test_string((char*)ptr13, 13);
    ptr13 = myrtos_realloc(ptr13, 8);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(7, n, "Number of headers returned after 13 byte to 8 byte realloc incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8, headers[0]->size, "Size of 13 byte to 8 byte realloc not 8");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(8 + MYRTOS_HEADER_SIZE, (size_t)headers[1] - (size_t)headers[0], "Actual allocated size not 8 bytes");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)headers[0] + MYRTOS_HEADER_SIZE, ptr13, "Returned PTR13 not pointing to correct address");
    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("deadbeef", (char*)ptr13, 8, "13 byte to 8 byte realloc data not maintained");
    TEST_ASSERT_NOT_EQUAL_CHAR_MESSAGE('d', ((char*)ptr13)[8], "Data copied over outside of realloc bounds");
    TEST_ASSERT_NOT_EQUAL_CHAR_MESSAGE('a', ((char*)ptr13)[10], "Data copied over outside of realloc bounds");
    TEST_ASSERT_NOT_EQUAL_CHAR_MESSAGE('b', ((char*)ptr13)[12], "Data copied over outside of realloc bounds");

    //reallocate again
    write_test_string((char*)ptr16, 16);
    ptr16 = myrtos_realloc(ptr16, 24);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(7, n, "Number of headers returned after 16 byte to 24 byte realloc incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(24, headers[4]->size, "Size of 16 byte to 24 byte realloc not 24");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(24 + MYRTOS_HEADER_SIZE, (size_t)headers[5] - (size_t)headers[4], "Actual allocated size not 24 bytes");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)headers[4] + MYRTOS_HEADER_SIZE, ptr16, "Returned PTR16 not pointing to correct address");
    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("deadbeefdeadbeef", (char*)ptr16, 16, "16 byte to 24 byte realloc data not maintained");

    /* Edge Cases */
    //free all
    myrtos_free(ptr8);
    myrtos_free(ptr16);
    myrtos_free(ptr13);
    myrtos_free(ptr5);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, n, "Number of headers returned after free all incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[0]->size, "Size of top header not 0 after freeing all blocks");

    //allocate block too large
    void* ptr_too_large = myrtos_alloc(2*MYRTOS_HEAP_SIZE);
    TEST_ASSERT_EQUAL_PTR_MESSAGE(NULL, ptr_too_large, "Non-void return from invalid allocate");
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, n, "Number of headers returned after too large allocate incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[0]->size, "Size of top header not 0 after too large allocate");

    //allocate entire heap
    void* ptr_all = myrtos_alloc(MYRTOS_HEAP_SIZE - 2*MYRTOS_HEADER_SIZE);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, n, "Number of headers returned after full heap allocate incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(MYRTOS_HEAP_SIZE - 2*MYRTOS_HEADER_SIZE, headers[0]->size, "Size of top header not max after full allocate");
    TEST_ASSERT_EQUAL_PTR_MESSAGE((void*)headers[0] + MYRTOS_HEADER_SIZE, ptr_all, "Returned PTR_ALL not pointing to correct address");

    //attempt to allocate after heap full
    ptr8 = myrtos_alloc(8);
    TEST_ASSERT_EQUAL_PTR_MESSAGE(NULL, ptr8, "Non-void return from invalid allocate when heap full");
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, n, "Number of headers returned after attempt to allocate on full heap incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(MYRTOS_HEAP_SIZE - 2*MYRTOS_HEADER_SIZE, headers[0]->size, "Size of top header not full after attempt to allocate on full heap");

    //free entire heap again
    myrtos_free(ptr_all);
    n = myrtos_list_heap(headers, 30);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, n, "Number of headers returned after free full heap incorrect");
    confirm_bounds(headers, n);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, headers[0]->size, "Size of top header not 0 after freeing full heap blocks");

    free(headers);
}