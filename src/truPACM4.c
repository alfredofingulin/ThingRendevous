/** \file
*   \brief ThingRendevous's customized ARM Cortex m4 Context Handlers 
*
* Doxygen and Indent don't cope too well with this source code.
*
* This module implements the rendezvous scheme described in
* \link uDN06_RendEI \endlink , 
* \link uDN07_RendTI \endlink ,
* and \link uDN08_TskCntxt \endlink
* for mutual exclusion and actual context switching.
*
* Most of that is transitioning from/to ready/running states.
* Much is hardware intimate and is properly done here.
*
* \author Alfred O Fingulin FinguPublic@gmail.com
*/
/*
* MIT License
* 
* Copyright (c) 2018, 2017 Alfred O Fingulin  FinguPublic@gmail.com
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
// no string
#include "trKIDS.h"
#include "trSR.h"
#include "trTi.h"
#include "trM.h"
#include "trTS.h"
#include "trTstc.h"
#include "trTP.h"
#include "trPl.h"
#include "trKhf.h"
#include "trAPIRet.h"

// No point in including our own header file.

/** Does the real-work to handle SVC.
*
* The input is a pointer to where to find the r0...et al register stack.
* What is in each register determins the activity.
* Results, if any, are posted to r0.
*
* Further: It returns a 64 bit value.
* That comes back in r1:r0.
* It's really a pointer to a r4...r11 save/restore area.
*
* Parameterized r0 : action code
*
*     If r0 is trKIDS_TI_Enter_srq
*
*         Either effect a switch to the Tti task or tell caller to invoke directly.
*
*         Parameterized r1 : points to a trKIDS_*_rq KIDS structure
*
*         Parameterized r2, r3 : ignored
*
*         On completion, parameterized r0 is modified :
*
*             < 0 : undefined
*             == 0 : Tti was used via a context switch.
*                    This task resumes execution on the next instruction.
*             > 0 : call Tti direct
*
*         On completion, parameterized r1 : undefined.
*
*         On completion, parameterized r2, r3 : nulled.
*
*         Return r1 : Where to save r4...r11.
*
*             null : don't save
*             non-null : where to save
*
*         Return r0 : Where to restore r4...r11.
*
*             null : don't restore
*             non-null : where to find the values to restore
*
*     If r0 is trKIDS_TI_Leave_srq
*
*         The Tti task is complete.
*         Oddly, it's always a direct return to the current or next task.
*
*         Parameterized r1 : points to a trKIDS_*_rq KIDS structure
*
*         Parameterized r2, r3 : ignored
*
*         On completion, parameterized r0 is modified : always 0.
*
*         On completion, parameterized r1 : undefined.
*
*         On completion, parameterized r2, r3 : nulled.
*
*         Return r1 : Where to save r4...r11.
*
*             null : don't save
*             non-null : where to save
*
*         Return r0 : Where to restore r4...r11.
*
*             null : don't restore
*             non-null : where to find the values to restore
*
*     If r0 is trKIDS_EI_Enter_srq
*
*         An interrupt has started.  Maybe save current context for restart.
*
*         Parameterized r1, r2, r3 : ignored
*
*         On completion, parameterized r0 is modified : always 0.
*
*         On completion, parameterized r1 : undefined.
*
*         On completion, parameterized r2 : nulled.
*
*         On completion, parameterized r3 : Where to save r4...r11.
*
*             null : don't save
*             non-null : where to save
*
*         Return r1, r0 : nulled.
*
*     If r0 is trKIDS_EI_Leave_srq
*
*         An interrupt has ended.  Maybe restore context for restart.
*
*         Parameterized r1, r2, r3 : ignored
*
*         On completion, parameterized r0 is modified : always 0.
*
*         On completion, parameterized r1 : undefined.
*
*         On completion, parameterized r2 : Where to restore r4...r11.
*
*             null : don't save
*             non-null : where to save
*
*         On completion, parameterized r3 : nulled.
*
*         Return r1, r0 : nulled.
*
*     If r0 is trKIDS_Private_Section_srq
*
*         A private section (aka critical section) used internally.
*
*         Parameterized r1 : First parameter for the private section function.
*
*         Parameterized r2 : points to a function taking two void pointers as paramters
*             This function will be called to handle the private section.
*
*         Parameterized r3 : Second parameter for the private section function.
*
*         On completion, parameterized r0 is modified : always 0.
*
*         On completion, parameterized r1 : undefined.
*
*         On completion, parameterized r2, r3 : nulled.
*
*         Return r1, r0 : nulled.
*
*/
uint64_t truPACM4_SVC_Handler(uint32_t * rX_p  /**< Pointer to registers r0...3 */
    )
{
    uint32_t result = 0;

    uint32_t EI_restorer4r11_r2 = 0;
    uint32_t EI_saver4r11_r3 = 0;

    uint32_t TI_restorer4r11_r0 = 0;
    uint32_t TI_saver4r11_r1 = 0;

    void *aKIDS_p = (void *)rX_p[1];
    void (*the_private_section_hdlr) (void *usually_aKIDS_p, void *more_stuff_p)
        = (void (*)(void *, void *))rX_p[2];
    void *actual_more_stuff_p = (void *)rX_p[3];

    struct trTP_CI_s *ci_p;
    struct trTP_CB_s *cb_p;
    uint8_t *sb_p;

    uint32_t tmp32;

    uint16_t nexttask_idx;

    if (!trSR_Flags_going()
        && (aKIDS_p != NULL)
        && (trKIDS_ComParams_aKIDS_p_AntiAlias().cp_request == trKIDS_Start_rq)
        && ((rX_p[0] & 0x0000FFFFUL) == trKIDS_TI_Leave_srq)) {
        /**
        * ~~ Check for the special case of system start.
        *
        * IF the go flag is not going, KIDS is not null, and it's a start, tap the go flag!
        *
        * Order matters, because after this, we're going right into leave execution.
        */
        trSR_Flags |= trSR_Flags_Go_bit;
    }

    if (trSR_Flags_going()) {
        /**
        * ~~ If we're going, take a look at r0 and do something useful.
        */
        switch (rX_p[0] & 0x0000FFFFUL) {

        case trKIDS_TI_Enter_srq:
            /**
            * ~~ Entering the Tr Task Itself.
            *
            * Assume we've screen out the run-on-caller case.
            * Either tell 'em to call Tti direct or switch context to Tti.
            */
            if ((trSR_ExIntrNest > 0) || (trSR_TrItselfNest > 0)) {

                /**
                * IF we're in explicit interrupt context
                * or re-entering Tr itself, we're already in
                * the context we need to be in.
                * 
                * - silently tag us as up for service
                * - increment Tr nesting
                * - rendevous direct.
                *
                * By definition we cannot be any other task.
                * No need to save/restore context, change stacks; 
                * muck with PSP, r4..11 et al.
                */

                trTstc_beginInServ(aKIDS_p);
                ++trSR_TrItselfNest;
                result = 1;     // non-zero is call direct

            } else {

                /** 
                * ELSE we're not in Tr itself or explicit interrupt context
                * and need to get into Tr itself context.
                */

                /**
                * First, remember the KIDS and stack for
                * the present task.  
                *
                * - KIDS will also be starting task data for Tr Itself.
                * - PSP has the task context
                * - the running task ARM specifics need to be saved too
                */
                trTi_TaskInfoList[trSR_RunningTask_idx].ib_t_p->ci_cb_p->cb_aKIDS_p = aKIDS_p;

                __asm__ __volatile__(
                    "mrs    r0, psp \n\t"
                    "str    r0, %0 \n\t"
                    : "=m"(tmp32) 
                    :
                    : "r0"
                    );
                trTi_TaskInfoList[trSR_RunningTask_idx].ib_t_p->ci_cb_p->cb_sp = (uint8_t *) tmp32;
                TI_saver4r11_r1 = (uint32_t) trTP_R4_11_StrucSave_get_p(trSR_RunningTask_idx);

                /**
                * Next:
                *
                * - set up Tr itself for launch, just like you were launching for the first time
                * - pass this KIDS to the Tr itself task
                * - set up the registers and psp for launch
                */
                trTP_getp(trTi_Tti_idx, &ci_p, &cb_p, &sb_p);

                cb_p->cb_sp = &(sb_p[ci_p->ci_StackSize - ((uint32_t) (sizeof(struct trTP_ExcepStkFrm_s)))]);
                ((struct trTP_ExcepStkFrm_s *)(cb_p->cb_sp))->r0 = (uint32_t) aKIDS_p;  // pass in KIDS for Tti to work on
                ((struct trTP_ExcepStkFrm_s *)(cb_p->cb_sp))->r14_lr = (uint32_t) ci_p->ci_someTask;
                ((struct trTP_ExcepStkFrm_s *)(cb_p->cb_sp))->r15_pc = ((uint32_t) ci_p->ci_someTask) & 0xFFFFFFFEUL;
                __asm__ __volatile__(
                    "mrs    r0, xpsr \n\t"
                    "str    r0, %0 \n\t"
                    : "=m"(tmp32) 
                    :
                    : "r0"
                    );
                ((struct trTP_ExcepStkFrm_s *)(cb_p->cb_sp))->xPSR = (tmp32 & 0xF9FF0200UL) | 0x01000000UL;      // ensure no IT block, in-thread, thumb bit set
    
                trTP_R4_11_StrucSave_get_p(trTi_Tti_idx)->r7 
                    = tmp32
                    = (uint32_t) cb_p->cb_sp;

                TI_restorer4r11_r0 = (uint32_t) trTP_R4_11_StrucSave_get_p(trTi_Tti_idx);
                __asm__ __volatile__(
                    "ldr    r0, %0 \n\t"
                    "msr    psp, r0 \n\t" 
                    "isb \n\t"
                    :
                    : "m"(tmp32) 
                    : "r0"
                    );

                /**
                * Finally:
                *
                * - Tag present request for service
                * - make Tti the running task
                * - nesting will go to 1
                * - Context switch to Tr itself.
                */
                trTstc_beginInServ(aKIDS_p);
                ++trSR_TrItselfNest;

                trTS_TaskState_set(trTi_TaskInfoList[trTi_Tti_idx].ib_t_p->ci_cb_p->cb_task_state,
                                   trTS_TaskState_Running);
                trSR_RunningTask_idx = trTi_Tti_idx;

                // result remains 0; gonna switch

            }
            break;

        case trKIDS_TI_Leave_srq:
            /**
            * ~~ Leaving the Tr Task Itself
            *
            * Assume the run-on-call case has been screened-out.
            * Called only from Tti; either return or context switch.
            */
            if (((trSR_ExIntrNest >= 1) && (trSR_TrItselfNest >= 1))
                || ((trSR_ExIntrNest == 0) && (trSR_TrItselfNest > 1))) {

                /**
                * IF we're unwinding nested explicit interrupts
                * or tr itself requests,
                * then stay in the present context.
                *
                * - End in service
                * - Decrement the tk nesting
                * - don't change PSP or r4..11; just return
                */

                --trSR_TrItselfNest;
                trTstc_endInServ(aKIDS_p);

            } else {
                /**
                * ELSE we're not still ex intr and we're leaving tr itself context.
                *
                * - is the request don't pre-empt?  If so, goin' back to him
                * - otherwise, pick the highest priority task
                */
                if ((trKIDS_ComParams_aKIDS_p_AntiAlias().cp_kernel_func_hndlr_flags & trKhf_Flags_DontPreEmpt) ==
                    trKhf_Flags_DontPreEmpt) {
                    nexttask_idx = trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx;
                } else {
                    nexttask_idx = trPl_Hi_remove(&trTi_TaskReadyResrc_pe[0], &trSR_ReadyTasks_pl);
                }

                /**
                * We're definately leaving the present task, Tti, in the dust.
                * Don't bother saving Tti stuff.
                * Let's set up for the next task
                */
                TI_restorer4r11_r0 = (uint32_t) trTP_R4_11_StrucSave_get_p(nexttask_idx);
                tmp32 = (uint32_t) ((trTi_TaskInfoList[nexttask_idx].ib_t_p)->ci_cb_p)->cb_sp;
                __asm__ __volatile__(
                    "ldr    r0, %0 \n\t"
                    "msr    psp, r0 \n\t" 
                    "isb \n\t"
                    :
                    : "m"(tmp32) 
                    : "r0"
                    );
                
                /**
                * - set the Tti task state to programmatic buzzword what-not
                * - end the currently serviced request
                * - decr nesting; should be zero now
                * - make next task running
                */
                trTS_TaskState_set(((trTi_TaskInfoList[trTi_Tti_idx].ib_t_p)->ci_cb_p)->cb_task_state,
                                   trTS_TaskState_AwaitingProgrammaticActivation);
                --trSR_TrItselfNest;
                trTstc_endInServ(aKIDS_p);

                trTS_TaskState_set(((trTi_TaskInfoList[nexttask_idx].ib_t_p)->ci_cb_p)->cb_task_state,
                                   trTS_TaskState_Running);
                trSR_RunningTask_idx = nexttask_idx;

                /**
                * We'll return to the next task.
                */
            }
            break;

        case trKIDS_EI_Enter_srq:
            /**
            * ~~ Entering an explicit interrupt.
            */
            /**
            * Per the explicit interrupt rendevous tech note,
            * just count explicit interrupt nesting.
            */

            if (trSR_ExIntrNest++ == 0) {
                /**
                * On the first time thru
                *
                * - we don't care about the KIDS
                * - it's always from the PSP to the MSP, so get the PSP
                * - move the task to the ready list; subsequent tr API calls may suspend; leave will pick a new, maybe different, task
                * - make the interrupt task the running task
                */

                EI_saver4r11_r3 = (uint32_t) trTP_R4_11_StrucSave_get_p(trSR_RunningTask_idx);
                __asm__ __volatile__(
                    "mrs    r0, psp \n\t"
                    "str    r0, %0 \n\t"
                    : "=m"(tmp32) 
                    :
                    : "r0"
                    );
                trTi_TaskInfoList[trSR_RunningTask_idx].ib_t_p->ci_cb_p->cb_sp = (uint8_t *) tmp32;

                trTS_TaskState_set(trTi_TaskInfoList[trSR_RunningTask_idx].ib_t_p->ci_cb_p->cb_task_state,
                                   trTS_TaskState_Ready);
                trPl_insert(trSR_RunningTask_idx, trTi_TaskReadyResrc_pe, &trSR_ReadyTasks_pl);

                trTS_TaskState_set(trTi_TaskInfoList[trTi_Tei_idx].ib_t_p->ci_cb_p->cb_task_state,
                                   trTS_TaskState_Running);
                trSR_RunningTask_idx = trTi_Tei_idx;

            }

            /**
            * Since ARM Cortex M4 has already switched to the main stack,
            * just return and continue on the main stack.
            * Any context switch will be handled by leave.
            */

            /**
            * Since we've never mucked with anything,
            * we'll simply return 
            * to the interrupt already in progress.
            */
            break;

        case trKIDS_EI_Leave_srq:
            /**
            * ~~ Leave an explicit interrupt.
            */
            /**
            * Per the explicit interrupt rendevous tech note,
            * just count explicit interrupt nesting.
            */

            if (trSR_ExIntrNest-- == 1) {
                /**
                * On the last leave thru
                *
                * - hibertate the interrupt task
                * - pick the highest priority task
                * - make it the running task
                * - make it's stack the process stack.
                *
                * Previous context is already on that task's stack.
                */
                trTS_TaskState_set(((trTi_TaskInfoList[trTi_Tei_idx].ib_t_p)->ci_cb_p)->cb_task_state,
                                   trTS_TaskState_AwaitingProgrammaticActivation);

                nexttask_idx = trPl_Hi_remove(&trTi_TaskReadyResrc_pe[0], &trSR_ReadyTasks_pl);

                EI_restorer4r11_r2 = (uint32_t) trTP_R4_11_StrucSave_get_p(nexttask_idx);
                tmp32 = (uint32_t) ((trTi_TaskInfoList[nexttask_idx].ib_t_p)->ci_cb_p)->cb_sp;
                __asm__ __volatile__(
                    "ldr    r0, %0 \n\t"
                    "msr    psp, r0 \n\t" 
                    "isb \n\t"
                    :
                    : "m"(tmp32) 
                    : "r0"
                    );
                
                trTS_TaskState_set( ((trTi_TaskInfoList[nexttask_idx].ib_t_p)->ci_cb_p)->cb_task_state,
                    trTS_TaskState_Running);
                trSR_RunningTask_idx = nexttask_idx;

            }
                /**
                * Implied else is: 
                * Just return and continue on the main stack.
                * Main stack should be right where it was when this
                * whole thing started.
                */

            /**
            * Since we've never mucked with anything,
            * we'll simply return 
            * to the interrupt or task already in progress.
            */
            break;

        case trKIDS_Private_Section_srq:
            (*the_private_section_hdlr) (aKIDS_p, actual_more_stuff_p);
            break;

        default:
            ;
            break;
        }
    } else {
        /**
        * ~~ Else if we're not going, don't do anything useful.
        * Call direct, do nothing with interrupts, allow private sections for testing.
        */
        switch (rX_p[0] & 0x0000FFFFUL) {

        case trKIDS_TI_Enter_srq:
            result = 1;         // any non-zero is call direct
            break;

        case trKIDS_Private_Section_srq:
            (*the_private_section_hdlr) (aKIDS_p, actual_more_stuff_p);
            break;

        case trKIDS_TI_Leave_srq:
        case trKIDS_EI_Enter_srq:
        case trKIDS_EI_Leave_srq:
        default:
            ;
            break;
        }
    }

    /**
    * ~~ Finally, return everything we need to return; ensure it's written to memory.
    */
    rX_p[3] = EI_saver4r11_r3;
    rX_p[2] = EI_restorer4r11_r2;
    rX_p[0] = result;
    __asm__ __volatile__(
        "dsb \n\t"
        : 
        :
        : 
        );
    return (((uint64_t) TI_saver4r11_r1) << 32) | ((uint64_t) TI_restorer4r11_r0);
}

/** The SVC exception handler target.  
*
* So why is SVC_Handler here and doing what it's doing?
* Because:
*
* - \_\_attribute\_\_((naked)) is too little
* - \_\_attribute\_\_((interrupt)) is necessary to fix-up sp due to possible non-alignment
* - you can't really do both
* - this exception can happen from both Main and Process stacks
* - PendSV exceptions expect a pointer to the stack in r0 to find parameters
* - it could be either stack and we need get there before prolog/epilog confuses everything.
*
* Almost like ARM App Note 179B, but GNU isn't helping us.
*
* So, we do it here, in assembler, 
* and then call the actual handler. 
*
* The following prolog, 
* modified from ARM \_\_attribute\_\_((interrupt)) standard,
* is implemented:
*
* - adjust stack pointer to 8-byte alignment
* - this leaves the original sp in r0, adjusted in r1
* - move adjusted sp in r1 into the sp
* - push r0, r7, lr
* - move sp into r7 to establish stack frame
*
* At this point, anything called should follow normal register conventions.
*
* Before calling the actual handler:
*
* - test to see which stack is used, MSP or PSP, by looking at the link register
* - if it's MSP, r1 has what we need
* - if is's PSP, get it directly from PSP.
* - whichever, get that to r0.
*
* R0 points to the saved stacked registers, where the params/rets are in r0...3.
*
* On return, R0 is where to save r4...r11, stacked r3 is where to restore r4...r11.
* But we need to clobber R0, so R0 is moved to R1.
*/
void __attribute__ ((naked)) SVC_Handler(void)
{
    __asm__ __volatile__(
        "mov    r0, sp \n\t"
        "bic    r1, r0, #7 \n\t"
        "mov    sp, r1 \n\t"
    "\n\t"
        "push   {r0, r7, lr} \n\t"
        "add    r7, sp, #0 \n\t"
    "\n\t"
        "tst    lr, #4 \n\t"
        "ite    eq \n\t"
        "moveq  r0, r1 \n\t"
        "mrsne  r0, psp \n\t"
    "\n\t"
        "bl     truPACM4_SVC_Handler \n\t"
    "\n\t"
        "mov    r3, r1 \n\t"
        "mov    r2, r0 \n\t"
    "\n\t"
        "mov    sp, r7 \n\t"
        "pop    {r0, r7, lr} \n\t"
        "mov    sp, r0 \n\t"
    "\n\t"
        "cmp    r3, #0 \n\t"
        "it     ne \n\t"
        "stmne  r3, {r4-r11} \n\t"
    "\n\t"
        "cmp    r2, #0 \n\t"
        "it     ne \n\t"
        "ldmne  r2, {r4-r11} \n\t"
    "\n\t"
        "bx lr \n\t"
        :
        :
        :
        );
}

/** Does the real-work to handle PendSV.
*
* \todo Not implemented.
*
*/
void truPACM4_PendSV_Handler(uint32_t * rX_p  /**< Pointer to registers r0...3 */
    )
{

}

/** The PendSV exception target.  Works just like SVC_Handler (which see).
*
* \todo Not implemented.
*
*/
void __attribute__ ((naked)) PendSV_Handler(void)
{
    __asm__ __volatile__(
        "mov    r0, sp \n\t"
        "bic    r1, r0, #7 \n\t"
        "mov    sp, r1 \n\t"
    "\n\t"
        "push   {r0, r7, lr} \n\t"
        "add    r7, sp, #0 \n\t"
    "\n\t"
        "tst    lr, #4 \n\t"
        "ite    eq \n\t"
        "moveq  r0, r1 \n\t"
        "mrsne  r0, psp \n\t"
    "\n\t"
        "bl     truPACM4_PendSV_Handler \n\t"
    "\n\t"
        "mov    sp, r7 \n\t"
        "pop    {r0, r7, lr} \n\t"
        "mov    sp, r0 \n\t"
    "\n\t"
        "bx lr \n\t"
        :
        :
        :
        );
}

/** This stupid init is necessary to ensure weak overrides will occur.
*/
void truPACM4_init(void)
{
    ;
}
