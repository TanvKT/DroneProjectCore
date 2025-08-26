/**
 * @file myRTOS_heap.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-08-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __MYRTOS_HEAP_H__
#define __MYRTOS_HEAP_H__

#include <stdint.h>

/**
 * @brief Local struct for heap header type
 * 
 * Total size 16 bytes, using multiples of 8 for each block
 *
 */
typedef struct MYRTOS_BLOCK_HEADER {
    size_t size;                            //(4 bytes) when size is 0, the block is free
    struct MYRTOS_BLOCK_HEADER* next;       //(4 bytes) pointer to next header, if next is NULL, then we are on end block
    struct MYRTOS_BLOCK_HEADER* prev;       //(4 bytes) pointer to previous header, if prev is NULL, then we are on top block
} myrtos_block_header_s;

#define MYRTOS_HEADER_SIZE 16      //we define header size as 16 bytes to keep our memory aligned to 8 byte blocks

myRTOS_return_type_e myrtos_zeroize_heap();
size_t myrtos_list_heap(myrtos_block_header_s** arr, size_t n);

#endif