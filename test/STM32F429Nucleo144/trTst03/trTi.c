/** \file
*   \brief ThingRendevous's Task Info implementation 
*
* Generated by Trtgen.py.
*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "trBidt.h"
#include "trPl.h"
#include "trEvt.h"
#include "trMsg.h"
#include "trStrm.h"
#include "trT.h"
#include "trEvtP.h"
#include "trMsgP.h"
#include "trStrmP.h"
#include "trTP.h"
#include "trTi.h"
/* No need for trTi_Strm_DB_Tti_128_1. */
/* No need for trTi_Strm_CB_Tti_128_1. */
/* No need for trTi_Strm_CI_Tti_128_1. */
/* No need for trTi_Msg_PL_Tti_128_1. */
/* No need for trTi_Msg_PY_Tti_128_1. */
/* No need for trTi_Msg_DB_Tti_128_1. */
/* No need for trTi_Msg_CB_Tti_128_1. */
/* No need for trTi_Msg_CI_Tti_128_1. */
/* No need for trTi_Evt_PL_Tti_128_1. */
/* No need for trTi_Evt_DB_Tti_128_1. */
/* No need for trTi_Evt_CB_Tti_128_1. */
/* No need for trTi_Evt_CI_Tti_128_1. */
uint64_t trTi_Task_SB_Tti_128_1[128]; /* uint64_t forces each 8 byte element on an 8 byte boundary */
struct trTP_CB_s trTi_Task_CB_Tti_128_1;
void trTask_Tti(void *theTaskData);
const struct trTP_CI_s trTi_Task_CI_Tti_128_1 =
{
	&trTi_Task_CB_Tti_128_1, /* ci_cb_p */ 
	trTask_Tti, /* ci_someTask */ 
	NULL, /* ci_theTaskData_p */ 
	(uint8_t *) &trTi_Task_SB_Tti_128_1[0], /* ci_sb_p */ 
	1024, /* ci_StackSize */ 
	128 /* ci_NominalPriority */ 
};
/* No need for trTi_Strm_DB_Test02CCCC_52428_5. */
/* No need for trTi_Strm_CB_Test02CCCC_52428_5. */
/* No need for trTi_Strm_CI_Test02CCCC_52428_5. */
/* No need for trTi_Msg_PL_Test02CCCC_52428_5. */
/* No need for trTi_Msg_PY_Test02CCCC_52428_5. */
/* No need for trTi_Msg_DB_Test02CCCC_52428_5. */
/* No need for trTi_Msg_CB_Test02CCCC_52428_5. */
/* No need for trTi_Msg_CI_Test02CCCC_52428_5. */
/* No need for trTi_Evt_PL_Test02CCCC_52428_5. */
/* No need for trTi_Evt_DB_Test02CCCC_52428_5. */
/* No need for trTi_Evt_CB_Test02CCCC_52428_5. */
/* No need for trTi_Evt_CI_Test02CCCC_52428_5. */
uint64_t trTi_Task_SB_Test02CCCC_52428_5[128]; /* uint64_t forces each 8 byte element on an 8 byte boundary */
struct trTP_CB_s trTi_Task_CB_Test02CCCC_52428_5;
void trTask_Test02CCCC(void *theTaskData);
const struct trTP_CI_s trTi_Task_CI_Test02CCCC_52428_5 =
{
	&trTi_Task_CB_Test02CCCC_52428_5, /* ci_cb_p */ 
	trTask_Test02CCCC, /* ci_someTask */ 
	NULL, /* ci_theTaskData_p */ 
	(uint8_t *) &trTi_Task_SB_Test02CCCC_52428_5[0], /* ci_sb_p */ 
	1024, /* ci_StackSize */ 
	259 /* ci_NominalPriority */ 
};
/* No need for trTi_Strm_DB_Test02DDDD_56797_2. */
/* No need for trTi_Strm_CB_Test02DDDD_56797_2. */
/* No need for trTi_Strm_CI_Test02DDDD_56797_2. */
/* Did not build trTi_Msg_PL_Test02DDDD_56797_2 because FIFO ordering; ci_pl_p will be NULL. */
uint64_t trTi_Msg_PY_Test02DDDD_56797_2[6]; /* uint64_t forces each 8 byte element on an 8 byte boundary */
struct trMsgP_DB_s trTi_Msg_DB_Test02DDDD_56797_2[3];
struct trMsgP_CB_s trTi_Msg_CB_Test02DDDD_56797_2;
const struct trMsgP_CI_s trTi_Msg_CI_Test02DDDD_56797_2 =
{
	3, /* ci_MsgLen */ 
	0, /* ci_MsgOrdering */ 
	16, /* ci_MsgPayload */ 
	&trTi_Msg_CB_Test02DDDD_56797_2, /* ci_cb_p */ 
	&trTi_Msg_DB_Test02DDDD_56797_2[0],  /* ci_db_p */ 
	(uint8_t *) &trTi_Msg_PY_Test02DDDD_56797_2[0], /* ci_py_p */ 
	NULL /* ci_pl_p */ 
};
/* No need for trTi_Evt_PL_Test02DDDD_56797_2. */
/* No need for trTi_Evt_DB_Test02DDDD_56797_2. */
/* No need for trTi_Evt_CB_Test02DDDD_56797_2. */
/* No need for trTi_Evt_CI_Test02DDDD_56797_2. */
uint64_t trTi_Task_SB_Test02DDDD_56797_2[128]; /* uint64_t forces each 8 byte element on an 8 byte boundary */
struct trTP_CB_s trTi_Task_CB_Test02DDDD_56797_2;
void trTask_Test02DDDD(void *theTaskData);
const struct trTP_CI_s trTi_Task_CI_Test02DDDD_56797_2 =
{
	&trTi_Task_CB_Test02DDDD_56797_2, /* ci_cb_p */ 
	trTask_Test02DDDD, /* ci_someTask */ 
	NULL, /* ci_theTaskData_p */ 
	(uint8_t *) &trTi_Task_SB_Test02DDDD_56797_2[0], /* ci_sb_p */ 
	1024, /* ci_StackSize */ 
	260 /* ci_NominalPriority */ 
};
/* No need for trTi_Strm_DB_trTst03_action_256_6. */
/* No need for trTi_Strm_CB_trTst03_action_256_6. */
/* No need for trTi_Strm_CI_trTst03_action_256_6. */
/* No need for trTi_Msg_PL_trTst03_action_256_6. */
/* No need for trTi_Msg_PY_trTst03_action_256_6. */
/* No need for trTi_Msg_DB_trTst03_action_256_6. */
/* No need for trTi_Msg_CB_trTst03_action_256_6. */
/* No need for trTi_Msg_CI_trTst03_action_256_6. */
/* No need for trTi_Evt_PL_trTst03_action_256_6. */
/* No need for trTi_Evt_DB_trTst03_action_256_6. */
/* No need for trTi_Evt_CB_trTst03_action_256_6. */
/* No need for trTi_Evt_CI_trTst03_action_256_6. */
uint64_t trTi_Task_SB_trTst03_action_256_6[128]; /* uint64_t forces each 8 byte element on an 8 byte boundary */
struct trTP_CB_s trTi_Task_CB_trTst03_action_256_6;
void trTask_trTst03_action(void *theTaskData);
const struct trTP_CI_s trTi_Task_CI_trTst03_action_256_6 =
{
	&trTi_Task_CB_trTst03_action_256_6, /* ci_cb_p */ 
	trTask_trTst03_action, /* ci_someTask */ 
	NULL, /* ci_theTaskData_p */ 
	(uint8_t *) &trTi_Task_SB_trTst03_action_256_6[0], /* ci_sb_p */ 
	1024, /* ci_StackSize */ 
	32767 /* ci_NominalPriority */ 
};
/* No need for trTi_Strm_DB_trTst03_init_255_7. */
/* No need for trTi_Strm_CB_trTst03_init_255_7. */
/* No need for trTi_Strm_CI_trTst03_init_255_7. */
/* No need for trTi_Msg_PL_trTst03_init_255_7. */
/* No need for trTi_Msg_PY_trTst03_init_255_7. */
/* No need for trTi_Msg_DB_trTst03_init_255_7. */
/* No need for trTi_Msg_CB_trTst03_init_255_7. */
/* No need for trTi_Msg_CI_trTst03_init_255_7. */
/* No need for trTi_Evt_PL_trTst03_init_255_7. */
/* No need for trTi_Evt_DB_trTst03_init_255_7. */
/* No need for trTi_Evt_CB_trTst03_init_255_7. */
/* No need for trTi_Evt_CI_trTst03_init_255_7. */
uint64_t trTi_Task_SB_trTst03_init_255_7[128]; /* uint64_t forces each 8 byte element on an 8 byte boundary */
struct trTP_CB_s trTi_Task_CB_trTst03_init_255_7;
void trTask_trTst03_init(void *theTaskData);
const struct trTP_CI_s trTi_Task_CI_trTst03_init_255_7 =
{
	&trTi_Task_CB_trTst03_init_255_7, /* ci_cb_p */ 
	trTask_trTst03_init, /* ci_someTask */ 
	NULL, /* ci_theTaskData_p */ 
	(uint8_t *) &trTi_Task_SB_trTst03_init_255_7[0], /* ci_sb_p */ 
	1024, /* ci_StackSize */ 
	255 /* ci_NominalPriority */ 
};
/* No need for trTi_Strm_DB_Test02BBBB_48059_3. */
/* No need for trTi_Strm_CB_Test02BBBB_48059_3. */
/* No need for trTi_Strm_CI_Test02BBBB_48059_3. */
struct trPl_Lst_s trTi_Msg_PL_Test02BBBB_48059_3[4];
uint64_t trTi_Msg_PY_Test02BBBB_48059_3[4]; /* uint64_t forces each 8 byte element on an 8 byte boundary */
struct trMsgP_DB_s trTi_Msg_DB_Test02BBBB_48059_3[4];
struct trMsgP_CB_s trTi_Msg_CB_Test02BBBB_48059_3;
const struct trMsgP_CI_s trTi_Msg_CI_Test02BBBB_48059_3 =
{
	4, /* ci_MsgLen */ 
	1, /* ci_MsgOrdering */ 
	8, /* ci_MsgPayload */ 
	&trTi_Msg_CB_Test02BBBB_48059_3, /* ci_cb_p */ 
	&trTi_Msg_DB_Test02BBBB_48059_3[0],  /* ci_db_p */ 
	(uint8_t *) &trTi_Msg_PY_Test02BBBB_48059_3[0], /* ci_py_p */ 
	&trTi_Msg_PL_Test02BBBB_48059_3[0] /* ci_pl_p */ 
};
/* No need for trTi_Evt_PL_Test02BBBB_48059_3. */
/* No need for trTi_Evt_DB_Test02BBBB_48059_3. */
/* No need for trTi_Evt_CB_Test02BBBB_48059_3. */
/* No need for trTi_Evt_CI_Test02BBBB_48059_3. */
uint64_t trTi_Task_SB_Test02BBBB_48059_3[128]; /* uint64_t forces each 8 byte element on an 8 byte boundary */
struct trTP_CB_s trTi_Task_CB_Test02BBBB_48059_3;
void trTask_Test02BBBB(void *theTaskData);
const struct trTP_CI_s trTi_Task_CI_Test02BBBB_48059_3 =
{
	&trTi_Task_CB_Test02BBBB_48059_3, /* ci_cb_p */ 
	trTask_Test02BBBB, /* ci_someTask */ 
	NULL, /* ci_theTaskData_p */ 
	(uint8_t *) &trTi_Task_SB_Test02BBBB_48059_3[0], /* ci_sb_p */ 
	1024, /* ci_StackSize */ 
	258 /* ci_NominalPriority */ 
};
/* No need for trTi_Strm_DB_Test02AAAA_43690_4. */
/* No need for trTi_Strm_CB_Test02AAAA_43690_4. */
/* No need for trTi_Strm_CI_Test02AAAA_43690_4. */
/* No need for trTi_Msg_PL_Test02AAAA_43690_4. */
/* No need for trTi_Msg_PY_Test02AAAA_43690_4. */
/* No need for trTi_Msg_DB_Test02AAAA_43690_4. */
/* No need for trTi_Msg_CB_Test02AAAA_43690_4. */
/* No need for trTi_Msg_CI_Test02AAAA_43690_4. */
/* No need for trTi_Evt_PL_Test02AAAA_43690_4. */
/* No need for trTi_Evt_DB_Test02AAAA_43690_4. */
/* No need for trTi_Evt_CB_Test02AAAA_43690_4. */
/* No need for trTi_Evt_CI_Test02AAAA_43690_4. */
uint64_t trTi_Task_SB_Test02AAAA_43690_4[128]; /* uint64_t forces each 8 byte element on an 8 byte boundary */
struct trTP_CB_s trTi_Task_CB_Test02AAAA_43690_4;
void trTask_Test02AAAA(void *theTaskData);
const struct trTP_CI_s trTi_Task_CI_Test02AAAA_43690_4 =
{
	&trTi_Task_CB_Test02AAAA_43690_4, /* ci_cb_p */ 
	trTask_Test02AAAA, /* ci_someTask */ 
	NULL, /* ci_theTaskData_p */ 
	(uint8_t *) &trTi_Task_SB_Test02AAAA_43690_4[0], /* ci_sb_p */ 
	1024, /* ci_StackSize */ 
	257 /* ci_NominalPriority */ 
};
/* No need for trTi_Strm_DB_Tei_64_0. */
/* No need for trTi_Strm_CB_Tei_64_0. */
/* No need for trTi_Strm_CI_Tei_64_0. */
/* No need for trTi_Msg_PL_Tei_64_0. */
/* No need for trTi_Msg_PY_Tei_64_0. */
/* No need for trTi_Msg_DB_Tei_64_0. */
/* No need for trTi_Msg_CB_Tei_64_0. */
/* No need for trTi_Msg_CI_Tei_64_0. */
/* No need for trTi_Evt_PL_Tei_64_0. */
/* No need for trTi_Evt_DB_Tei_64_0. */
/* No need for trTi_Evt_CB_Tei_64_0. */
/* No need for trTi_Evt_CI_Tei_64_0. */
uint64_t trTi_Task_SB_Tei_64_0[1]; /* uint64_t forces each 8 byte element on an 8 byte boundary */
struct trTP_CB_s trTi_Task_CB_Tei_64_0;
void trTask_Tei(void *theTaskData);
const struct trTP_CI_s trTi_Task_CI_Tei_64_0 =
{
	&trTi_Task_CB_Tei_64_0, /* ci_cb_p */ 
	trTask_Tei, /* ci_someTask */ 
	NULL, /* ci_theTaskData_p */ 
	(uint8_t *) &trTi_Task_SB_Tei_64_0[0], /* ci_sb_p */ 
	8, /* ci_StackSize */ 
	64 /* ci_NominalPriority */ 
};
const uint16_t trTi_NumOfTasks = 8;
const uint16_t trTi_NumOfTasks_PlusOne = 9;
const struct trTi_T_IB_s trTi_TaskInfoList[8] = /* This is sized trTi_NumOfTasks. */
{
	{
		&trTi_Task_CI_Tei_64_0, /* ib_t_p */
		NULL, /* ib_evt_p Tei has no Evt. */
		NULL, /* ib_msg_p Tei has no Msg. */
		NULL /* ib_strm_p Tei has no Strm. */
	},
	{
		&trTi_Task_CI_Tti_128_1, /* ib_t_p */
		NULL, /* ib_evt_p Tti has no Evt. */
		NULL, /* ib_msg_p Tti has no Msg. */
		NULL /* ib_strm_p Tti has no Strm. */
	},
	{
		&trTi_Task_CI_Test02DDDD_56797_2, /* ib_t_p */
		NULL, /* ib_evt_p Test02DDDD has no Evt. */
		&trTi_Msg_CI_Test02DDDD_56797_2, /* ib_msg_p */
		NULL /* ib_strm_p Test02DDDD has no Strm. */
	},
	{
		&trTi_Task_CI_Test02BBBB_48059_3, /* ib_t_p */
		NULL, /* ib_evt_p Test02BBBB has no Evt. */
		&trTi_Msg_CI_Test02BBBB_48059_3, /* ib_msg_p */
		NULL /* ib_strm_p Test02BBBB has no Strm. */
	},
	{
		&trTi_Task_CI_Test02AAAA_43690_4, /* ib_t_p */
		NULL, /* ib_evt_p Test02AAAA has no Evt. */
		NULL, /* ib_msg_p Test02AAAA has no Msg. */
		NULL /* ib_strm_p Test02AAAA has no Strm. */
	},
	{
		&trTi_Task_CI_Test02CCCC_52428_5, /* ib_t_p */
		NULL, /* ib_evt_p Test02CCCC has no Evt. */
		NULL, /* ib_msg_p Test02CCCC has no Msg. */
		NULL /* ib_strm_p Test02CCCC has no Strm. */
	},
	{
		&trTi_Task_CI_trTst03_action_256_6, /* ib_t_p */
		NULL, /* ib_evt_p trTst03_action has no Evt. */
		NULL, /* ib_msg_p trTst03_action has no Msg. */
		NULL /* ib_strm_p trTst03_action has no Strm. */
	},
	{
		&trTi_Task_CI_trTst03_init_255_7, /* ib_t_p */
		NULL, /* ib_evt_p trTst03_init has no Evt. */
		NULL, /* ib_msg_p trTst03_init has no Msg. */
		NULL /* ib_strm_p trTst03_init has no Strm. */
	}
};
const struct trBidt_Node_const_s trTi_TaskIDTree[9] = /* This is sized trTi_NumOfTasks_PlusOne. */
{
	{
		64, /* the_ID for Tei with idx 0 */
		65535, /* child_left is End */
		1  /* child_right is Tti at this idx. */
	},
	{
		128, /* the_ID for Tti with idx 1 */
		65535, /* child_left is End */
		65535 /* child_right is End */
	},
	{
		56797, /* the_ID for Test02DDDD with idx 2 */
		3, /* child_left is Test02BBBB at this idx. */
		65535 /* child_right is End */
	},
	{
		48059, /* the_ID for Test02BBBB with idx 3 */
		4, /* child_left is Test02AAAA at this idx. */
		5  /* child_right is Test02CCCC at this idx. */
	},
	{
		43690, /* the_ID for Test02AAAA with idx 4 */
		6, /* child_left is trTst03_action at this idx. */
		65535 /* child_right is End */
	},
	{
		52428, /* the_ID for Test02CCCC with idx 5 */
		65535, /* child_left is End */
		65535 /* child_right is End */
	},
	{
		256, /* the_ID for trTst03_action with idx 6 */
		7, /* child_left is trTst03_init at this idx. */
		65535 /* child_right is End */
	},
	{
		255, /* the_ID for trTst03_init with idx 7 */
		65535, /* child_left is End */
		65535 /* child_right is End */
	},
	{
		0, /* the_ID for Root with idx 8 */
		0, /* child_left is Tei at this idx. */
		2  /* child_right is Test02DDDD at this idx. */
	}
};
struct trPl_Lst_s trTi_TaskReadyResrc_pe[8];  /* This is sized trTi_NumOfTasks. */
struct trPl_Lst_s trTi_TaskTimeout_pe[8];  /* This is sized trTi_NumOfTasks. */
const uint16_t trTi_Tti_ID = 128;
const uint16_t trTi_Tti_idx = 1;
const uint16_t trTi_Tei_ID = 64;
const uint16_t trTi_Tei_idx = 0;
