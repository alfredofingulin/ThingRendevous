/** \file
*   \brief ThingRendevous's Event implementation
*
* Here are the KIDS function handlers.
* Relies on externs populated in trTi by generation from Trtgen.
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
#include "truPACM4.h"
#include "trEvtP.h"
#include "trEvt.h"

/** This private grab is no different from the public one, but the API status is returned and not filled-in at the KIDs pointer.
*
* Tries to get an event from the calling task's event queue; successful or not.
*
* \retval trAPIRet_OK trAPIRet_Evt_Nothing2Grb trAPIRet_Usr_CallNotValid
*/
uint16_t trEvt_privGrb(void *aKIDS_p)
{
    struct trEvtP_GrabInfo_s me;
    me.ret_val = trAPIRet_Gen_InProcess;

    if (trEvtP_getp(trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx, &me.ci_p, &me.cb_p, &me.db_p, &me.pl_p)) {
        truPACM4_Private_Section(trEvtP_grab_PrivSect_hdlr, aKIDS_p, (void *)&me);
    } else {
        me.ret_val = trAPIRet_Usr_CallNotValid;
    }

    return me.ret_val;
}

/** This cross-over grab is no different from the private one, but the target index is assumed good and the API status is filled-in at the KIDs pointer.
*
* Use this when you know the post went-off
* and you need to prime the target 
* when a grab/pend is... well... pending.
*
* \retval true - a cross was executed; make this task ready
* \retval false - no crossover; no need to make ready
*/
bool trEvt_xoverGrb(uint16_t target_task_idx
                                  /**< Presumed valid task index.  Always has task info; by assumption, has event info. */
    ) {
    void *aKIDS_p = ((struct trTP_CB_s *)trTi_TaskInfoList[target_task_idx].ib_t_p->ci_cb_p)->cb_aKIDS_p;
    struct trEvtP_XoverGrabInfo_s me;
    me.ret_val = false;
    me.cb_p = (struct trEvtP_CB_s *)trTi_TaskInfoList[target_task_idx].ib_evt_p->ci_cb_p;
    me.db_p = trTi_TaskInfoList[target_task_idx].ib_evt_p->ci_db_p;
    me.pl_p = trTi_TaskInfoList[target_task_idx].ib_evt_p->ci_pl_p;
    me.target_task_idx = target_task_idx;

    truPACM4_Private_Section(trEvtP_xoverGrab_PrivSect_hdlr, aKIDS_p, (void *)&me);

    return me.ret_val;
}

/** This private stuff is no different from the public one, but the API status is returned and not filled-in at the KIDs pointer.
*
* Note you can stuff events to any task; so the destination is assumed good.
*
* \retval trAPIRet_OK trAPIRet_Strm_NoRoom2PostOrStf trAPIRet_Usr_CallNotValid
*/
uint16_t trEvt_privStf(void *aKIDS_p,
                        /**< Obvious. */
                       uint16_t * target_idx_p
                                /**< Copies out the target idx so you don't have to look for it again. */
    )
{
    struct trEvtP_StufInfo_s me;
    me.ret_val = trAPIRet_Gen_InProcess;

    if (trEvtP_getp(*target_idx_p = trM_CP_TarDesT_idx_get(aKIDS_p), &me.ci_p, &me.cb_p, &me.db_p, &me.pl_p)) {
        me.thesourceid = trM_CP_Source_ID_get(aKIDS_p, &trTi_TaskIDTree[0], trTi_NumOfTasks_PlusOne);
        me.thesourcepri =
            ((struct trTP_CI_s *)trTi_TaskInfoList[trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx].ib_t_p)->
            ci_NominalPriority;

        truPACM4_Private_Section(trEvtP_stuf_PrivSect_hdlr, aKIDS_p, (void *)&me);
    } else {
        me.ret_val = trAPIRet_Usr_CallNotValid;
    }

    return me.ret_val;
}

/** Look up the task ID's config info and blocks if any; copy out relevant info.
*
* Expects a call direct.
*/
void trEvt_status(void *aKIDS_p)
{
    uint16_t desttask_idx = trM_CP_TarDesT_idx_get(aKIDS_p);
    struct trEvtP_CI_s *ci_p;
    struct trEvtP_CB_s *cb_p;
    struct trEvtP_DB_s *db_p;
    struct trPl_Lst_s *pl_p;
    if (trEvtP_getp(desttask_idx, &ci_p, &cb_p, &db_p, &pl_p)) {
        (trKIDS_Evt_Status_aKIDS_p()->kes_stat_p)->res_task_ID = trM_CP_TarDesT_ID_get(aKIDS_p);
        (trKIDS_Evt_Status_aKIDS_p()->kes_stat_p)->res_max_len = ci_p->ci_EvtLen;
        (trKIDS_Evt_Status_aKIDS_p()->kes_stat_p)->res_len_used = cb_p->cb_len_used;
        (trKIDS_Evt_Status_aKIDS_p()->cp).cp_ret_val = trAPIRet_OK;
    } else {
        (trKIDS_Evt_Status_aKIDS_p()->cp).cp_ret_val = trAPIRet_Usr_CallNotValid;
    }
}

/** Post event to any task's stream; successful or not, the calling task may be rescheduled and blocked.
*
* Note you can post to any task; so the destination is assumed good.
*
* Note this is just a stuff with caller rescheduling.
*/
void trEvt_post(void *aKIDS_p)
{
    if (trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx == trTi_Tei_idx) {
        trKIDS_ComParams_aKIDS_p_AntiAlias().cp_ret_val = trAPIRet_Usr_NotValidAtThisTime;
    } else {
        trEvt_stf(aKIDS_p);
    }
    // stf does the in-service completion for us
}

/** Stuffs events to any task's stream; successful or not, the calling task is not rescheduled or blocked.
*
* Note you can stuff any task; so the destination is assumed good.
*
* A few things:
*
* - If the stuff is successful, then see if event grab/pend is pending.
* - If so, cross over, to the grab for the target, and make the target ready.
* - Note that the stuffing-ing task cannot be the same as the grabbing task (think about it).
*/
void trEvt_stf(void *aKIDS_p)
{
    struct trTstc_Ready_Resc_Info_s targ;
    targ.task_susp_state = trTS_Susp_ForEvt;

    (trKIDS_Evt_StfPost_aKIDS_p()->cp).cp_ret_val = trEvt_privStf(aKIDS_p, &targ.task_idx);

    if (((trKIDS_Evt_StfPost_aKIDS_p()->cp).cp_ret_val == trAPIRet_OK)
        && trEvt_xoverGrb(targ.task_idx)) {
        truPACM4_Private_Section(trTstc_ready_Resc_PrivSect_hdlr, NULL, (void *)&targ);
    }
    trTstc_completedInServ(aKIDS_p);
}

/** Pends for events from the calling task's event queue; successful or not, the calling task may be rescheduled and blocked.
*
* Note that pend is valid only for the calling task; 
* you can't "pend" on another task;
* so the calling task's index is forced.
*/
void trEvt_pend(void *aKIDS_p)
{
    uint16_t ret_val;
    struct trTstc_Susp_Resc_Info_s me;
    me.task_idx = trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx;
    me.task_susp_state = trTS_Susp_ForEvt;

    if (trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx == trTi_Tei_idx) {
        trKIDS_ComParams_aKIDS_p_AntiAlias().cp_ret_val = trAPIRet_Usr_NotValidAtThisTime;
    } else {
        ret_val = trEvt_privGrb(aKIDS_p);
        if (ret_val == trAPIRet_Evt_Nothing2Grb) {
            // Don't change in-service task's return value...yet.
            truPACM4_Private_Section(trTstc_susp_Resc_PrivSect_hdlr, NULL, (void *)&me);
        } else {
            (trKIDS_Evt_GrbPend_aKIDS_p()->cp).cp_ret_val = ret_val;
        }
    }
    trTstc_completedInServ(aKIDS_p);
}

/** Tries to grab an event from the calling task's event queue; successful or not, the calling task is not rescheduled or blocked.
*/
void trEvt_grb(void *aKIDS_p)
{
    (trKIDS_Evt_GrbPend_aKIDS_p()->cp).cp_ret_val = trEvt_privGrb(aKIDS_p);
    trTstc_completedInServ(aKIDS_p);
}

/** Event inits.
*/
void trEvt_init(void)
{
    uint16_t i;
    struct trEvtP_CI_s *ci_p;
    struct trEvtP_CB_s *cb_p;
    struct trEvtP_DB_s *db_p;
    struct trPl_Lst_s *pl_p;

    for (i = 0; i < trTi_NumOfTasks; i++) {
        if (trEvtP_getp(i, &ci_p, &cb_p, &db_p, &pl_p)) {
            cb_p->cb_len_used = 0;
            trPl_init(ci_p->ci_pl_p, ci_p->ci_EvtLen);
            trPl_initRootEle(&(cb_p->cb_root_ele));
        }
    }

}
