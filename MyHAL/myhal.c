/**
 * @file myhal.c
 * @author Tanvin Thiagarajan (tthiagarajan@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "myhal.h"

/**
 * @brief A helper function to check if a given pointer is null
 * 
 * @param ptr pointer to check
 * @return true (1) if null
 * @return false (0) if non-null
 */
inline bool my_null_check(void* ptr)
{
    if (ptr == NULL)
        return true;
    else
        return false;
}

/**
 * @brief A helper function to assert on a condition
 * Polls infinitely if false
 * 
 * @param v value to check
 * @return void
 */
inline void my_assert(bool v)
{
    if (!v)
    {
        int i = 0;
        for (;;)
        {
            //do nothing...
            i++;  //sanity check to make sure compiler doesn't optimize
        }
    }
}