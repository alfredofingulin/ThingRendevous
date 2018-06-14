/** \file
*   \brief ThingRendevous's Start implementation, customized for ARM Cortex M4 
*
* Here begins ThingRendevous's execution.
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
#include "trAPIRet.h"
#include "trTi.h"
#include "trM.h"
#include "trTS.h"
#include "trTstc.h"
#include "trTP.h"
#include "trPl.h"

#include "trSR.h"
#include "trSClk.h"
#include "trEvt.h"
#include "trMsg.h"
#include "trStrm.h"
#include "trT.h"
#include "trTti.h"
#include "trTei.h"

#include "truPACM4.h"

#include "trStart.h"

/** Something to call, in the Tti stack context.  That way, we can define automatics and truly begin kernel start.
*
*/
void trStart_sidestepStart(void)
{
    /**
    * Create a start KIDS to satisfy Tr Task Itself's leave parameter requirement.
    */
    struct trKIDS_Start_s my_rq;
    my_rq.cp.cp_request = trKIDS_Start_rq;
    my_rq.cp.cp_ret_val = trAPIRet_Gen_InProcess;
    my_rq.cp.cp_my_task_idx = trSR_RunningTask_idx;
    my_rq.cp.cp_destination_target_task = trTi_Tti_ID;
    my_rq.cp.cp_kernel_func_hndlr = NULL;
    my_rq.cp.cp_kernel_func_hndlr_flags = 0;

    /**
    * Now enable interrupts w/ instruction boundary and trap to the start.
    */
    __asm__ __volatile__(
        "cpsie  i \n\t"
        "isb \n\t"
        :
        :
        :
        );

    /**
    * We've faked a Tr Task Itself.
    * Do a kernel leave with a start request and that'll tap the go flag and pick the next task to launch.
    */
    trTti_leave(&my_rq);
    while (1) {
    }
}

/** Begins the whole queiscent init chain for tr.
*
* Order matters.  Pauses to do uP-specific stuff at key points.
*
* Must be running on the main stack pointer, not the PSP.
*
* Detailed in the code below.
*/
void trStart_start(void)
{
    uint16_t i;
    struct trTP_CI_s *ci_p;
    struct trTP_CB_s *cb_p;
    uint8_t *sb_p;
    uint32_t ttistack_p;
    uint32_t xpsrinit;

    /**
    * Disable interrupts, instruction boundary.
    */
    __asm__ __volatile__(
        "cpsid  i \n\t"
        "isb \n\t"
        :
        :
        :
        );

    /**
    * Part of the usual Tr inits.
    */
    truPACM4_init();
    trSR_init();
    trSClk_init();
    trEvt_init();
    trMsg_init();
    trStrm_init();
    trT_init();

    /**
    * Pause to initialize task control blocks and uP-specific register content.
    * 
    * Tei is skipped; it inherits the main stack pointer; so no real space
    * was allocated for it.  Don't blindly init it; 'cause you'll overrun
    * adjacent data!
    *
    * And remember visiting Tti, 'cause we want to 
    * use that stack to run on later.
    */
    __asm__ __volatile__(
        "mrs    r0, xpsr \n\t"
        "str    r0, %0 \n\t"
        : "=m"(xpsrinit) 
        :
        : "r0"
        );
    xpsrinit = (xpsrinit & 0xF9FF0200UL) | 0x01000000UL;      // ensure no IT block, in-thread, thumb bit set
    for (i = 0; i < trTi_NumOfTasks; i++) {
        if (i != trTi_Tei_idx) {
            trTP_getp(i, &ci_p, &cb_p, &sb_p);

            cb_p->cb_sp = &(sb_p[ci_p->ci_StackSize - ((uint32_t) (sizeof(struct trTP_ExcepStkFrm_s)))]);

            if (i == trTi_Tti_idx) {
                ttistack_p = (uint32_t) cb_p->cb_sp;
            }

            ((struct trTP_ExcepStkFrm_s *)(cb_p->cb_sp))->r0 = (uint32_t) ci_p->ci_theTaskData_p;
            ((struct trTP_ExcepStkFrm_s *)(cb_p->cb_sp))->r14_lr = (uint32_t) ci_p->ci_someTask;
            ((struct trTP_ExcepStkFrm_s *)(cb_p->cb_sp))->r15_pc = ((uint32_t) ci_p->ci_someTask) & 0xFFFFFFFEUL;
            ((struct trTP_ExcepStkFrm_s *)(cb_p->cb_sp))->xPSR = xpsrinit;

            trTP_R4_11_StrucSave_get_p(i)->r7 = (uint32_t) cb_p->cb_sp; // present stack frame

        }
    }

    /**
    * Finish init-ing Tr internals.
    */
    trTti_init();
    trTei_init();
    trTstc_init();

    /**
    * Mimic Tti.  
    * Have this set up before enabling interrupts, or the go flag, or the stack!
    */
    trSR_RunningTask_idx = trTi_Tti_idx;
    trSR_TrItselfNest = 1;

    /**
    * Set PSP.
    * Insruction boundary.
    * Switch to the PSP; steal the stack space from Tti to run on for now.
    * Insruction boundary.
    * At this point, all our automatics are toast.
    */
    __asm__ __volatile__(
        "ldr    r0, %0 \n\t"
        "msr    psp, r0 \n\t" 
        "isb \n\t"
        "mrs    r0, control \n\t"
        "orr    r1, r0, #2 \n\t"
        "msr    control, r1 \n\t"
        "isb \n\t"
        :
        : "m"(ttistack_p) 
        : "r0", "r1"
        );
    
    /**
    * To set up and use new automatics, sidestep to another procedure;
    * that'll create a new stack frame on the psp.
    */
    trStart_sidestepStart();

}
