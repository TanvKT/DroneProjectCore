/**
 * @file myhal.h
 * @author Tanvin Thiagarajan (tthiagarajan@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */
/**********************************************************
 *                  MYHAL INCLUDES                        *
 *********************************************************/
#ifndef __MYHAL_H__
#define __MYHAL_H__

#include "stdint.h"
#include "stdbool.h"

//some helper functions
inline bool my_null_check(void* ptr);
inline void my_assert(bool v);

//gpio includes
#include "GPIO.h"

#endif