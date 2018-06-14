/** \file
*   \brief ThingRendevous's Trusted State Transition Coordinator implementation
*
* These move tasks from one state to another, 
* updating relevant data structures along the way.
*
* This does not perform safety checking; 
* assumes all necessary data structures and parameters are correct and valid.
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
// no trAPIRet.h
#include "trTi.h"
#include "trM.h"
#include "trTS.h"
#include "trTP.h"
#include "trPl.h"
#include "trSR.h"
#include "trKhf.h"
#include "truPACM4.h"
#include "trTstc.h"

/** Make the task suspended for resource reason supplied.
*
* If already suspended for a timeout, then this resource reason will be added.
*
* Should not be used on running tasks, or Tei/Tti.
* Can be used on all other in service/in flux, ready, or suspended tasks.
*
* Strange and Wonderous Things will happen if the suspend state is timeout.
*/
void trTstc_susp_Resc_PrivSect_hdlr(void *notused, void *susp_Resc_Info_s_p)
{
    if (trTS_TaskState_tst
        (trTi_TaskInfoList[trTstc_Susp_Resc_Info_p()->task_idx].ib_t_p->ci_cb_p->cb_task_state, trTS_TaskState_Ready)) {
        trPl_delete(trTstc_Susp_Resc_Info_p()->task_idx, trTi_TaskReadyResrc_pe, &trSR_ReadyTasks_pl);
    }
    trTS_Susp_set_st(trTi_TaskInfoList[trTstc_Susp_Resc_Info_p()->task_idx].ib_t_p->ci_cb_p->cb_task_state,
                     trTstc_Susp_Resc_Info_p()->task_susp_state);
    trTS_TaskState_set(trTi_TaskInfoList[trTstc_Susp_Resc_Info_p()->task_idx].ib_t_p->ci_cb_p->cb_task_state,
                       trTS_TaskState_Suspended);
}

/** Remove suspend for resource reason supplied, and if nothing else pending, make the task ready.
*
* If already suspended for a timeout, and the timeout continues, 
* then the task will not be made-ready.
*
* Should not be used on running tasks, or Tei/Tti.
* Can be used on all other in service/in flux, ready, or suspended tasks.
*
* Strange and Wonderous Things will happen if suspend state is a timeout.
*/
void trTstc_ready_Resc_PrivSect_hdlr(void *notused, void *ready_Resc_Info_s_p)
{
    trTS_Susp_clr_st(trTi_TaskInfoList[trTstc_Ready_Resc_Info_p()->task_idx].ib_t_p->ci_cb_p->cb_task_state,
                     trTstc_Ready_Resc_Info_p()->task_susp_state);
    if (!trTS_Susp_tst_any
        (trTi_TaskInfoList[trTstc_Ready_Resc_Info_p()->task_idx].ib_t_p->ci_cb_p->cb_task_state, trTS_Susp_bits)) {
        trPl_insert(trTstc_Ready_Resc_Info_p()->task_idx, trTi_TaskReadyResrc_pe, &trSR_ReadyTasks_pl);
        trTS_TaskState_set(trTi_TaskInfoList[trTstc_Ready_Resc_Info_p()->task_idx].ib_t_p->ci_cb_p->cb_task_state,
                           trTS_TaskState_Ready);
    }
}

/** Make the task suspended for a timeout with value supplied.
*
* If already suspended for a resource, then this timeout reason will be added.
*
* This will also safely override an existing timeout value.
*
* Should not be used on running tasks, or Tei/Tti.
* Can be used on all other in service/in flux, ready, or suspended tasks.
*/
void trTstc_susp_Timo_PrivSect_hdlr(void *notused, void *susp_Timo_Info_s_p)
{
    if (trTS_TaskState_tst
        (trTi_TaskInfoList[trTstc_Susp_Timo_Info_p()->task_idx].ib_t_p->ci_cb_p->cb_task_state, trTS_TaskState_Ready)) {
        trPl_delete(trTstc_Susp_Timo_Info_p()->task_idx, trTi_TaskReadyResrc_pe, &trSR_ReadyTasks_pl);
    } else
        if (trTS_Susp_tst
            (trTi_TaskInfoList[trTstc_Susp_Timo_Info_p()->task_idx].ib_t_p->ci_cb_p->cb_task_state, trTS_Susp_ForDelay)
            && (trTi_TaskInfoList[trTstc_Susp_Timo_Info_p()->task_idx].ib_t_p->ci_cb_p->cb_task_delay_remaining <
                0xFFFF)) {
        trPl_delete(trTstc_Susp_Timo_Info_p()->task_idx, trTi_TaskTimeout_pe, &trSR_TimeoutTasks_pl);
    }
    trTi_TaskInfoList[trTstc_Susp_Timo_Info_p()->task_idx].ib_t_p->ci_cb_p->cb_task_delay_remaining =
        trTstc_Susp_Timo_Info_p()->timo_val;
    if (trTi_TaskInfoList[trTstc_Susp_Timo_Info_p()->task_idx].ib_t_p->ci_cb_p->cb_task_delay_remaining != 0xFFFF) {
        trPl_insert(trTstc_Susp_Timo_Info_p()->task_idx, trTi_TaskTimeout_pe, &trSR_TimeoutTasks_pl);
    }
    trTS_Susp_set_st(trTi_TaskInfoList[trTstc_Susp_Timo_Info_p()->task_idx].ib_t_p->ci_cb_p->cb_task_state,
                     trTS_Susp_ForDelay);
    trTS_TaskState_set(trTi_TaskInfoList[trTstc_Susp_Timo_Info_p()->task_idx].ib_t_p->ci_cb_p->cb_task_state,
                       trTS_TaskState_Suspended);
}

/** Remove suspend for timeout, and if nothing else pending, make the task ready.
*
* This does NOT operate in a private section.
*
* This will cancel any remaining timeout, 
* but will not ignore a suspension for any other reason.
*
* Should not be used on running tasks, or Tei/Tti.
* Can be used on all other in service/in flux, ready, or suspended tasks.
*
* Strange and Wonderous Things may happen should the task not be timing-out.
*/
void trTstc_ready_Timo(uint16_t task_idx   /**< The index of the task suspended. */
    )
{
    if (trTi_TaskInfoList[task_idx].ib_t_p->ci_cb_p->cb_task_delay_remaining != 0xFFFF) {
        trPl_delete(task_idx, trTi_TaskTimeout_pe, &trSR_TimeoutTasks_pl);
    }
    trTS_Susp_clr_st(trTi_TaskInfoList[task_idx].ib_t_p->ci_cb_p->cb_task_state, trTS_Susp_ForDelay);
    if (!trTS_Susp_tst_any(trTi_TaskInfoList[task_idx].ib_t_p->ci_cb_p->cb_task_state, trTS_Susp_bits)) {
        trPl_insert(task_idx, trTi_TaskReadyResrc_pe, &trSR_ReadyTasks_pl);
        trTS_TaskState_set(trTi_TaskInfoList[task_idx].ib_t_p->ci_cb_p->cb_task_state, trTS_TaskState_Ready);
    }
}

/** A private section wrapper for trTstc_ready_Timo; yes, uses suspend info pointer!
*/
void trTstc_ready_Timo_PrivSect_hdlr(void *notused, void *susp_Timo_Info_s_p)
{
    trTstc_ready_Timo(trTstc_Susp_Timo_Info_p()->task_idx);
}

/** Make the requesting task in service and in-flux; but Tei/Tti just get in service and assumed to continue running.
*
* Is not a private section handler, but must be invoked within a private section.
*/
void trTstc_beginInServ(void *aKIDS_p /**< Kernel Interface Data Structure */ )
{
    uint16_t mytask_idx = trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx;
    if ((mytask_idx != trTi_Tti_idx) && (mytask_idx != trTi_Tei_idx)) {
        trTS_TaskState_set(trTi_TaskInfoList[mytask_idx].ib_t_p->ci_cb_p->cb_task_state, trTS_TaskState_InFlux);
    }
    trTS_TaskInServ_set_st(trTi_TaskInfoList[mytask_idx].ib_t_p->ci_cb_p->cb_task_state);
}

void trTstc_completedInServ_PrivSect_hdlr(void *notused, void *mytask_idx_p)
{
    if (trTS_TaskState_tst
        (trTi_TaskInfoList[*((uint16_t *) mytask_idx_p)].ib_t_p->ci_cb_p->cb_task_state, trTS_TaskState_InFlux)) {
        trPl_insert(*((uint16_t *) mytask_idx_p), trTi_TaskReadyResrc_pe, &trSR_ReadyTasks_pl);
        trTS_TaskState_set(trTi_TaskInfoList[*((uint16_t *) mytask_idx_p)].ib_t_p->ci_cb_p->cb_task_state,
                           trTS_TaskState_Ready);
    }
}

/** Possibly move a requesting task from in service task to the ready list.
*
* If the requesting task is Tti/Tei, then nothing is done.
* Let the context switcher figure it out.
*
* Otherwise, it all depends on:
*
* - whether or not is does not pre-empt ("dnp")
* - task state in-flux ("ifl"), ready ("rea"), or suspend ("sus").
*
* \code
*
*      | tsk ||
*  dnp | sta || outcome
* -----------++----
*   1  |  x  || task state should be ifl; rea or sus are errors; do nothing
*      |     || the context switcher will by pass ready list to run this task
*      |     ||
*   0  | ifl || service complete; put on ready list, advance to rea state
*      |     ||
*   0  | rea || already on a ready list; do nothing
*      |     ||
*   0  | sus || already on a suspend list; do nothing
*
* \endcode
*/
void trTstc_completedInServ(void *aKIDS_p /**< Kernel Interface Data Structure */ )
{
    uint16_t mytask_idx = trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx;
    if ((mytask_idx != trTi_Tti_idx) && (mytask_idx != trTi_Tei_idx)
        && ((trKIDS_ComParams_aKIDS_p_AntiAlias().cp_kernel_func_hndlr_flags & trKhf_Flags_DontPreEmpt) !=
            trKhf_Flags_DontPreEmpt)) {
        truPACM4_Private_Section(trTstc_completedInServ_PrivSect_hdlr, NULL, (void *)&mytask_idx);
    }
}

/** Clear the requesting task's in service flag; requesting task's state is assumed close-to-final.
*
* Is not a private section handler, but must be invoked within a private section.
*/
void trTstc_endInServ(void *aKIDS_p /**< Kernel Interface Data Structure */ )
{
    trTS_TaskInServ_clr_st(trTi_TaskInfoList[trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx].ib_t_p->
                           ci_cb_p->cb_task_state);
}

/** Load up everyting in trTS, trTi, using privates in trTP.
*
* Note Tei and Tti are removed from the priority list;
* it's always done "by hand" or thru hardware.
*/
void trTstc_init(void)
{
    uint16_t i;
    struct trTP_CI_s *ci_p;
    struct trTP_CB_s *cb_p;
    uint8_t *sb_p;

    trPl_init(&trTi_TaskTimeout_pe[0], trTi_NumOfTasks);
    trPl_init(&trTi_TaskReadyResrc_pe[0], trTi_NumOfTasks);

    for (i = 0; i < trTi_NumOfTasks; i++) {
        trTP_getp(i, &ci_p, &cb_p, &sb_p);
        trPl_Pri_set(i, ci_p->ci_NominalPriority, &trTi_TaskTimeout_pe[0]
            );
        trPl_Pri_set(i, ci_p->ci_NominalPriority, &trTi_TaskReadyResrc_pe[0]
            );
        if ((i == trTi_Tti_idx) || (i == trTi_Tei_idx)) {
            cb_p->cb_task_state = trTS_TaskState_AwaitingProgrammaticActivation;
        } else {
            trPl_insert(i, &trTi_TaskReadyResrc_pe[0], &trSR_ReadyTasks_pl);
            cb_p->cb_task_state = trTS_TaskState_Ready;
        }
    }

}
