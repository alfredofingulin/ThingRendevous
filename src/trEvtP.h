/** \file
*   \brief ThingRendevous's Event Private interface
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

#ifndef trEvtP_h
#define trEvtP_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "trPl.h"

/** Event Priority List.
*
* - Every Event Data Block entry corresponds to a Event Priority List entry.
* - The Event Priority List element contains the task priority of the task posting an event.
* - The Event Priotity List's element index corresponds to an Event Data Block entry.
* - This is how the entries are ordered by priority, so the highest priority event is served up.
* - Trtgen names this struct trPl_Lst_s trTi_Evt_PL_SomeName_ID_Idx[ WhateverTheLengthIs ].
*/

/** Event Data Block.
*
* - Located in .bss.
* - An array of these forms a task's event queue.
* - Trtgen names this trTi_Evt_DB_SomeName_ID_Idx[ WhateverTheLengthIs ].
*/
struct trEvtP_DB_s {
    uint16_t db_source_ID;      /**< Source task ID for this event. */
    uint16_t db_flags;      /**< Presently unused. */
    uint32_t db_evt;      /**< The 32-bit event itself. */
};

/** Event queue Control Block
*
* - Located in .bss.
* - one exists for every task that has an event queue
* - Trtgen names this trTi_Evt_CB_SomeName_ID_Idx
*/
struct trEvtP_CB_s {
    uint16_t cb_len_used;      /**< Event packets in the queue at this moment. */
    struct trPl_RootEle_s cb_root_ele;      /**< Contains the root element for the priority list elements.  */
};

/** Event queue Config Info
*
* - Located in .txt
* - one exists for every task that has an event queue.
* - Trtgen names this trTi_Evt_CI_SomeName_ID_Idx.
*/
struct trEvtP_CI_s {
    const uint16_t ci_EvtLen;      /**< Number of event packets. */
    struct trEvtP_CB_s *const ci_cb_p;       /**< Points to the event queue control block in RAM. */
    struct trEvtP_DB_s *const ci_db_p;       /**< Points to the event queue packet array in RAM. */
    struct trPl_Lst_s *const ci_pl_p;       /**< Points to the priority list that resolves priorities to packet arrays. */
};

bool trEvtP_getp(uint16_t forTask_idx,
                 struct trEvtP_CI_s **ci_pp,
                 struct trEvtP_CB_s **cb_pp, struct trEvtP_DB_s **db_pp, struct trPl_Lst_s **pl_pp);

/** Event queue private Grab Info
*
* Expected by the private Grab Private Section handler
*/
struct trEvtP_GrabInfo_s {
    uint16_t ret_val;  /**< Return value. */
    struct trEvtP_CI_s *ci_p;
    struct trEvtP_CB_s *cb_p;
    struct trEvtP_DB_s *db_p;
    struct trPl_Lst_s *pl_p;
};

#define trEvtP_GrabInfo_p() ((struct trEvtP_GrabInfo_s *)grabInfo_s_p)

void trEvtP_grab_PrivSect_hdlr(void *aKIDS_p, void *grabInfo_s_p);

/** Event queue private Xover Grab Info
*
* Expected by the private Xover Grab Private Section handler
*/
struct trEvtP_XoverGrabInfo_s {
    bool ret_val;  /**< Not quite the same return value. */
    // note lack of trEvtP_CI_s
    struct trEvtP_CB_s *cb_p;
    struct trEvtP_DB_s *db_p;
    struct trPl_Lst_s *pl_p;
    uint16_t target_task_idx;
};

#define trEvtP_XoverGrabInfo_p() ((struct trEvtP_XoverGrabInfo_s *)xoverGrabInfo_s_p)

void trEvtP_xoverGrab_PrivSect_hdlr(void *aKIDS_p, void *xoverGrabInfo_s_p);

/** Event queue private Stuf Info
*
* Expected by the private Stuf Private Section handler
*/
struct trEvtP_StufInfo_s {
    uint16_t ret_val;  /**< Return value. */
    struct trEvtP_CI_s *ci_p;
    struct trEvtP_CB_s *cb_p;
    struct trEvtP_DB_s *db_p;
    struct trPl_Lst_s *pl_p;
    uint16_t thesourceid;  /**< Event source ID. */
    uint16_t thesourcepri;  /**< Event source priority. */
};

#define trEvtP_StufInfo_p() ((struct trEvtP_StufInfo_s *)stufInfo_s_p)

void trEvtP_stuf_PrivSect_hdlr(void *aKIDS_p, void *stufInfo_s_p);

#endif
