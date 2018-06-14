/** \file
*   \brief ThingRendevous's Stream Private implementation
*
* Private types known only to Stream and trTi code generator Trtgen.
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
// no pl
#include "trStrmP.h"

/** For a task index, serve up all pointers to info used to service the stream.
*
* \retval true - stream info exists for this task index
* \retval false - no stream associated with this task.
*/
bool trStrmP_getp(uint16_t forTask_idx,   /**< Index, not the task ID, of the task of interest. */
                  struct trStrmP_CI_s **ci_pp,
                                      /**< Where to put the pointer to the config info, if any.  */
                  struct trStrmP_CB_s **cb_pp,
                                      /**< Where to put the pointer to a control block, if any.  */
                  uint8_t ** db_pp
                         /**< Where to put the pointer to a data block, if any. */
    )
{
    bool ret_val = false;

    if ((forTask_idx < trTi_NumOfTasks)
        && (trTi_TaskInfoList[forTask_idx].ib_strm_p != NULL)) {
        *ci_pp = (struct trStrmP_CI_s *)trTi_TaskInfoList[forTask_idx].ib_strm_p;
        *cb_pp = (struct trStrmP_CB_s *)trTi_TaskInfoList[forTask_idx].ib_strm_p->ci_cb_p;
        *db_pp = trTi_TaskInfoList[forTask_idx].ib_strm_p->ci_db_p;
        ret_val = true;
    }
    return ret_val;
}

/** Complete the get in a private section.
*/
void trStrmP_get_PrivSect_hdlr(void *aKIDS_p,/**< Associated KIDS. */
                               void *getInfo_s_p     /**< Non-private-section acquired/returned data produced by this handler. */
    )
{
    uint16_t toget = 0;

    if (trKIDS_Strm_GetRead_aKIDS_p()->ksgr_max_bytes <= trStrmP_GetInfo_p()->cb_p->cb_len_used) {

        toget = trKIDS_Strm_GetRead_aKIDS_p()->ksgr_max_bytes;

    } else if (trStrmP_GetInfo_p()->cb_p->cb_len_used >= trKIDS_Strm_GetRead_aKIDS_p()->ksgr_min_bytes) {

        toget = trStrmP_GetInfo_p()->cb_p->cb_len_used;

    }

    if (toget == 0) {
        *(trKIDS_Strm_GetRead_aKIDS_p()->ksgr_avail_bytes_p)
            = trStrmP_GetInfo_p()->cb_p->cb_len_used;

        *(trKIDS_Strm_GetRead_aKIDS_p()->ksgr_actual_bytes_p) = 0;
        trStrmP_GetInfo_p()->ret_val = trAPIRet_Strm_Nothing2Get;
    } else {
        trM_wrapcpy_src(trKIDS_Strm_GetRead_aKIDS_p()->ksgr_buff_p,
                        trStrmP_GetInfo_p()->db_p, (uint16_t) trStrmP_GetInfo_p()->ci_p->ci_StrmLen,
                        trStrmP_GetInfo_p()->cb_p->cb_offset_read, toget, &(trStrmP_GetInfo_p()->cb_p->cb_offset_read)
            );

        *(trKIDS_Strm_GetRead_aKIDS_p()->ksgr_avail_bytes_p)
            = trStrmP_GetInfo_p()->cb_p->cb_len_used = trStrmP_GetInfo_p()->cb_p->cb_len_used - toget;

        *(trKIDS_Strm_GetRead_aKIDS_p()->ksgr_actual_bytes_p) = toget;
        trStrmP_GetInfo_p()->ret_val = trAPIRet_OK;
    }
}

/** Complete the xover get in a private section.
*/
void trStrmP_xoverGet_PrivSect_hdlr(void *aKIDS_p,/**< Associated KIDS. */
                                    void *xoverGetInfo_s_p/**< Non-private-section acquired/returned data produced by this handler. */
    )
{
    uint16_t toget = 0;

    if (trTS_Susp_tst
        (((struct trTP_CB_s *)trTi_TaskInfoList[trStrmP_XoverGetInfo_p()->target_task_idx].ib_t_p->
          ci_cb_p)->cb_task_state, trTS_Susp_ForStrm)) {

        if (trKIDS_Strm_GetRead_aKIDS_p()->ksgr_max_bytes <= trStrmP_XoverGetInfo_p()->cb_p->cb_len_used) {

            toget = trKIDS_Strm_GetRead_aKIDS_p()->ksgr_max_bytes;

        } else if (trStrmP_XoverGetInfo_p()->cb_p->cb_len_used >= trKIDS_Strm_GetRead_aKIDS_p()->ksgr_min_bytes) {

            toget = trStrmP_XoverGetInfo_p()->cb_p->cb_len_used;

        }

        if (toget == 0) {
            *(trKIDS_Strm_GetRead_aKIDS_p()->ksgr_avail_bytes_p)
                = trStrmP_XoverGetInfo_p()->cb_p->cb_len_used;

            *(trKIDS_Strm_GetRead_aKIDS_p()->ksgr_actual_bytes_p) = 0;
            // nothing to get, so
            // don't set the KIDS result
            // return false (better be init'd that way)
            // and don't reschedule
        } else {
            trM_wrapcpy_src(trKIDS_Strm_GetRead_aKIDS_p()->ksgr_buff_p,
                            trStrmP_XoverGetInfo_p()->db_p, (uint16_t) trStrmP_XoverGetInfo_p()->ci_p->ci_StrmLen,
                            trStrmP_XoverGetInfo_p()->cb_p->cb_offset_read, toget,
                            &(trStrmP_XoverGetInfo_p()->cb_p->cb_offset_read)
                );

            *(trKIDS_Strm_GetRead_aKIDS_p()->ksgr_avail_bytes_p)
                = trStrmP_XoverGetInfo_p()->cb_p->cb_len_used = trStrmP_XoverGetInfo_p()->cb_p->cb_len_used - toget;

            *(trKIDS_Strm_GetRead_aKIDS_p()->ksgr_actual_bytes_p) = toget;
            (trKIDS_Strm_GetRead_aKIDS_p()->cp).cp_ret_val = trAPIRet_OK;
            trStrmP_XoverGetInfo_p()->ret_val = true;
        }
    }
}

/** Complete the put in a private section.
*/
void trStrmP_put_PrivSect_hdlr(void *aKIDS_p, /**< Associated KIDS. */
                               void *putInfo_s_p   /**< Non-private-section acquired/returned data produced by this handler. */
    )
{
    uint16_t toput = 0;
    uint16_t spaceremaining;
    spaceremaining = trStrmP_PutInfo_p()->ci_p->ci_StrmLen - trStrmP_PutInfo_p()->cb_p->cb_len_used;

    if (trKIDS_Strm_PutWrit_aKIDS_p()->kspw_max_bytes <= spaceremaining) {

        toput = trKIDS_Strm_PutWrit_aKIDS_p()->kspw_max_bytes;

    } else if (spaceremaining >= trKIDS_Strm_PutWrit_aKIDS_p()->kspw_min_bytes) {

        toput = spaceremaining;

    }

    if (toput == 0) {

        *(trKIDS_Strm_PutWrit_aKIDS_p()->kspw_actual_bytes_p) = 0;
        *(trKIDS_Strm_PutWrit_aKIDS_p()->kspw_avail_bytes_p)
            = trStrmP_PutInfo_p()->cb_p->cb_len_used;
        trStrmP_PutInfo_p()->ret_val = trAPIRet_Strm_NoRoom2WritOrPut;
    } else {
        trM_wrapcpy_dest(trStrmP_PutInfo_p()->db_p,
                         (uint16_t) trStrmP_PutInfo_p()->ci_p->ci_StrmLen,
                         trStrmP_PutInfo_p()->cb_p->cb_offset_writ,
                         toput, &(trStrmP_PutInfo_p()->cb_p->cb_offset_writ),
                         trKIDS_Strm_PutWrit_aKIDS_p()->kspw_buff_p);

        *(trKIDS_Strm_PutWrit_aKIDS_p()->kspw_avail_bytes_p)
            = trStrmP_PutInfo_p()->cb_p->cb_len_used = trStrmP_PutInfo_p()->cb_p->cb_len_used + toput;

        *(trKIDS_Strm_PutWrit_aKIDS_p()->kspw_actual_bytes_p) = toput;
        trStrmP_PutInfo_p()->ret_val = trAPIRet_OK;
    }
}
