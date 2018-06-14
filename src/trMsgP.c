/** \file
*   \brief ThingRendevous's Message Private implementation
*
* Private types known only to Message and trTi code generator Trtgen.
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
#include "trTP.h"
#include "trPl.h"
#include "trMsgP.h"

/** For a task index, serve up all pointers to info used to service the message.
*
* \retval true - message info exists for this task index
* \retval false - no message associated with this task.
*/
bool trMsgP_getp(uint16_t forTask_idx,   /**< Index, not the task ID, of the task of interest. */
                 struct trMsgP_CI_s **ci_pp,
                                     /**< Where to put the pointer to the config info, if any.  */
                 struct trMsgP_CB_s **cb_pp,
                                     /**< Where to put the pointer to a control block, if any.  */
                 struct trMsgP_DB_s **db_pp,
                                     /**< Where to put the pointer to a data block, if any. */
                 uint8_t ** py_pp,
                          /**< Where to put the base of the message payload array, if any.  */
                 struct trPl_Lst_s **pl_pp
                                   /**< Wher to put the pointer to the priority list, used to resolve data blocks. */
    )
{
    bool ret_val = false;

    if ((forTask_idx < trTi_NumOfTasks)
        && (trTi_TaskInfoList[forTask_idx].ib_msg_p != NULL)) {
        *ci_pp = (struct trMsgP_CI_s *)trTi_TaskInfoList[forTask_idx].ib_msg_p;
        *cb_pp = (struct trMsgP_CB_s *)trTi_TaskInfoList[forTask_idx].ib_msg_p->ci_cb_p;
        *db_pp = (struct trMsgP_DB_s *)trTi_TaskInfoList[forTask_idx].ib_msg_p->ci_db_p;
        *py_pp = (uint8_t *) trTi_TaskInfoList[forTask_idx].ib_msg_p->ci_py_p;
        *pl_pp = (struct trPl_Lst_s *)trTi_TaskInfoList[forTask_idx].ib_msg_p->ci_pl_p;
        ret_val = true;
    }
    return ret_val;
}

/** Complete the tak (Priority) in a private section.
*/
void trMsgP_takPriQ_PrivSect_hdlr(void *aKIDS_p, /**< Associated KIDS. */
                                  void *takInfo_s_p/**< Non-private-section acquired/returned data produced by this handler. */
    )
{
    uint16_t theentry_idx;

    if ((trMsgP_TakInfo_p()->cb_p->cb_len_used > 0)
        && ((theentry_idx = trPl_Hi_remove(trMsgP_TakInfo_p()->pl_p, &(trMsgP_TakInfo_p()->cb_p->cb_root_ele)))
            != trB_None)) {
        *trKIDS_Msg_TakRecv_aKIDS_p()->kmtr_source_p = trMsgP_TakInfo_p()->db_p[theentry_idx].db_source_ID;
        *trKIDS_Msg_TakRecv_aKIDS_p()->kmtr_flags_p = trMsgP_TakInfo_p()->db_p[theentry_idx].db_flags;
        memcpy(trKIDS_Msg_TakRecv_aKIDS_p()->kmtr_payload_p,
               &(trMsgP_TakInfo_p()->py_p[theentry_idx * trMsgP_TakInfo_p()->ci_p->ci_MsgPayload]),
               trMsgP_TakInfo_p()->ci_p->ci_MsgPayload);

        --(trMsgP_TakInfo_p()->cb_p->cb_len_used);
        trPl_makeFree(theentry_idx, trMsgP_TakInfo_p()->pl_p);

        trMsgP_TakInfo_p()->ret_val = trAPIRet_OK;
    } else {
        trMsgP_TakInfo_p()->ret_val = trAPIRet_Msg_Nothing2Tak;
    }
}

/** Complete the tak (FIFO) in a private section.
*/
void trMsgP_takFIFO_PrivSect_hdlr(void *aKIDS_p, /**< Associated KIDS. */
                                  void *takInfo_s_p/**< Non-private-section acquired/returned data produced by this handler. */
    )
{
    if (trMsgP_TakInfo_p()->cb_p->cb_len_used > 0) {
        *trKIDS_Msg_TakRecv_aKIDS_p()->kmtr_source_p =
            trMsgP_TakInfo_p()->db_p[trMsgP_TakInfo_p()->cb_p->cb_offset_recv].db_source_ID;
        *trKIDS_Msg_TakRecv_aKIDS_p()->kmtr_flags_p =
            trMsgP_TakInfo_p()->db_p[trMsgP_TakInfo_p()->cb_p->cb_offset_recv].db_flags;
        memcpy(trKIDS_Msg_TakRecv_aKIDS_p()->kmtr_payload_p,
               &(trMsgP_TakInfo_p()->py_p
                 [trMsgP_TakInfo_p()->cb_p->cb_offset_recv * trMsgP_TakInfo_p()->ci_p->ci_MsgPayload]),
               trMsgP_TakInfo_p()->ci_p->ci_MsgPayload);

        if (++(trMsgP_TakInfo_p()->cb_p->cb_offset_recv) >= trMsgP_TakInfo_p()->ci_p->ci_MsgLen) {
            trMsgP_TakInfo_p()->cb_p->cb_offset_recv = 0;
        }
        --(trMsgP_TakInfo_p()->cb_p->cb_len_used);

        trMsgP_TakInfo_p()->ret_val = trAPIRet_OK;
    } else {
        trMsgP_TakInfo_p()->ret_val = trAPIRet_Msg_Nothing2Tak;
    }
}

/** Complete the xoverTak (Priority) in a private section.
*/
void trMsgP_xoverTakPriQ_PrivSect_hdlr(void *aKIDS_p, /**< Associated KIDS. */
                                       void *xoverTakInfo_s_p
                                                        /**< Non-private-section acquired/returned data produced by this handler. */
    )
{
    uint16_t theentry_idx;

    if (trTS_Susp_tst
        (((struct trTP_CB_s *)trTi_TaskInfoList[trMsgP_XoverTakInfo_p()->target_task_idx].ib_t_p->
          ci_cb_p)->cb_task_state, trTS_Susp_ForMsg)
        && (trMsgP_XoverTakInfo_p()->cb_p->cb_len_used > 0)
        &&
        ((theentry_idx = trPl_Hi_remove(trMsgP_XoverTakInfo_p()->pl_p, &(trMsgP_XoverTakInfo_p()->cb_p->cb_root_ele)))
         != trB_None)) {
        *trKIDS_Msg_TakRecv_aKIDS_p()->kmtr_source_p = trMsgP_XoverTakInfo_p()->db_p[theentry_idx].db_source_ID;
        *trKIDS_Msg_TakRecv_aKIDS_p()->kmtr_flags_p = trMsgP_XoverTakInfo_p()->db_p[theentry_idx].db_flags;
        memcpy(trKIDS_Msg_TakRecv_aKIDS_p()->kmtr_payload_p,
               &(trMsgP_XoverTakInfo_p()->py_p[theentry_idx * trMsgP_XoverTakInfo_p()->ci_p->ci_MsgPayload]),
               trMsgP_XoverTakInfo_p()->ci_p->ci_MsgPayload);

        --(trMsgP_XoverTakInfo_p()->cb_p->cb_len_used);
        trPl_makeFree(theentry_idx, trMsgP_XoverTakInfo_p()->pl_p);

        (trKIDS_Msg_TakRecv_aKIDS_p()->cp).cp_ret_val = trAPIRet_OK;
        trMsgP_XoverTakInfo_p()->ret_val = true;
    }                           // implied else {
    // nothing to get, so
    // don't set the KIDS result
    // return false (should be init'd to that)
    // and don't reschedule
}

/** Complete the xoverTak (FIFO) in a private section.
*/
void trMsgP_xoverTakFIFO_PrivSect_hdlr(void *aKIDS_p, /**< Associated KIDS. */
                                       void *xoverTakInfo_s_p
                                                        /**< Non-private-section acquired/returned data produced by this handler. */
    )
{
    if (trTS_Susp_tst
        (((struct trTP_CB_s *)trTi_TaskInfoList[trMsgP_XoverTakInfo_p()->target_task_idx].ib_t_p->
          ci_cb_p)->cb_task_state, trTS_Susp_ForMsg)
        && (trMsgP_XoverTakInfo_p()->cb_p->cb_len_used > 0)) {
        *trKIDS_Msg_TakRecv_aKIDS_p()->kmtr_source_p =
            trMsgP_XoverTakInfo_p()->db_p[trMsgP_XoverTakInfo_p()->cb_p->cb_offset_recv].db_source_ID;
        *trKIDS_Msg_TakRecv_aKIDS_p()->kmtr_flags_p =
            trMsgP_XoverTakInfo_p()->db_p[trMsgP_XoverTakInfo_p()->cb_p->cb_offset_recv].db_flags;
        memcpy(trKIDS_Msg_TakRecv_aKIDS_p()->kmtr_payload_p,
               &(trMsgP_XoverTakInfo_p()->py_p
                 [trMsgP_XoverTakInfo_p()->cb_p->cb_offset_recv * trMsgP_XoverTakInfo_p()->ci_p->ci_MsgPayload]),
               trMsgP_XoverTakInfo_p()->ci_p->ci_MsgPayload);

        if (++(trMsgP_XoverTakInfo_p()->cb_p->cb_offset_recv) >= trMsgP_XoverTakInfo_p()->ci_p->ci_MsgLen) {
            trMsgP_XoverTakInfo_p()->cb_p->cb_offset_recv = 0;
        }
        --(trMsgP_XoverTakInfo_p()->cb_p->cb_len_used);

        (trKIDS_Msg_TakRecv_aKIDS_p()->cp).cp_ret_val = trAPIRet_OK;
        trMsgP_XoverTakInfo_p()->ret_val = true;
    }                           // implied else {
    // nothing to get, so
    // don't set the KIDS result
    // return false (should be init'd to that)
    // and don't reschedule
}

/** Complete the giv (Priority) in a private section.
*/
void trMsgP_givPriQ_PrivSect_hdlr(void *aKIDS_p, /**< Associated KIDS. */
                                  void *givInfo_s_p/**< Non-private-section acquired/returned data produced by this handler. */
    )
{
    uint16_t whichmsg2fillin;

    if ((trMsgP_GivInfo_p()->cb_p->cb_len_used < trMsgP_GivInfo_p()->ci_p->ci_MsgLen)) {

        whichmsg2fillin = trPl_findFree(trMsgP_GivInfo_p()->pl_p, trMsgP_GivInfo_p()->ci_p->ci_MsgLen);
        trPl_Pri_set(whichmsg2fillin, trMsgP_GivInfo_p()->thesourcepri, trMsgP_GivInfo_p()->pl_p);

        trMsgP_GivInfo_p()->db_p[whichmsg2fillin].db_source_ID = trMsgP_GivInfo_p()->thesourceid;
        trMsgP_GivInfo_p()->db_p[whichmsg2fillin].db_flags = trKIDS_Msg_GivSend_aKIDS_p()->kmgs_flags;
        memcpy(&(trMsgP_GivInfo_p()->py_p[whichmsg2fillin * trMsgP_GivInfo_p()->ci_p->ci_MsgPayload]),
               trKIDS_Msg_GivSend_aKIDS_p()->kmgs_payload_p, trMsgP_GivInfo_p()->ci_p->ci_MsgPayload);

        trPl_insert(whichmsg2fillin, trMsgP_GivInfo_p()->pl_p, &(trMsgP_GivInfo_p()->cb_p->cb_root_ele)
            );
        ++(trMsgP_GivInfo_p()->cb_p->cb_len_used);

        trMsgP_GivInfo_p()->ret_val = trAPIRet_OK;
    } else {
        trMsgP_GivInfo_p()->ret_val = trAPIRet_Msg_NoRoom2SendOrGiv;
    }
}

/** Complete the giv (FIFO) in a private section.
*/
void trMsgP_givFIFO_PrivSect_hdlr(void *aKIDS_p, /**< Associated KIDS. */
                                  void *givInfo_s_p/**< Non-private-section acquired/returned data produced by this handler. */
    )
{
    if (trMsgP_GivInfo_p()->cb_p->cb_len_used < trMsgP_GivInfo_p()->ci_p->ci_MsgLen) {

        trMsgP_GivInfo_p()->db_p[trMsgP_GivInfo_p()->cb_p->cb_offset_send].db_source_ID =
            trMsgP_GivInfo_p()->thesourceid;
        trMsgP_GivInfo_p()->db_p[trMsgP_GivInfo_p()->cb_p->cb_offset_send].db_flags =
            trKIDS_Msg_GivSend_aKIDS_p()->kmgs_flags;
        memcpy(&
               (trMsgP_GivInfo_p()->py_p
                [trMsgP_GivInfo_p()->cb_p->cb_offset_send * trMsgP_GivInfo_p()->ci_p->ci_MsgPayload]),
               trKIDS_Msg_GivSend_aKIDS_p()->kmgs_payload_p, trMsgP_GivInfo_p()->ci_p->ci_MsgPayload);

        if (++(trMsgP_GivInfo_p()->cb_p->cb_offset_send) >= trMsgP_GivInfo_p()->ci_p->ci_MsgLen) {
            trMsgP_GivInfo_p()->cb_p->cb_offset_send = 0;
        }
        ++(trMsgP_GivInfo_p()->cb_p->cb_len_used);

        trMsgP_GivInfo_p()->ret_val = trAPIRet_OK;
    } else {
        trMsgP_GivInfo_p()->ret_val = trAPIRet_Msg_NoRoom2SendOrGiv;
    }
}
