/** \file
*   \brief ThingRendevous's Tr Task Itself interface 
*
* Note the following are undefined, and forced to these values in trtgen.py:
*
* \li trTask_Tti_ID 128
* \li trTask_Tti_Priority 128
*
* You can change those, and trtgen will go along with it,
* but it violates the architecture.
*
* You can change these, but as trTask_Tti is coded, these won't work anyway:
*
* \li trTask_Tti_theTaskData NULL
* \li trTask_Tti_StrmLen 0
* \li trTask_Tti_MsgLen 0
* \li trTask_Tti_EvtLen 0
*
* Changing those is dumb idea.
*
* Changing the stack size is fair game; it's the same default as forced in trtgen.py;
* change it here to change the stack size.
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

#ifndef trTti_h
#define trTti_h

#include "trKIDS.h"
#include "trTi.h"
#include "trM.h"

uint16_t trTti_enter(void *aKIDS_p);
void trTti_leave(void *aKIDS_p);

#define trTask_Tti_StackSize 1024

void trTask_Tti(void *theTaskData_p);

void trTti_init(void);

#endif
