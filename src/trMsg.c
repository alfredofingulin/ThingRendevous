/** \file
*   \brief ThingRendevous's Message implementation
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
#include <string.h>
#include "trKIDS.h"
#include "trAPIRet.h"
#include "trTi.h"
#include "trM.h"
#include "trTS.h"
#include "trTstc.h"
#include "trTP.h"
#include "trPl.h"
#include "truPACM4.h"
#include "trMsgP.h"
#include "trMsg.h"

/** Look up the task ID's config info and blocks if any; copy out relevant info.
*
* Expects a call direct.
*/
void trMsg_status(void *aKIDS_p)
{
    uint16_t desttask_idx = trM_CP_TarDesT_idx_get(aKIDS_p);
    struct trMsgP_CI_s *ci_p;
    struct trMsgP_CB_s *cb_p;
    struct trMsgP_DB_s *db_p;
    uint8_t *py_p;
    struct trPl_Lst_s *pl_p;
    if (trMsgP_getp(desttask_idx, &ci_p, &cb_p, &db_p, &py_p, &pl_p)) {
        (trKIDS_Msg_Status_aKIDS_p()->kms_stat_p)->rms_task_ID = trM_CP_TarDesT_ID_get(aKIDS_p);
        (trKIDS_Msg_Status_aKIDS_p()->kms_stat_p)->rms_cfg = ci_p->ci_MsgOrdering;
        (trKIDS_Msg_Status_aKIDS_p()->kms_stat_p)->rms_payload_size = ci_p->ci_MsgPayload;
        (trKIDS_Msg_Status_aKIDS_p()->kms_stat_p)->rms_max_len = ci_p->ci_MsgLen;
        (trKIDS_Msg_Status_aKIDS_p()->kms_stat_p)->rms_len_used = cb_p->cb_len_used;
        (trKIDS_Msg_Status_aKIDS_p()->cp).cp_ret_val = trAPIRet_OK;
    } else {
        (trKIDS_Msg_Status_aKIDS_p()->cp).cp_ret_val = trAPIRet_Usr_CallNotValid;
    }
}

/** This private take is no different from the public one, but the API status is returned and not filled-in at the KIDs pointer.
*
* Tries to take a message from the calling task's message queue; successful or not.
*
* \retval trAPIRet_OK trAPIRet_Msg_Nothing2Tak trAPIRet_Usr_CallNotValid
*/
uint16_t trMsg_privTak(void *aKIDS_p)
{
    struct trMsgP_TakInfo_s me;
    me.ret_val = trAPIRet_Gen_InProcess;

    if (trMsgP_getp
        (trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx, &me.ci_p, &me.cb_p, &me.db_p, &me.py_p, &me.pl_p)) {

        if ((me.ci_p->ci_MsgOrdering & 0x0001) > 0)     // priority queue
        {
            truPACM4_Private_Section(trMsgP_takPriQ_PrivSect_hdlr, aKIDS_p, (void *)&me);
        } else                  // FIFO
        {
            truPACM4_Private_Section(trMsgP_takFIFO_PrivSect_hdlr, aKIDS_p, (void *)&me);
        }

    } else {
        me.ret_val = trAPIRet_Usr_CallNotValid;
    }

    return me.ret_val;
}

/** This cross-over take is no different from the private one, but the target index is assumed good and the API status is filled-in at the KIDs pointer.
*
* Use this when you know the send went-off
* and you need to prime the target 
* when a take/recv is... well... available.
*
* \retval true - a cross was executed; make this task ready
* \retval false - no crossover; no need to make ready
*/
bool trMsg_xoverTak(uint16_t target_task_idx
                                  /**< Presumed valid task index.  Always has task info; by assumption, has event info. */
    ) {
    void *aKIDS_p = ((struct trTP_CB_s *)trTi_TaskInfoList[target_task_idx].ib_t_p->ci_cb_p)->cb_aKIDS_p;
    struct trMsgP_XoverTakInfo_s me;
    me.ret_val = false;
    me.ci_p = (struct trMsgP_CI_s *)trTi_TaskInfoList[target_task_idx].ib_msg_p;
    me.cb_p = (struct trMsgP_CB_s *)trTi_TaskInfoList[target_task_idx].ib_msg_p->ci_cb_p;
    me.db_p = (struct trMsgP_DB_s *)trTi_TaskInfoList[target_task_idx].ib_msg_p->ci_db_p;
    me.py_p = (uint8_t *) trTi_TaskInfoList[target_task_idx].ib_msg_p->ci_py_p;
    me.pl_p = trTi_TaskInfoList[target_task_idx].ib_msg_p->ci_pl_p;
    me.target_task_idx = target_task_idx;

    if ((((struct trMsgP_CI_s *)trTi_TaskInfoList[target_task_idx].ib_msg_p)->ci_MsgOrdering & 0x0001) > 0)     // priority queue
    {
        truPACM4_Private_Section(trMsgP_xoverTakPriQ_PrivSect_hdlr, aKIDS_p, (void *)&me);
    } else                      // FIFO
    {
        truPACM4_Private_Section(trMsgP_xoverTakFIFO_PrivSect_hdlr, aKIDS_p, (void *)&me);
    }
    return me.ret_val;
}

/** This private give is no different from the public one, but the API status is returned and not filled-in at the KIDs pointer.
*
* Note you can give a message to any task; so the destination is assumed good.
*
* \retval trAPIRet_OK trAPIRet_Msg_NoRoom2SendOrGiv trAPIRet_Usr_CallNotValid
*/
uint16_t trMsg_privGiv(void *aKIDS_p,
                        /**< Obvious. */
                       uint16_t * target_idx_p
                                /**< Copies out the target idx so you don't have to look for it again. */
    )
{
    struct trMsgP_GivInfo_s me;
    me.ret_val = trAPIRet_Gen_InProcess;

    if (trMsgP_getp(*target_idx_p = trM_CP_TarDesT_idx_get(aKIDS_p), &me.ci_p, &me.cb_p, &me.db_p, &me.py_p, &me.pl_p)) {

        me.thesourceid = trM_CP_Source_ID_get(aKIDS_p, &trTi_TaskIDTree[0], trTi_NumOfTasks_PlusOne);
        me.thesourcepri =
            ((struct trTP_CI_s *)trTi_TaskInfoList[trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx].ib_t_p)->
            ci_NominalPriority;

        if ((me.ci_p->ci_MsgOrdering & 0x0001) > 0)     // priority queue
        {
            truPACM4_Private_Section(trMsgP_givPriQ_PrivSect_hdlr, aKIDS_p, (void *)&me);
        } else                  // FIFO
        {
            truPACM4_Private_Section(trMsgP_givFIFO_PrivSect_hdlr, aKIDS_p, (void *)&me);
        }

    } else {
        me.ret_val = trAPIRet_Usr_CallNotValid;
    }

    return me.ret_val;
}

/** Sends messages to any task's message queue; successful or not, the calling task may be rescheduled or blocked.
*
* Note you can send to any task; so the destination is assumed good.
*
* Note this is just a give with caller rescheduling.
*/
void trMsg_send(void *aKIDS_p)
{
    if (trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx == trTi_Tei_idx) {
        trKIDS_ComParams_aKIDS_p_AntiAlias().cp_ret_val = trAPIRet_Usr_NotValidAtThisTime;
    } else {
        trMsg_giv(aKIDS_p);
    }
    // giv does the in-service completion for us
}

/** Gives messages to any task's message queue; succesfull or not, the calling task is not rescheduled or blocked.
*
* Note you can give to any task; so the destination is assumed good.
*
* A few things:
*
* - If the give is successful, the see if message tak/recv is pending.
* - If so, cross over, to the tak for the target, and make the target ready.
* - Note that the giving task cannot be the same as the tak-ing task (think about it).
*/
void trMsg_giv(void *aKIDS_p)
{
    struct trTstc_Ready_Resc_Info_s targ;
    targ.task_susp_state = trTS_Susp_ForMsg;

    (trKIDS_Msg_GivSend_aKIDS_p()->cp).cp_ret_val = trMsg_privGiv(aKIDS_p, &targ.task_idx);

    if (((trKIDS_Msg_GivSend_aKIDS_p()->cp).cp_ret_val == trAPIRet_OK)
        && trMsg_xoverTak(targ.task_idx)) {
        truPACM4_Private_Section(trTstc_ready_Resc_PrivSect_hdlr, NULL, (void *)&targ);
    }
    trTstc_completedInServ(aKIDS_p);
}

/** Receives messages from the calling task's message queue; successful or not, the calling task may rescheduled and blocked.
*
* Note that receive is valid only for the calling task;
* you can't "receive" on another task;
* so the calling task's index is forced.
*/
void trMsg_recv(void *aKIDS_p)
{
    uint16_t ret_val;
    struct trTstc_Susp_Resc_Info_s me;
    me.task_idx = trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx;
    me.task_susp_state = trTS_Susp_ForMsg;

    if (trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx == trTi_Tei_idx) {
        trKIDS_ComParams_aKIDS_p_AntiAlias().cp_ret_val = trAPIRet_Usr_NotValidAtThisTime;
    } else {
        ret_val = trMsg_privTak(aKIDS_p);
        if (ret_val == trAPIRet_Msg_Nothing2Tak) {
            // Don't change in-service task's return value...yet.
            truPACM4_Private_Section(trTstc_susp_Resc_PrivSect_hdlr, NULL, (void *)&me);
        } else {
            (trKIDS_Msg_TakRecv_aKIDS_p()->cp).cp_ret_val = ret_val;
        }
    }
    trTstc_completedInServ(aKIDS_p);
}

/** Tries to take a message from the calling task's message queue; succesful or not, the calling task is not rescheduled or blocked.
*/
void trMsg_tak(void *aKIDS_p)
{
    (trKIDS_Msg_TakRecv_aKIDS_p()->cp).cp_ret_val = trMsg_privTak(aKIDS_p);
    trTstc_completedInServ(aKIDS_p);
}

/** Message inits.
*
*/
void trMsg_init(void)
{
    uint16_t i;
    struct trMsgP_CI_s *ci_p;
    struct trMsgP_CB_s *cb_p;
    struct trMsgP_DB_s *db_p;
    uint8_t *py_p;
    struct trPl_Lst_s *pl_p;

    for (i = 0; i < trTi_NumOfTasks; i++) {
        if (trMsgP_getp(i, &ci_p, &cb_p, &db_p, &py_p, &pl_p)) {
            cb_p->cb_len_used = 0;
            cb_p->cb_offset_recv = 0;
            cb_p->cb_offset_send = 0;
            if (pl_p != NULL) {
                trPl_init(ci_p->ci_pl_p, ci_p->ci_MsgLen);
                trPl_initRootEle(&(cb_p->cb_root_ele));
            }
        }
    }

}
