/** \file
*   \brief ThingRendevous's Bounds interface (no implementation)
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

#ifndef trB_h
#define trB_h

#define trB_Any 0
#define trB_None 0xFFFFU

#define trB_ID_Self 0
#define trB_ID_Any 0
#define trB_ID_None 0xFFFFU

#define trB_TrUsed_ID_first 1U
#define trB_TrUsed_ID_last 0x00FEU

#define trB_TrUsed_Pri_first 1U
#define trB_TrUsed_Pri_last 0x00FEU
#define trB_TrUsed_Pri_highest trB_TrUsed_Pri_first
#define trB_TrUsed_Pri_lowest trB_TrUsed_Pri_last

#define trB_TrAppUsed_Pri_first 0x00FFU
#define trB_TrAppUsed_Pri_last 0x00FFU
#define trB_TrAppUsed_Pri_highest trB_TrAppUsed_Pri_first
#define trB_TrAppUsed_Pri_lowest trB_TrAppUsed_Pri_last

#define trB_FixedPri_ID_first 1U
#define trB_FixedPri_ID_last 0x00FFU
#define trB_FixedPri_Pri_first 1U
#define trB_FixedPri_Pri_last 0x00FFU
#define trB_FixedPri_Pri_highest trB_FixedPri_Pri_first
#define trB_FixedPri_Pri_lowest trB_FixedPri_Pri_last

#define trB_TrUsedAppUsed_ID_boundary 0x00FFU

#define trB_AppUsed_ID_first 0x0100U
#define trB_AppUsed_ID_last 0xFFFEU

#define trB_AppUsed_Pri_first 0x0100U
#define trB_AppUsed_Pri_last 0xFFFEU
#define trB_AppUsed_Pri_highest trB_AppUsed_Pri_first
#define trB_AppUsed_Pri_lowest trB_AppUsed_Pri_last

#define trB_Tr_ExIntr_ID 64U
#define trB_Tr_ExIntr_Pri 64U
#define trB_Tr_TrItself_ID 128U
#define trB_Tr_TrItself_Pri 128U

#endif
