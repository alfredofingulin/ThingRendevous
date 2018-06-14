/** \file
*   \brief ThingRendevous's Trusted State Transition Coordinator interface
*
* These move tasks from one state to another, 
* updating relevant data structures along the way.
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

#ifndef trTstc_h
#define trTstc_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "trTS.h"

/** Suspend Resource Info
*
* Expected by the Suspend Resource Private Section handler
*/
struct trTstc_Susp_Resc_Info_s {
    uint16_t task_idx;  /**< target of the resource suspend */
    uint16_t task_susp_state;  /**< suspend state to be added */
};

#define trTstc_Susp_Resc_Info_p() ((struct trTstc_Susp_Resc_Info_s *)susp_Resc_Info_s_p)

void trTstc_susp_Resc_PrivSect_hdlr(void *notused, void *susp_Resc_Info_s_p);

/** make Ready Resource Info
*
* Expected by the make Ready Resource Private Section handler
*/
struct trTstc_Ready_Resc_Info_s {
    uint16_t task_idx;  /**< target of the ready from resource */
    uint16_t task_susp_state;  /**< what we're making ready from */
};

#define trTstc_Ready_Resc_Info_p() ((struct trTstc_Ready_Resc_Info_s *)ready_Resc_Info_s_p)

void trTstc_ready_Resc_PrivSect_hdlr(void *notused, void *ready_Resc_Info_s_p);

/** Suspend Timeout Info
*
* Expected by the Suspend for Timeout Private Section handler
*/
struct trTstc_Susp_Timo_Info_s {
    uint16_t task_idx;  /**< target of the suspend for timeout */
    uint16_t timo_val;                /**< How long to suspend timeout, in sysclock ticks. */
};

#define trTstc_Susp_Timo_Info_p() ((struct trTstc_Susp_Timo_Info_s *)susp_Timo_Info_s_p)

void trTstc_susp_Timo_PrivSect_hdlr(void *notused, void *susp_Timo_Info_s_p);

void trTstc_ready_Timo(uint16_t task_idx);

void trTstc_ready_Timo_PrivSect_hdlr(void *notused, void *susp_Timo_Info_s_p);

void trTstc_beginInServ(void *aKIDS_p);
void trTstc_completedInServ(void *aKIDS_p);
void trTstc_endInServ(void *aKIDS_p);

void trTstc_init(void);

#endif
