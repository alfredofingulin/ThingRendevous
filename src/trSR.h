/** \file
*   \brief ThingRendevous's State Registers interface
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

#ifndef trSR_h
#define trSR_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "trPl.h"

extern uint16_t trSR_Flags;
#define trSR_Flags_reserved 0x7FFF
#define trSR_Flags_Go_bit 0x8000
#define trSR_Flags_going() ((trSR_Flags & trSR_Flags_Go_bit) == trSR_Flags_Go_bit)

extern uint16_t trSR_RunningTask_idx;
extern struct trPl_RootEle_s trSR_ReadyTasks_pl;
extern struct trPl_RootEle_s trSR_TimeoutTasks_pl;

extern uint8_t trSR_ExIntrNest;
extern uint8_t trSR_TrItselfNest;

void trSR_init(void);

#endif
