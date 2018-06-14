/** \file
*   \brief ThingRendevous's State Registers implementation
*
* These register the current state of ThingRendevous operations.
*
* \author Alfred O Fingulin FinguPublic@gmail.com
*/
/*
* MIT License
* 
* Copyright (c) 2017 Alfred O Fingulin  FinguPublic@gmail.com
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
#include "trPl.h"
// Did not include the header.

/** We always need a flags register for something.  No helpers 'cause changes should stick out like a sore thumb.
*
* At syste start this should be zero, aka not-going.
*/
uint16_t trSR_Flags;

/** Contains the index to the currently running task.
*
* At sys start this should be zero, aka not-intialized.
*/
uint16_t trSR_RunningTask_idx;

/** Here's the priority list for tasks ready-to-run.  It uses elements from trTi_TaskReadyResrc_pe.
*/
struct trPl_RootEle_s trSR_ReadyTasks_pl;

/** Here's the priority list for tasks timing-out for some reason.  It uses elements from trTi_TaskTimeout_pe.
*/
struct trPl_RootEle_s trSR_TimeoutTasks_pl;

/** How many times explicit interrupts have been nested.
*/
uint8_t trSR_ExIntrNest;

/** How many times Tr Itself has been nested.
*/
uint8_t trSR_TrItselfNest;

/**
*
*/
void trSR_init(void)
{
    trSR_Flags = 0;
    trSR_RunningTask_idx = trB_None;
    trPl_initRootEle(&trSR_ReadyTasks_pl);
    trPl_initRootEle(&trSR_TimeoutTasks_pl);
    trSR_ExIntrNest = 0;
    trSR_TrItselfNest = 0;
}
