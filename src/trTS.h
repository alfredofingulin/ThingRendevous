/** \file
*   \brief ThingRendevous's Task State interface (no implementation)
*
* Here's what's packed into each task's state variable.
*
* ThingRendevous Task States
*
* Non-initialized
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

#ifndef trTS_h
#define trTS_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/** The lowest bit is being serviced by Task Tr Itself or Tr's Task Explicit Interrupt.
*
* \li Everything we need is on the calling task's stack.
* \li Everything is transistioning for this task.
* \li Only trTti or trTei enter/leave can set/clear this bit.
*/
#define trTS_TaskInServ_bits 0x0001

#define trTS_TaskInServ_get(x) ( x & trTS_TaskInServ_bits )
#define trTS_TaskInServ_set(x,y) ( x = ( ( y & trTS_TaskInServ_bits ) | ( x & ~trTS_TaskInServ_bits ) ) )
#define trTS_TaskInServ_set_st(x) ( x = x | trTS_TaskInServ_bits )
#define trTS_TaskInServ_clr_st(x) ( x = x & ~trTS_TaskInServ_bits )
#define trTS_TaskInServ_tst(x,y) ( ( x & trTS_TaskInServ_bits ) == y )

/** The next three bits encode the task state.
*/
#define trTS_TaskState_bits 0x000E

#define trTS_TaskState_get(x) ( x & trTS_TaskState_bits )
#define trTS_TaskState_set(x,y) ( x = ( ( y & trTS_TaskState_bits ) | ( x & ~trTS_TaskState_bits ) ) )
#define trTS_TaskState_tst(x,y) ( ( x & trTS_TaskState_bits ) == y )

/** Everything is in flux.
*/
#define trTS_TaskState_InFlux 0

/** The one-and-only task that is running.
*/
#define trTS_TaskState_Running 0x0002

/** This task's is ready to run.
*
* The ready/resource priority element trTi_TaskReadyResrc_pe is on the prioritized ready tasks list trSR_ReadyTasks_pl.
* 
* Otherwise the task is suspended or timing-out.
* The ready/resource priority element trTi_TaskReadyResrc_pe is floating 
* and is_not_ on the prioritized ready tasks list trSR_ReadyTasks_pl.
*/
#define trTS_TaskState_Ready 0x0004

/** The task has been suspended for some reason.
*/
#define trTS_TaskState_Suspended 0x0006

#define trTS_TaskState_reserved_8 0x0008
#define trTS_TaskState_reserved_A 0x000A
#define trTS_TaskState_reserved_C 0x000C

/** Used only by Tti/Tei: Just what it says.
*/
#define trTS_TaskState_AwaitingProgrammaticActivation 0x000E

/** The next four bits encode the task's suspend state.
*/
#define trTS_Susp_bits 0x00F0

#define trTS_Susp_get(x) ( x & trTS_Susp_bits )
#define trTS_Susp_set(x,y) ( x = ( ( y & trTS_Susp_bits ) | ( x & ~trTS_Susp_bits ) ) )
#define trTS_Susp_tst(x,y) ( ( ( x & trTS_TaskState_bits ) == trTS_TaskState_Suspended ) && ( ( x & trTS_Susp_bits & y ) == y ) )
#define trTS_Susp_tst_any(x,y) ( ( ( x & trTS_TaskState_bits ) == trTS_TaskState_Suspended ) && ( ( x & trTS_Susp_bits & y ) > 0 ) )
#define trTS_Susp_set_st(x,y) ( x = ( ( x & ~trTS_Susp_bits ) | ( ( x & trTS_Susp_bits ) | ( y & trTS_Susp_bits ) ) ) )
#define trTS_Susp_clr_st(x,y) ( x = ( ( x & ~trTS_Susp_bits ) | ( ( x & trTS_Susp_bits ) & ~( y & trTS_Susp_bits ) ) ) )

/** The task is suspened for timing out.  
*
* Can occur with any other suspend reason.  
*
* If the task's delay remaining value in the task's cb_task_delay_remaining
* is 0xFFFF, then the task is being suspended indefinately.
* This task's timeout priority element trTi_TaskTimeout_pe is floating
* and is _not_ on the timeout prioritized list trSR_TimeoutTasks_pl.  
*
* If the task's delay remaining value in the task's cb_task_delay_remaining
* is 0xFFFE..Ox0001, then the task is timing-out.
* This task's timeout priority element trTi_TaskTimeout_pe 
* and _is_ on the timeout prioritized list trSR_TimeoutTasks_pl.  
*
* If the task's delay remaining value in the task's cb_task_delay_remaining
* rolls from 0x0001 to 0, or is 0, then the timeout is complete.
* This task's timeout priority element trTi_TaskTimeout_pe is floating
* and is _not_ on the timeout prioritized list trSR_TimeoutTasks_pl.  
* Note the task may still be suspended for event/message/stream.
*/
#define trTS_Susp_ForDelay 0x0080

/** The task is suspended on a task's event.  
*
* This excludes suspension on messages or streams, 
* but future planning should make no such assumption.
*/
#define trTS_Susp_ForEvt 0x0010

/** The task is suspened on a task's message.
*
* This excludes suspension on events or streams, 
* but future planning should make no such assumption.
*/
#define trTS_Susp_ForMsg 0x0020

/** The task is suspened on a task's stream.  
*
* This excludes suspension on events or messages, 
* but future planning should make no such assumption.
*/
#define trTS_Susp_ForStrm 0x0040

// No initializer; you gotta do this.

#endif
