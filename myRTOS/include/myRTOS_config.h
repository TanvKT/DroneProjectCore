/**
 * @file myRTOS_config.h
 * @author Tanvin Thiagarajan (tthigarajan@wisc.edu)
 * @brief Config defines for myRTOS
 * 
 *          IMPORTANT -- AS OF NOW --
 *              To include myRTOS in a project the sources and inlcude flags must be modified to search for
 *              myRTOS code, it should look something like the following:
 *
 *              SOURCES=$(wildcard ../myRTOS/source/ *.c) $(UNITY_SRC)
 *              INCLUDES=../myRTOS/include ../shlib/unity
 *
 * 
 * @version 0.1
 * @date 2025-08-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #ifndef __MYRTOS_CONFIG_H__
 #define __MYRTOS_CONFIG_H__

 /**
 * @brief MyRTOS config MACROS
 * 
 */

/************************************************************************************************************************************************************
*    @brief     NOTE: These defines allow myRTOS to be implemented on different devices, change accordingly                                                 *                                                                      
************************************************************************************************************************************************************/
#define     MYRTOS_USE_CYHAL                                            //This define tells myRTOS to use CYHAL function for hardware abstraction
#define     MYRTOS_ARM_CM4                                              //This define tells myRTOS to use ARM CM4 architecture
/***********************************************************************************************************************************************************/

/* DEVICE */
#define     MYRTOS_UART_BAUD           115200
#define     MYRTOS_UART_BUFF_SIZE      2048

/* TASK TIMER */
#define     MYRTOS_TASK_TIMER_PERIOD   999                              //Here we assume a 1MHz clock
#define     MYRTOS_TASK_TIMER_FREQ     1000000u                         //These numbers result in a 1ms period

/* MEMORY */
//#define     MYRTOS_USE_LINKER                                           //IMPORTANT : comment line this out if not using linker
                                                                        //  Part of the setup for this library involves including the linker file myRTOS.ld
                                                                        //      within the linker script for each project after the .bss sections (INCLUDE myRTOS.ld)
                                                                        //  The LDFLAGS tag in the Makefile must also be modified to LDFLAGS+=-L{path_to_myRTOS}
                                                                        //      assuming myRTOS lives in a directory on the same level as the overall project
                                                                        //      this would be LDFLAGS+=-L../myRTOS

#define     MYRTOS_STACK_SIZE          (128 * 1024)                     //default allocate 128KB stack for tasks
#define     MYRTOS_MIN_STACK_SIZE      (4 * 1024)                       //default minimum stack size for tasks 4KB
#define     MYRTOS_HEAP_SIZE           (64 * 1024)                      //default heap size 64KB
#define     MYRTOS_LOCK_HEAP_SIZE      (2 * 1024)                       //default lock heap size 2KB

/* SCHEDULING */
#define     MYRTOS_TASK_NAME_LEN       64                               //max length of task names

// Important note, only one of these can be uncommented at once
// Failure to comply will result in unforseen consequences

//#define     MYRTOS_ROUND_ROBIN         1                                   //MYRTOS_ROUND_ROBIN       -       Every task is allotted equal CPU time
//#define     MYRTOS_PRIORITY_BASED      1                                   //MYRTOS_PRIORITY_BASED    -       Tasks of higher priority will get more CPU time
                                                                           //                                      If two tasks have the same priority, round robin is used
#define     MYRTOS_DYNAMIC_PRIORITY    1                                   //MYRTOS_DYNAMIC_PRIORITY  -       Tasks taking larger chunks of slice will have their priority lowered

#define     MYRTOS_PRIORITY_LEVELS     7                                //default to 7 levels of priority (0 - 6) anything larger is truncated down
                                                                        //          (NOT USED IF ROUND_ROBIN)

#ifdef      MYRTOS_DYNAMIC_PRIORITY
#define     MYRTOS_PRIO_LOWER_THRESH   3                                //defines how many times a task can use full time slice before being demoted
#endif

/* MISC */
#define     MYRTOS_DEBUG_MODE          1                                //0 - debug prints disabled
                                                                        //1 - debug prints enabled

#endif