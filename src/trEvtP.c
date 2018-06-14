/** \file
*   \brief ThingRendevous's Event Private implementation
*
* Private types known only to Event and trTi code generator Trtgen.
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
#include "trKIDS.h"
#include "trAPIRet.h"
#include "trTi.h"
#include "trM.h"
#include "trTS.h"
#include "trTP.h"
#include "trPl.h"
#include "trEvtP.h"
#include "trEvtP.h"

/** For a task index, serve up all pointers to info used to service the event.
*
* \retval true - event info exists for this task index
* \retval false - no event associated with this task.
*/
bool trEvtP_getp(uint16_t forTask_idx,   /**< Index, not the task ID, of the task of interest. */
                 struct trEvtP_CI_s **ci_pp,
                                     /**< Where to put the pointer to the config info, if any.  */
                 struct trEvtP_CB_s **cb_pp,
                                     /**< Where to put the pointer to a control block, if any.  */
                 struct trEvtP_DB_s **db_pp,
                                     /**< Where to put the pointer to a data block, if any. */
                 struct trPl_Lst_s **pl_pp
                                   /**< Wher to put the pointer to the priority list, used to resolve data blocks. */
    )
{
    bool ret_val = false;

    if ((forTask_idx < trTi_NumOfTasks)
        && (trTi_TaskInfoList[forTask_idx].ib_evt_p != NULL)) {
        *ci_pp = (struct trEvtP_CI_s *)trTi_TaskInfoList[forTask_idx].ib_evt_p;
        *cb_pp = (struct trEvtP_CB_s *)trTi_TaskInfoList[forTask_idx].ib_evt_p->ci_cb_p;
        *db_pp = (struct trEvtP_DB_s *)trTi_TaskInfoList[forTask_idx].ib_evt_p->ci_db_p;
        *pl_pp = (struct trPl_Lst_s *)trTi_TaskInfoList[forTask_idx].ib_evt_p->ci_pl_p;
        ret_val = true;
    }
    return ret_val;
}

/** Complete the grab in a private section.
*/
void trEvtP_grab_PrivSect_hdlr(void *aKIDS_p, /**< Associated KIDS. */
                               void *grabInfo_s_p   /**< Non-private-section acquired/returned data produced by this handler. */
    )
{
    uint16_t theentry_idx;

    if ((trEvtP_GrabInfo_p()->cb_p->cb_len_used > 0)
        && ((theentry_idx = trPl_Hi_remove(trEvtP_GrabInfo_p()->pl_p, &(trEvtP_GrabInfo_p()->cb_p->cb_root_ele)))
            != trB_None)) {
        *(trKIDS_Evt_GrbPend_aKIDS_p()->kegp_source_p) = trEvtP_GrabInfo_p()->db_p[theentry_idx].db_source_ID;
        *(trKIDS_Evt_GrbPend_aKIDS_p()->kegp_flags_p) = trEvtP_GrabInfo_p()->db_p[theentry_idx].db_flags;
        *(trKIDS_Evt_GrbPend_aKIDS_p()->kegp_event_p) = trEvtP_GrabInfo_p()->db_p[theentry_idx].db_evt;

        --(trEvtP_GrabInfo_p()->cb_p->cb_len_used);
        trPl_makeFree(theentry_idx, trEvtP_GrabInfo_p()->pl_p);

        trEvtP_GrabInfo_p()->ret_val = trAPIRet_OK;
    } else {
        trEvtP_GrabInfo_p()->ret_val = trAPIRet_Evt_Nothing2Grb;
    }
}

/** Complete the xover grab in a private section.
*/
void trEvtP_xoverGrab_PrivSect_hdlr(void *aKIDS_p, /**< Associated KIDS. */
                                    void *xoverGrabInfo_s_p
                                                         /**< Non-private-section acquired/returned data produced by this handler. */
    )
{
    uint16_t theentry_idx;

    if (trTS_Susp_tst
        (((struct trTP_CB_s *)trTi_TaskInfoList[trEvtP_XoverGrabInfo_p()->target_task_idx].ib_t_p->
          ci_cb_p)->cb_task_state, trTS_Susp_ForEvt)
        && (trEvtP_XoverGrabInfo_p()->cb_p->cb_len_used > 0)
        &&
        ((theentry_idx = trPl_Hi_remove(trEvtP_XoverGrabInfo_p()->pl_p, &(trEvtP_XoverGrabInfo_p()->cb_p->cb_root_ele)))
         != trB_None)) {
        *(trKIDS_Evt_GrbPend_aKIDS_p()->kegp_source_p) = trEvtP_XoverGrabInfo_p()->db_p[theentry_idx].db_source_ID;
        *(trKIDS_Evt_GrbPend_aKIDS_p()->kegp_flags_p) = trEvtP_XoverGrabInfo_p()->db_p[theentry_idx].db_flags;
        *(trKIDS_Evt_GrbPend_aKIDS_p()->kegp_event_p) = trEvtP_XoverGrabInfo_p()->db_p[theentry_idx].db_evt;

        --(trEvtP_XoverGrabInfo_p()->cb_p->cb_len_used);
        trPl_makeFree(theentry_idx, trEvtP_XoverGrabInfo_p()->pl_p);

        (trKIDS_Evt_GrbPend_aKIDS_p()->cp).cp_ret_val = trAPIRet_OK;
        trEvtP_XoverGrabInfo_p()->ret_val = true;
    }                           // implied else
    // nothing to get, so
    // don't set the KIDS result
    // return false (ret_val should be init'd to this)
    // and don't reschedule
}

/** Complete the stuf in a private section.
*/
void trEvtP_stuf_PrivSect_hdlr(void *aKIDS_p, /**< Associated KIDS. */
                               void *stufInfo_s_p   /**< Non-private-section acquired/returned data produced by this handler. */
    )
{
    uint16_t whichevent2fillin;

    if ((trEvtP_StufInfo_p()->cb_p->cb_len_used < trEvtP_StufInfo_p()->ci_p->ci_EvtLen)) {

        whichevent2fillin = trPl_findFree(trEvtP_StufInfo_p()->pl_p, trEvtP_StufInfo_p()->ci_p->ci_EvtLen);
        trPl_Pri_set(whichevent2fillin, trEvtP_StufInfo_p()->thesourcepri, trEvtP_StufInfo_p()->pl_p);

        trEvtP_StufInfo_p()->db_p[whichevent2fillin].db_source_ID = trEvtP_StufInfo_p()->thesourceid;
        trEvtP_StufInfo_p()->db_p[whichevent2fillin].db_flags = trKIDS_Evt_StfPost_aKIDS_p()->kesp_flags;
        trEvtP_StufInfo_p()->db_p[whichevent2fillin].db_evt = trKIDS_Evt_StfPost_aKIDS_p()->kesp_event;

        trPl_insert(whichevent2fillin, trEvtP_StufInfo_p()->pl_p, &(trEvtP_StufInfo_p()->cb_p->cb_root_ele)
            );
        ++(trEvtP_StufInfo_p()->cb_p->cb_len_used);

        trEvtP_StufInfo_p()->ret_val = trAPIRet_OK;
    } else {
        trEvtP_StufInfo_p()->ret_val = trAPIRet_Evt_NoRoom2PostOrStf;
    }
}
