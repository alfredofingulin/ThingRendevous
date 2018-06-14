/** \file
*   \brief ThingRendevous's Message Private interface
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

#ifndef trMsgP_h
#define trMsgP_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/** Message Priority List.
*
* - Every Message Data Block entry corresponds to a Message Priority List entry.
* - The Message Priority List element contains the task priority of the task posting an event.
* - The Message Priotity List's element index corresponds to an Message Data Block entry.
* - This is how the entries are ordered by priority, so the highest priority event is served up.
* - Trtgen names this struct trPl_Lst_s trTi_Msg_PL_SomeName_ID_Idx[ WhateverTheLengthIs ].
*/

/** Message Payload Block
*
* - Located in .bss.
* - An array sized WhateverTheLengthIs * WhateverThePayloadSizeIs
* - The first index corresponds the source ID/flags for this payload (0, 1, 2...)
* - The second index is always the payload size (8, 16, 24, 32).
* - The payload is found at [ message_idx * payload_size ].
* - Trtgen names this trTi_Msg_PL_SomeName_ID_Idx[ WhateverTheLengthIs * WhateverThePayloadSizeIs].
* - It's just uint8_t.
* - I use this 'cause const/unions/2-d arrays/compiler obscure rather than explicate.
*/

/** Message Data Block: Source ID and Flags
*
* - Located in .bss.
* - An array of these forms source and flags info for a message; payload is elsewhere.
* - Trtgen names this trTi_Msg_DB_SomeName_ID_Idx[ WhateverTheLengthIs ].
*/
struct trMsgP_DB_s {
    uint16_t db_source_ID;      /**< Source task ID for this message. */
    uint16_t db_flags;      /**< Presently unused. */
};

/** Message queue Control Block
*
* - Located in .bss.
* - one exists for every task that has an event queue
* - Trtgen names this trTi_Msg_CB_SomeName_ID_Idx
*/
struct trMsgP_CB_s {
    uint16_t cb_len_used;      /**< Messages in the queue at this moment. */
    uint16_t cb_offset_recv;      /**< Zero-based offset in the message queue where data will be received from. */
    uint16_t cb_offset_send;      /**< Zero-based offset in the message queue where data will be sent to. */
    struct trPl_RootEle_s cb_root_ele;      /**< Contains the root element for the priority list elements.  */
};

/** Message queue Config Info
*
* - Located in .txt
* - one exists for every task that has an event queue.
* - Trtgen names this trTi_Msg_CI_SomeName_ID_Idx.
*/
struct trMsgP_CI_s {
    const uint16_t ci_MsgLen;      /**< Number of message packets. */
    const uint16_t ci_MsgOrdering;      /**< 0 for FIFO, 1 for priority; other bits for flags in the future. */
    const uint16_t ci_MsgPayload;      /**< 8, 16, 24, 32. */
    struct trMsgP_CB_s *const ci_cb_p;       /**< Points to the message queue control block in RAM. */
    struct trMsgP_DB_s *const ci_db_p;       /**< Points to the message queue ID/flags blocks. */
    uint8_t *const ci_py_p;       /**< Points to the array containing payload data.  */
    struct trPl_Lst_s *const ci_pl_p;       /**< Points to the priority list that resolves priorities to packet arrays. If FIFO, then NULL, if priority, then Non-NULL. */
};

bool trMsgP_getp(uint16_t forTask_idx,
                 struct trMsgP_CI_s **ci_pp,
                 struct trMsgP_CB_s **cb_pp, struct trMsgP_DB_s **db_pp, uint8_t ** py_pp, struct trPl_Lst_s **pl_pp);

/** Msg queue private Tak Info
*
* Expected by the private Tak Private Section handler
*/
struct trMsgP_TakInfo_s {
    uint16_t ret_val;  /**< Return value. */
    struct trMsgP_CI_s *ci_p;
    struct trMsgP_CB_s *cb_p;
    struct trMsgP_DB_s *db_p;
    uint8_t *py_p;
    struct trPl_Lst_s *pl_p;
};

#define trMsgP_TakInfo_p() ((struct trMsgP_TakInfo_s *)takInfo_s_p)

void trMsgP_takPriQ_PrivSect_hdlr(void *aKIDS_p, void *takInfo_s_p);
void trMsgP_takFIFO_PrivSect_hdlr(void *aKIDS_p, void *takInfo_s_p);

/** Msg queue private Xover Tak Info
*
* Expected by the private Xover Tak Private Section handler
*/
struct trMsgP_XoverTakInfo_s {
    bool ret_val;  /**< Not quite the same return value. */
    struct trMsgP_CI_s *ci_p;
    struct trMsgP_CB_s *cb_p;
    struct trMsgP_DB_s *db_p;
    uint8_t *py_p;
    struct trPl_Lst_s *pl_p;
    uint16_t target_task_idx;
};

#define trMsgP_XoverTakInfo_p() ((struct trMsgP_XoverTakInfo_s *)xoverTakInfo_s_p)

void trMsgP_xoverTakPriQ_PrivSect_hdlr(void *aKIDS_p, void *xoverTakInfo_s_p);
void trMsgP_xoverTakFIFO_PrivSect_hdlr(void *aKIDS_p, void *xoverTakInfo_s_p);

/** Msg queue private Giv Info
*
* Expected by the private Giv Private Section handler
*/
struct trMsgP_GivInfo_s {
    uint16_t ret_val;  /**< Return value. */
    struct trMsgP_CI_s *ci_p;
    struct trMsgP_CB_s *cb_p;
    struct trMsgP_DB_s *db_p;
    uint8_t *py_p;
    struct trPl_Lst_s *pl_p;
    uint16_t thesourceid;  /**< Event source ID. */
    uint16_t thesourcepri;  /**< Event source priority. */
};

#define trMsgP_GivInfo_p() ((struct trMsgP_GivInfo_s *)givInfo_s_p)

void trMsgP_givPriQ_PrivSect_hdlr(void *aKIDS_p, void *givInfo_s_p);
void trMsgP_givFIFO_PrivSect_hdlr(void *aKIDS_p, void *givInfo_s_p);

#endif
