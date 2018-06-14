/** \file
*   \brief ThingRendevous's Kernel Handler Finder interface.
*
* Helps find a specific kernel handler, based on the KIDS.
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

#ifndef trKhf_h
#define trKhf_h

#include "trKIDS.h"

/** Flag indicates the request can use the callers stack.
*/
#define trKhf_Flags_RunOnCaller 0x01

/** Flag suppresses task switching on request completion.
*/
#define trKhf_Flags_DontPreEmpt 0x02

void trKhf_find(void *aKIDS_p, void (**the_kernel_func_hdlr) (void *aKIDS_p), uint16_t * the_kernel_func_hdlr_flags_p);

/** Test to see if aKIDS_p is configured to run on the caller.
*/
#define trKhf_runOnCaller(aKIDS_p) ( ((((struct trKIDS_ComParams_AntiAlias_s *)aKIDS_p)->cp).cp_kernel_func_hndlr_flags & trKhf_Flags_RunOnCaller) > 0 )

#endif
