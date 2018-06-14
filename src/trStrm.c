/** \file
*   \brief ThingRendevous's Stream implementation
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
// no pl
#include "truPACM4.h"
#include "trStrmP.h"
#include "trStrm.h"

/** This private get is no different from the public one, but the API status is returned and not filled-in at the KIDs pointer.
*
* Tries to get bytes from the calling task's stream; successful or not.
*
* \retval trAPIRet_OK trAPIRet_Strm_Nothing2Get trAPIRet_Usr_CallNotValid
*/
uint16_t trStrm_privGet(void *aKIDS_p)
{
    struct trStrmP_GetInfo_s me;
    me.ret_val = trAPIRet_Gen_InProcess;

    if (trStrmP_getp(trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx, &me.ci_p, &me.cb_p, &me.db_p)) {
        truPACM4_Private_Section(trStrmP_get_PrivSect_hdlr, aKIDS_p, (void *)&me);
    } else {
        me.ret_val = trAPIRet_Usr_CallNotValid;
    }

    return me.ret_val;
}

/** This cross-over get is no different from the private one, but the target index is assumed good and the API status is filled-in at the KIDs pointer.
*
* Use this when you know the write went-off
* and you need to prime the target 
* when a get/read is pending.
*
* \retval true - a cross was executed; make this task ready
* \retval false - no crossover; no need to make ready
*/
bool trStrm_xoverGet(uint16_t target_task_idx
                                  /**< Presumed valid task index.  Always has task info; by assumption, has stream info. */
    ) {
    void *aKIDS_p = ((struct trTP_CB_s *)trTi_TaskInfoList[target_task_idx].ib_t_p->ci_cb_p)->cb_aKIDS_p;
    struct trStrmP_XoverGetInfo_s me;
    me.ret_val = false;
    me.ci_p = (struct trStrmP_CI_s *)trTi_TaskInfoList[target_task_idx].ib_strm_p;
    me.cb_p = (struct trStrmP_CB_s *)trTi_TaskInfoList[target_task_idx].ib_strm_p->ci_cb_p;
    me.db_p = trTi_TaskInfoList[target_task_idx].ib_strm_p->ci_db_p;
    me.target_task_idx = target_task_idx;

    truPACM4_Private_Section(trStrmP_xoverGet_PrivSect_hdlr, aKIDS_p, (void *)&me);

    return me.ret_val;
}

/** This private put is no different from the public one, but the API status is returned and not filled-in at the KIDs pointer.
*
* Note you can put to any task; so the destination is assumed good.
*
* \retval trAPIRet_OK trAPIRet_Strm_NoRoom2WritOrPut trAPIRet_Usr_CallNotValid
*/
uint16_t trStrm_privPut(void *aKIDS_p,
                        /**< Obvious. */
                        uint16_t * target_idx_p
                                /**< Copies out the target idx so you don't have to look for it again. */
    )
{
    struct trStrmP_XoverGetInfo_s me;
    me.ret_val = trAPIRet_Gen_InProcess;

    if (trStrmP_getp(*target_idx_p = trM_CP_TarDesT_idx_get(aKIDS_p), &me.ci_p, &me.cb_p, &me.db_p)) {
        truPACM4_Private_Section(trStrmP_put_PrivSect_hdlr, aKIDS_p, (void *)&me);
    } else {
        me.ret_val = trAPIRet_Usr_CallNotValid;
    }

    return me.ret_val;
}

/** Look up the task ID's config info and blocks if any; copy out relevant info.
*
* Expects a call direct.
*/
void trStrm_status(void *aKIDS_p)
{
    struct trStrmP_CI_s *ci_p;
    struct trStrmP_CB_s *cb_p;
    uint8_t *db_p;
    if (trStrmP_getp(trM_CP_TarDesT_idx_get(aKIDS_p), &ci_p, &cb_p, &db_p)) {
        (trKIDS_Strm_Status_aKIDS_p()->kss_stat_p)->rss_task_ID = trM_CP_TarDesT_ID_get(aKIDS_p);
        (trKIDS_Strm_Status_aKIDS_p()->kss_stat_p)->rss_max_len = ci_p->ci_StrmLen;
        (trKIDS_Strm_Status_aKIDS_p()->kss_stat_p)->rss_len_used = cb_p->cb_len_used;
        (trKIDS_Strm_Status_aKIDS_p()->kss_stat_p)->rss_offset_read = cb_p->cb_offset_read;
        (trKIDS_Strm_Status_aKIDS_p()->kss_stat_p)->rss_offset_writ = cb_p->cb_offset_writ;
        (trKIDS_Strm_Status_aKIDS_p()->cp).cp_ret_val = trAPIRet_OK;
    } else {
        (trKIDS_Strm_Status_aKIDS_p()->cp).cp_ret_val = trAPIRet_Usr_CallNotValid;
    }
}

/** Tries to writ bytes to any task's stream; successful or not, the calling task may be rescheduled and blocked.
*
* Note you can writ to any task; so the destination is assumed good.
*
* Note this is just a put with caller rescheduling.
*/
void trStrm_writ(void *aKIDS_p)
{
    if (trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx == trTi_Tei_idx) {
        trKIDS_ComParams_aKIDS_p_AntiAlias().cp_ret_val = trAPIRet_Usr_NotValidAtThisTime;
    } else {
        trStrm_put(aKIDS_p);
    }
    // put does the in-service completion for us
}

/** Tries to put bytes to any task's stream; successful or not, the calling task is not rescheduled or blocked.
*
* Note you can put to any task; so the destination is assumed good.
*
* A few things:
*
* - If the put is successful, then see if stream get/read is pending.
* - If so, cross over, to the get for the target, and make the target ready.
* - Note that the put-ing task cannot be the same as the getting task (think about it).
*/
void trStrm_put(void *aKIDS_p)
{
    struct trTstc_Ready_Resc_Info_s targ;
    targ.task_susp_state = trTS_Susp_ForStrm;

    (trKIDS_Strm_PutWrit_aKIDS_p()->cp).cp_ret_val = trStrm_privPut(aKIDS_p, &targ.task_idx);

    if (((trKIDS_Strm_PutWrit_aKIDS_p()->cp).cp_ret_val == trAPIRet_OK)
        && trStrm_xoverGet(targ.task_idx)) {
        truPACM4_Private_Section(trTstc_ready_Resc_PrivSect_hdlr, NULL, (void *)&targ);
    }
    trTstc_completedInServ(aKIDS_p);
}

/** Tries to read bytes from the calling task's stream; successful or not, the calling task may be rescheduled and blocked.
*
* Note that read is valid only for the calling task; 
* you can't "read" on another task;
* so the calling task's index is forced.
*/
void trStrm_read(void *aKIDS_p)
{
    uint16_t ret_val;
    struct trTstc_Susp_Resc_Info_s me;
    me.task_idx = trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx;
    me.task_susp_state = trTS_Susp_ForStrm;

    if (trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx == trTi_Tei_idx) {
        trKIDS_ComParams_aKIDS_p_AntiAlias().cp_ret_val = trAPIRet_Usr_NotValidAtThisTime;
    } else {
        ret_val = trStrm_privGet(aKIDS_p);
        if (ret_val == trAPIRet_Strm_Nothing2Get) {
            // Don't change in-service task's return value...yet.
            truPACM4_Private_Section(trTstc_susp_Resc_PrivSect_hdlr, NULL, (void *)&me);
        } else {
            (trKIDS_Strm_GetRead_aKIDS_p()->cp).cp_ret_val = ret_val;
        }
    }
    trTstc_completedInServ(aKIDS_p);
}

/** Tries to get bytes from the calling task's stream; successful or not, the calling task is not rescheduled or blocked.
*/
void trStrm_get(void *aKIDS_p)
{
    (trKIDS_Strm_GetRead_aKIDS_p()->cp).cp_ret_val = trStrm_privGet(aKIDS_p);
    trTstc_completedInServ(aKIDS_p);
}

/** Stream inits.
*/
void trStrm_init(void)
{
    uint16_t i;
    struct trStrmP_CI_s *ci_p;
    struct trStrmP_CB_s *cb_p;
    uint8_t *db_p;

    for (i = 0; i < trTi_NumOfTasks; i++) {
        if (trStrmP_getp(i, &ci_p, &cb_p, &db_p)) {
            cb_p->cb_len_used = 0;
            cb_p->cb_offset_read = 0;
            cb_p->cb_offset_writ = 0;
        }
    }

}
