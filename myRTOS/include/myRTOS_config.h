/**
 * @file myRTOS_config.h
 * @author Tanvin Thiagarajan (tthigarajan@wisc.edu)
 * @brief Config defines for myRTOS
 * @version 0.1
 * @date 2025-08-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #ifndef __MYRTOS_CONFIG_H__
 #define __MYRTOS_CONGIF_H__

 #include "myRTOS_sched.h"

 /**
 * @brief MyRTOS config MACROS
 * 
 */

/* MEMORY */
#define     MYRTOS_USE_LINKER                                           //IMPORTANT : comment line this out if not using linker

#define     MYRTOS_STACK_SIZE          (128 * 1024)                     //default allocate 128KB stack for tasks
#define     MYRTOS_MIN_STACK_SIZE      (4 * 1024)                       //default minimum stack size for tasks 4KB
#define     MYRTOS_HEAP_SIZE           (64 * 1024)                      //default heap size 64KB

/* SCHEDULING */
#define     MYRTOS_MAX_TASKS           (MYRTOS_STACK_SIZE / (MYRTOS_MIN_STACK_SIZE  + sizeof(myRTOS_int_task_type_s))) - 1    //max possible tasks used for memory allocation of task info array

#define     MYRTOS_ROUND_ROBIN                                          //ROUND_ROBIN       -       Every task is allotted equal CPU time
                                                                        //PRIORITY_BASED    -       Tasks of higher priority will get more CPU time
                                                                        //                              If two tasks have the same priority, round robin is used
                                                                        //DYNAMIC_PRIORITY  -       Tasks taking larger chunks of slice will have their priority lowered

#define     MYRTOS_PRIORITY_LEVELS     7                                //default to 7 levels of priority (0 - 6) anything larger is truncated down
                                                                        //          (NOT USED IF ROUND_ROBIN)

#endif