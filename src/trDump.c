/** \file
*   \brief ThingRendevous's Dump implementation (no interface!)
*
* This hidden module's only purpose is to dump everything internal
* to standard output.
*
* It's a debugging tool.  You call it.  Don't make it part of release code.
*
* Completely undocumented for a reason: To discourage you from using it.
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

#include <stdio.h>

#include "trKIDS.h"
#include "trAPIRet.h"
#include "trBidt.h"
#include "trPl.h"
#include "trTi.h"
#include "trM.h"
#include "trTS.h"
#include "trEvtP.h"
#include "trEvt.h"
#include "trMsgP.h"
#include "trMsg.h"
#include "trStrmP.h"
#include "trStrm.h"
#include "trTP.h"
#include "trT.h"
#include "trSR.h"

void trDump_shout1(void)
{
    const char *const trDump_shout1s = "Shout 1\n";
    printf(trDump_shout1s);
}

void trDump_shout2(void)
{
    const char *const trDump_shout2s = "Shout 2\n";
    printf(trDump_shout2s);
}

const char *const trDump_nl = "\n";
const char *const trDump_by = " %0#3hx";
const char *const trDump_wo = " %0#5hx";

void trDump_stupidNl(void)
{
    printf(trDump_nl);
}

void trDump_byteDump(uint8_t * b_p, uint16_t cnt)
{
    uint16_t i;
    for (i = 0; i < cnt; i++) {
        printf(trDump_by, b_p[i]);
    }
}

void trDump_wordDump(uint16_t * b_p, uint16_t cnt)
{
    uint16_t i;
    for (i = 0; i < cnt; i++) {
        printf(trDump_wo, b_p[i]);
    }
}

uint16_t trDump_id2Idx(uint16_t id)
{
    return trBidt_find(id, (struct trBidt_Node_s *)&trTi_TaskIDTree[0], trTi_NumOfTasks_PlusOne);
}

void trDump_finageIdx(uint16_t idx)
{
    trSR_RunningTask_idx = idx;
}

// uint8_t  %0#3hx
// uint16_t %0#5hx
// uint32_t %0#10lx

void trDump_doEvt(uint16_t id)
{
    uint16_t idx = trDump_id2Idx(id);
    const char *const trdde_SignOn = "---\ntrDump_doEvt  idx %0#5x  ID %0#5x\n";
    printf(trdde_SignOn, idx,
           trBidt_revfind(idx, (struct trBidt_Node_s *)&trTi_TaskIDTree[0], trTi_NumOfTasks_PlusOne));
    struct trEvtP_CI_s *ci_p;
    struct trEvtP_CB_s *cb_p;
    struct trEvtP_DB_s *db_p;
    struct trPl_Lst_s *pl_p;
    uint16_t theentry_idx;
    if (trEvtP_getp(idx, &ci_p, &cb_p, &db_p, &pl_p)) {

        if ((cb_p->cb_len_used > 0)
            && ((theentry_idx = trPl_Hi_peek(&(cb_p->cb_root_ele)))
                != trB_None)) {
            const char *const trdde_EvtQd = "Top has  Source: %0#5hx  Flags: %0#5hx  Event: %0#10lx\n";
            printf(trdde_EvtQd,
                   db_p[theentry_idx].db_source_ID, db_p[theentry_idx].db_flags, db_p[theentry_idx].db_evt);
        } else {
            const char *const trdde_NoEvtsQd = "No Evt queued.\n";
            printf(trdde_NoEvtsQd);
        }

    } else {
        const char *const trdde_Nope = "No Evt.\n";
        printf(trdde_Nope);
    }
}

void trDump_doMsg(uint16_t id)
{
/*	uint16_t idx = trDump_id2Idx(id);
	const char * const trddm_SignOn = "---\ntrDump_doMsg  idx %0#5x  ID %0#5x\n";
	printf(trddm_SignOn, idx, trBidt_revfind( idx, (struct trBidt_Node_s *)&trTi_TaskIDTree[0], trTi_NumOfTasks_PlusOne ) );
	struct trMsgP_CI_s *ci_p;
	struct trMsgP_CB_s *cb_p;
	union trMsgP_DB_u_Nonconst db_p;
	struct trPl_Lst_s *pl_p;
	if ( trMsgP_getp( idx, &ci_p, &cb_p, &db_p, &pl_p ) )
	{
		;
	}
	else
	{
*/ const char *const trddm_Nope = "No Msg.\n";
    printf(trddm_Nope);
/*	}
*/ }

void trDump_doStrm(uint16_t id)
{
    uint16_t idx = trDump_id2Idx(id);
    const char *const trddst_SignOn = "---\ntrDump_doStrm  idx %0#5x  ID %0#5x\n";
    printf(trddst_SignOn, idx,
           trBidt_revfind(idx, (struct trBidt_Node_s *)&trTi_TaskIDTree[0], trTi_NumOfTasks_PlusOne));
    struct trStrmP_CI_s *ci_p;
    struct trStrmP_CB_s *cb_p;
    uint8_t *db_p;
    uint16_t i, j;
    if (trStrmP_getp(idx, &ci_p, &cb_p, &db_p)) {

        const char *const trddst_cb = "used: %0#5x  read: %0#5x  writ: %0#5x\n";
        printf(trddst_cb, cb_p->cb_len_used, cb_p->cb_offset_read, cb_p->cb_offset_writ);
        const char *const trddst_byr = "read bytes: ";
        printf(trddst_byr);
        for (i = 0, j = cb_p->cb_offset_read; i < 5; i++) {
            trDump_byteDump(&db_p[j], 1);
            if (++j > ci_p->ci_StrmLen) {
                j = 0;
            }
        }
        trDump_stupidNl();
        const char *const trddst_byw = "writ bytes: ";
        printf(trddst_byw);
        for (i = 0, j = cb_p->cb_offset_writ; i < 5; i++) {
            trDump_byteDump(&db_p[j], 1);
            if (++j > ci_p->ci_StrmLen) {
                j = 0;
            }
        }
        trDump_stupidNl();
    } else {
        const char *const trddst_Nope = "No Strm.\n";
        printf(trddst_Nope);
    }
}

void trDump_doT(uint16_t id)
{
    uint16_t idx = trDump_id2Idx(id);
    const char *const trddt_SignOn = "---\ntrDump_doT  idx %0#5x  ID %0#5x\n";
    printf(trddt_SignOn, idx,
           trBidt_revfind(idx, (struct trBidt_Node_s *)&trTi_TaskIDTree[0], trTi_NumOfTasks_PlusOne));
    struct trTP_CI_s *ci_p;
    struct trTP_CB_s *cb_p;
    uint8_t *sb_p;
    trTP_getp(idx, &ci_p, &cb_p, &sb_p);
    const char *const trddt_StateDelay = "state %0#5x  delay_remaining %0#5x\n";
    printf(trddt_StateDelay, cb_p->cb_task_state, cb_p->cb_task_delay_remaining);
}

void trDump_doSR(void)
{
    const char *const trdds_SignOn = "===\ntrDump_doSR\n";
    printf(trdds_SignOn);
    const char *const trdds_Run = "RunningTask idx %0#5x\n";
    printf(trdds_Run, trSR_RunningTask_idx);
    const char *const trdds_Ready = "Ready.head idx %0#5x  Ready.tail idx %0#5x\n";
    printf(trdds_Ready, trSR_ReadyTasks_pl.head, trSR_ReadyTasks_pl.tail);
    const char *const trdds_Timeout = "Timeout.head idx %0#5x  Timeout.tail idx %0#5x\n";
    printf(trdds_Timeout, trSR_TimeoutTasks_pl.head, trSR_TimeoutTasks_pl.tail);

}

void trDump_doIt(void)
{

    const char *const trddi_SignOn = "===\ntrDump_doIt\n";
    printf(trddi_SignOn);

}
