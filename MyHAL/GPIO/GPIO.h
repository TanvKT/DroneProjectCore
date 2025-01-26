/**
 * @file GPIO.h
 * @author Tanvin Thiagarajan (tthiagarajan@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include "stdint.h"
#include "stdbool.h"

/*  Global Defs     */

/*  Type Defs     */
/* gpio drive strength */
typedef enum {
    GPIO_STR_STRONG,
    GPIO_STR_WEAK
} my_GPIO_strength;
/* gpio drive type */
typedef enum {
    GPIO_DRV_Z,
    GPIO_DRV_IN,
    GPIO_DRV_OUT,
    GPIO_DRV_INOUT
} my_GPIO_drive;

/* config type */
typedef struct {
    uint32_t pin;
    my_GPIO_strength strength;
    my_GPIO_drive drive;
    bool state;
} my_GPIO_config;

/* gpio type for function */
typedef struct {
    my_GPIO_config config;
} my_GPIO_t;

/*  Function Defs   */
void my_GPIO_init(my_GPIO_t* o, my_GPIO_config* c);
bool my_GPIO_write(my_GPIO_t* o);
bool my_GPIO_read(my_GPIO_t* o);

#endif