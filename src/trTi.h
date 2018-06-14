/** \file
*   \brief ThingRendevous's Task Info interface 
*
* This defines the task info data expected by the kernel.
* Actual content is generated by tool Trtgen.
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

#ifndef trTi_h
#define trTi_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "trBidt.h"
#include "trPl.h"
#include "trEvtP.h"
#include "trMsgP.h"
#include "trStrmP.h"
#include "trTP.h"

//-------------------------------------------------------------------------

/** Task Info Block.  Every task has one of these.
*/
struct trTi_T_IB_s {
    const struct trTP_CI_s *const ib_t_p;       /**< Points to the config info for a task.  It is always present. */
    const struct trEvtP_CI_s *const ib_evt_p;       /**< Points to the config info for a task's events; NULL if task doesn't handle events. */
    const struct trMsgP_CI_s *const ib_msg_p;       /**< Points to the config info for a task's message; NULL if task doesn't handle messages. */
    const struct trStrmP_CI_s *const ib_strm_p;       /**< Points to the config info for a task's streams; NULL if task doesn't handle streams. */
};

//-------------------------------------------------------------------------

/** The total number of tasks in this system.  Used to size all other data structures.
*/
extern const uint16_t trTi_NumOfTasks;

/** The total number of tasks in this system + 1.  Sizes the binary ID tree.
*/
extern const uint16_t trTi_NumOfTasks_PlusOne;

//-------------------------------------------------------------------------

/** The list of tasks and all there stream, message, and event info.  Every into this table corresponds the ID in the binary ID table.  Has size trTi_NumOfTasks.
*/
extern const struct trTi_T_IB_s trTi_TaskInfoList[];

//-------------------------------------------------------------------------

/** The binary tree of IDs.  Every index into this table corresponds to task information in another table.  Has size trTi_NumOfTasks_PlusOne; note last element is the head of the binary tree.
*/
extern const struct trBidt_Node_const_s trTi_TaskIDTree[];

//-------------------------------------------------------------------------

/** Every task get a ready/resource priority linked list element.
*/
extern struct trPl_Lst_s trTi_TaskReadyResrc_pe[];

/** Every task get a timeout priority linked list element.
*/
extern struct trPl_Lst_s trTi_TaskTimeout_pe[];

//-------------------------------------------------------------------------

/** The ID for tr's task itself; so we don't have to look it up all the time.
*/
extern const uint16_t trTi_Tti_ID;

/** The index to tr's task itself; so we don't have to look it up all the time.
*/
extern const uint16_t trTi_Tti_idx;

/** The ID to tr's explicit interrupt task; so we don't have to look it up all the time.
*/
extern const uint16_t trTi_Tei_ID;

/** The index to tr's explicit interrupt task; so we don't have to look it up all the time.
*/
extern const uint16_t trTi_Tei_idx;

//-------------------------------------------------------------------------

/** There is no init.
*/

#endif