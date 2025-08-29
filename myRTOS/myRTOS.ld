/*
*   myRTOS.ld
*   Optional (highly recommended) include script
*       for reserving memory regions for myRTOS functionality
*   If not included will default to using heap memory
*   If not included heap allocation needs to be large enough for functionality or else errors will occur
*   To include add linker include after .bss section of project linker script
*/

/*
*   Allocate memory for task array
*       This is where information about each task will be stored
*       If priority is involved in scheduling this becomes a priority queue
*/
.myrtos_task_stacks (NOLOAD) :
{
    . = ALIGN(4);
    KEEP(*(.myrtos_tasks))
    . = ALIGN(4);
} > ram

/* 
*   Allocate memory for individual task stacks 
*       This is formatted as a contiguous block of memory with no safety checks
*       It is up to the user to not exceed total stack allocation
*       Modifications to the total stack size can be made in myRTOS_config.h
*/
.myrtos_task_stacks (NOLOAD) :
{
    . = ALIGN(4);
    KEEP(*(.myrtos_stacks))
    . = ALIGN(4);
} > ram

/*
*   Allocate memory for user heap
*       This block of memory will be managed by myRTOS heap manager
        This is a thread safe alternative to malloc
*/
.myrtos_heap (NOLOAD) :
{
    . = ALIGN(4);
    KEEP(*(.myrtos_heap))
    . = ALIGN(4);
} > ram