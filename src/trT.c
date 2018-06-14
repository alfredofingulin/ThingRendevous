/** \file
*   \brief ThingRendevous's Task implementation
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
#include "trPl.h"
#include "truPACM4.h"
#include "trTP.h"
#include "trT.h"

/** Look up the task ID's config info and blocks if any; copy out relevant info.
*
* Expects a call direct.
*/
void trT_status(void *aKIDS_p)
{
    uint16_t desttask_idx = trM_CP_TarDesT_idx_get(aKIDS_p);
    struct trTP_CI_s *ci_p;
    struct trTP_CB_s *cb_p;
    uint8_t *sb_p;
    if (trTP_getp(desttask_idx, &ci_p, &cb_p, &sb_p)) {
        (trKIDS_Task_Status_aKIDS_p()->kts_stat_p)->rts_task_ID = trM_CP_TarDesT_ID_get(aKIDS_p);
        (trKIDS_Task_Status_aKIDS_p()->kts_stat_p)->rts_nominal_priority = ci_p->ci_NominalPriority;
        (trKIDS_Task_Status_aKIDS_p()->kts_stat_p)->rts_priority_offset = cb_p->cb_priority_offset;
        (trKIDS_Task_Status_aKIDS_p()->kts_stat_p)->rts_task_delay_remaining = cb_p->cb_task_delay_remaining;
        (trKIDS_Task_Status_aKIDS_p()->cp).cp_ret_val = trAPIRet_OK;
    } else {
        (trKIDS_Task_Status_aKIDS_p()->cp).cp_ret_val = trAPIRet_Usr_NoSuchID;  // call is valid! but ID is DNE
    }
}

/** Do/change a delay on the target, but not for Tti or Tei.
*
*/
void trT_delay(void *aKIDS_p)
{
    struct trTstc_Susp_Timo_Info_s me;
    me.task_idx = trM_CP_TarDesT_idx_get(aKIDS_p);
    me.timo_val = trKIDS_Task_Delay_aKIDS_p()->ktd_how_long;

    if ((me.task_idx == trTi_Tti_idx) || (me.task_idx == trTi_Tei_idx) || (me.task_idx == trB_None)) {
        (trKIDS_Task_Delay_aKIDS_p()->cp).cp_ret_val = trAPIRet_Usr_CallNotValid;
    } else {
        if (me.timo_val > 0) {
            truPACM4_Private_Section(trTstc_susp_Timo_PrivSect_hdlr, NULL, (void *)&me);
        } else if (me.task_idx != trM_CP_Source_idx_get(aKIDS_p)) {
            truPACM4_Private_Section(trTstc_ready_Timo_PrivSect_hdlr, NULL, (void *)&me);
        }
        // implied else is I'm relinquishing myself; other code will pick this up

        (trKIDS_Task_Delay_aKIDS_p()->cp).cp_ret_val = trAPIRet_OK;
    }
    trTstc_completedInServ(aKIDS_p);
}

/** Task inits, but much is in trTstc and uP-specific in trStart.
*
* Note stack pointer stuff is not done here; done in trStart
* 'cause it can be uP-specific.
*/
void trT_init(void)
{
    uint16_t i;
    struct trTP_CI_s *ci_p;
    struct trTP_CB_s *cb_p;
    uint8_t *sb_p;

    for (i = 0; i < trTi_NumOfTasks; i++) {
        trTP_getp(i, &ci_p, &cb_p, &sb_p);

        cb_p->cb_priority_offset = 0;
        cb_p->cb_task_delay_remaining = 0;
        cb_p->cb_aKIDS_p = NULL;

    }
}
