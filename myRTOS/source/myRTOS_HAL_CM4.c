/**
 * @file myRTOS_HAL_CM4.c
 * @author your name (you@domain.com)
 * @brief Defines HAL abstracted functions for CM4 based architecture
 * @version 0.1
 * @date 2025-09-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 //ARM CM4 specific implementation
#include "myRTOS_config.h"
#include "myRTOS_HAL.h"
#include "myRTOS_sched.h"

#ifdef MYRTOS_ARM_CM4
#include "core_cm4.h"

/**
 * @brief IMPORTANT: This define needs to be implememnted for every architecture
 * 
 */
#define LOW_INT_PRIORITY (1UL << __NVIC_PRIO_BITS) - 1

int myrtos_hal_schedule_init(myRTOS_int_task_type_vp t)
{
    NVIC_SetPriority(PendSV_IRQn, LOW_INT_PRIORITY);
    __set_PSP((uint32_t)t->sp);

    //switch to use PSP in Thread mode (set CONTROL.SPSEL = 1)
    uint32_t ctrl = __get_CONTROL();
    ctrl |=  (1U << 1);
    __set_CONTROL(ctrl);

    // Enable CP10/CP11 (FPU) in CPACR
    SCB->CPACR |= (0xF << 20);  // full access to CP10 and CP11
    __DSB();

    //ensure the change to CONTROL is visible immediately
    __ISB();
    return 1;
}
int myrtos_hal_stack_setup(myRTOS_int_task_type_vp t)
{
    //modify the stack with desired values, these are the values the CPU expects to pop when starting task
    uint32_t* sp = (uint32_t*)t->sp;
    *(--sp) = (1U << 24);                   // xPSR: Thumb bit set
    *(--sp) = (uint32_t)t->t.handle;        // PC: task entry point
    *(--sp) = 0xFFFFFFFDUL;                 // LR: EXC_RETURN → return to Thread mode, use PSP
    *(--sp) = 0;                            // R12
    *(--sp) = 0;                            // R3
    *(--sp) = 0;                            // R2
    *(--sp) = (uint32_t)t->t.args;          // R1 task arguments
    *(--sp) = 0;                            // R0
    //r4-r11
    for (int i = 0; i < 8; ++i) {
        *(--sp) = 0;
    }
    return 1;
}

/**
 * @brief The following code was generated using AI assistance
 * 
 */
__attribute__((naked)) void PendSV_Handler(void)
{
    __asm volatile (
        /* --- Save context of the task being preempted --- */
        "MRS   r0, PSP                 \n" // r0 = PSP (process stack pointer)

        /* Save high FP regs s16-s31 onto task stack (conservative) */
        "VSTMDB r0!, {s16-s31}         \n" // store s16..s31, decremented

        /* Save callee-saved core registers r4-r11 onto task stack */
        "STMDB r0!, {r4-r11}           \n" // store r4..r11

        /* Store updated PSP into current_tcb->stack_ptr */
        "LDR   r1, =s_curr_task_p      \n" // r1 = &current_tcb (address of global pointer)
        "LDR   r2, [r1]                \n" // r2 = current_tcb (pointer to TCB)
        "STR   r0, [r2]                \n" // current_tcb->stack_ptr = r0

        /* --- Call scheduler (must preserve EXC_RETURN in LR) --- */
        "PUSH  {lr}                    \n" // save EXC_RETURN (in LR)
        "BL    myrtos_schedule         \n" // call C scheduler: updates current_tcb
        "POP   {lr}                    \n" // restore EXC_RETURN

        /* --- Restore context of the next task --- */
        "LDR   r1, =s_curr_task_p      \n" // r1 = &current_tcb
        "LDR   r2, [r1]                \n" // r2 = current_tcb
        "LDR   r0, [r2]                \n" // r0 = new task's saved PSP

        /* Restore core callee-saved regs r4-r11 */
        "LDMIA r0!, {r4-r11}           \n" // pop r4..r11

        /* Restore FP regs s16-s31 */
        "VLDMIA r0!, {s16-s31}         \n" // pop s16..s31

        /* Write PSP with the incremented value */
        "MSR   PSP, r0                 \n" // PSP = new SP

        /* Return from exception (EXC_RETURN value in LR tells hw to pop r0..r3,r12,lr,pc,xPSR) */
        "BX    lr                      \n"
    );
}
void myrtos_hal_set_hardware_timer_flag(void)
{
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}
void myrtos_hal_enable_interrupts(void)
{
    __enable_irq();
}
void myrtos_hal_disable_interrupts(void)
{
    __disable_irq();
}
uint8_t myrtos_hal_get_lowest_priority(void)
{
    return LOW_INT_PRIORITY;
}
uint8_t myrtos_hal_atomic_read_write(volatile uint8_t* v)
{
    return __STREXB(1, v);
}
#endif