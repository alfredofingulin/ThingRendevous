/** \file
*   \brief ThingRendevous's Task Private interface, customized for ARM Cortex M4 
*
* Private types known only to Task and trTi code generator Trtgen.
*
* \author Alfred O Fingulin FinguPublic@gmail.com
*/
/*
* MIT License
* 
* Copyright (c) 2018, 2017 Alfred O Fingulin  FinguPublic@gmail.com
* 
* Permission is granted, free of charge, to any person obtaining a copy
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

#ifndef trTP_h
#define trTP_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/** Task Data Block; there is no such thing.
*/

/** But there is a Task Stack Block; really just an array of bytes.
*
* - Located in .bss.
* - An array of these forms a task's thread.
* - Trtgen names this trTi_Task_SB_SomeName_ID_Idx[ WhateverTheLengthIs ].
*/

/** Task Control Block
*
* - Located in .bss.
* - one exists for every task, period.
* - Trtgen names this trTi_Task_CB_SomeName_ID_Idx
*/
struct trTP_CB_s {
    uint16_t cb_task_state;      /**< As defined in trTS.h */
    int16_t cb_priority_offset;      /**< Any offset to the nominal priority.  0 means none; task is at nominal priority.  Negative values indicate the task's priority has been raised by this amount (remember: lower number means higher priority).  Positive values should never occur. */
    uint16_t cb_task_delay_remaining;      /**< If the task is blocked by another task and a timeout is pending, how long remains.  Always in units of system clock ticks.  trTi_TaskTimeout_pe will be on the trSR_TimeoutTasks_pl.  Otherwise undefined. */
    void *cb_aKIDS_p;      /**< If the task is in service or suspended on a resource then this points to the kernel request.  Undefined otherwise.  */
    uint8_t *cb_sp;  /**< If the task isn't running, here's where the PSP is. */
    uint64_t cb_r4_11_save[4];  /**< 32 bytes aligned on 8-byte boundary for ARM "unsaved" registers.  Use trTP_R4_11_*Save_get_p to get this.  */
};

/** Task Config Info
*
* - Located in .txt
* - one exists for every task.  Every task, dammit.
* - Trtgen names this trTi_Task_CI_SomeName_ID_Idx.
*/
struct trTP_CI_s {
    struct trTP_CB_s *const ci_cb_p;       /**< Points to the task control block in RAM. */
    void (*const ci_someTask) (void *ci_theTaskData_p);           /**< The task function itself. */
    void *const ci_theTaskData_p;       /**< Points to task parameters expected in RAM. */
    uint8_t *const ci_sb_p;       /**< Points to the stack block byte array in RAM. */
    const uint32_t ci_StackSize;      /**< Number of bytes possible. */
    const uint16_t ci_NominalPriority;      /**< The nominal priority at which the task runs.  Remember: Low values indicate high priority and high values indicate low priority.  That is: priority 17 runs before priority 177. */
};

bool trTP_getp(uint16_t forTask_idx, struct trTP_CI_s **ci_pp, struct trTP_CB_s **cb_pp, uint8_t ** sb_pp);

/** Return a pointer to the ARM R4..R11 save area; forTask_idx better be good or else! 
*/
#define trTP_R4_11_Save_get_p(forTask_idx) ((uint8_t *)(&((trTi_TaskInfoList[forTask_idx].ib_t_p)->ci_cb_p)->cb_r4_11_save[0]))

/** All registers not saved during an interrupt or exception.
*/
struct trTP_R4_11_Save_s {
    uint32_t r4;      /**< aka v1 */
    uint32_t r5;      /**< aka v2 */
    uint32_t r6;      /**< aka v3 */
    uint32_t r7;      /**< aka v4 */
    uint32_t r8;      /**< aka v5 */
    uint32_t r9;      /**< aka v6/rfp */
    uint32_t r10;      /**< aka sl */
    uint32_t r11;      /**< aka fp */
};

/** Return a pointer to the ARM R4..R11 _structured_ save area. forTask_idx blah blah blah.
*/
#define trTP_R4_11_StrucSave_get_p(forTask_idx) ((struct trTP_R4_11_Save_s *)(&((trTi_TaskInfoList[forTask_idx].ib_t_p)->ci_cb_p)->cb_r4_11_save[0]))

/** Here's what's stacked during a interrupt or exception.
*/
struct trTP_ExcepStkFrm_s {
    uint32_t r0;      /**< aka a1 scratch */
    uint32_t r1;      /**< aka a2 scratch */
    uint32_t r2;      /**< aka a3 scratch */
    uint32_t r3;      /**< aka a4 scratch */
    uint32_t r12;      /**< aka ip temporary */
    uint32_t r14_lr;      /**< r14 link register */
    uint32_t r15_pc;      /**< program counter */
    uint32_t xPSR;      /**< program status register */
};

#endif
