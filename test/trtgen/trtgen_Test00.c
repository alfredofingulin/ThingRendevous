 /** \file
*   \brief ThingRendevous's trtgen Test 00 implementation
*
* Non-executable code intended as initial test for trtgen's output.
* Oddly formatted for a reason: that's part of the test
* (don't inflict indent on this, OK?).
*
* Inflict trtgen on this using the command line:
*
* - python3 [PathToTr]/tool/trtgen.py -t -indir [PathToTr]/src [PathTo]/trtgen_Test00.c
*
* then inspect trtgen's outputs for correctness:
*
* - trTi.c
* - trtgenReport.out
*
* See \link TN07_AllAboutTrTasks \endlink for details.
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
#include "trKIDS.h"
#include "trAPI.h"
#include "trAPIRet.h"

#include "trEvtP.h"
#include "trMsgP.h"
#include "trStrmP.h"
#include "trTP.h"

#include "trTi.h"

//-------------------------------------------------------------------------

#define 	trTask_naxolote  	32  // This keyword is unrecognized and should be ignored.
#define 	trTask_ampilcilan_MsgLen  	32  // This task does not exist and the keywork is recognized; it should be ignored.
#define 	trTask_elmoMatic_eggnog  	32  // This is a valid task but an unrecognized keyword; should be ignored.

#define trTask_Tti_MsgLen 3  // Valid task and keyword, but not allowed for reserved task Tti; ignored.

#define trTask_elmoMatic_ID 512  // Task with nothing else.
#define trTask_elmoMatic_StackSize 512
#define trTask_elmoMatic_NominalPriority 512
#define trTask_elmoMatic_StrmLen 0
#define trTask_elmoMatic_MsgLen 0
#define trTask_elmoMatic_MsgOrdering 0
#define trTask_elmoMatic_MsgPayload 0
#define trTask_elmoMatic_EvtLen 0
	void	trTask_elmoMatic	(void)
	{
	}

#define trTask_dongDong_ID 511  // Task with one stream.
#define trTask_dongDong_StackSize 511  // Stack should be bumped to 512.
#define trTask_dongDong_NominalPriority 511
#define Pashtoon 6  // Appease the compiler for the test below.
#define trTask_dongDong_StrmLen Pashtoon  // Invalid should be ignored.
#undef trTask_dongDong_StrmLen  // Another compiler appeasement.
#define trTask_dongDong_StrmLen 65  // Note stream length.
#define trTask_dongDong_MsgLen 0
#define trTask_dongDong_MsgOrdering 0
#define trTask_dongDong_MsgPayload 0
#define trTask_dongDong_EvtLen 0
	void	trTask_dongDong	(void)  // Valid strm, but no msg, or evt.
	{
	}

#define trTask_weasalHead_ID 513  // Task with a valid message length and payload.
#define trTask_weasalHead_StackSize 513  // Stack should be bumped to 520.
#define trTask_weasalHead_NominalPriority 513
#define trTask_weasalHead_StrmLen 0
#define trTask_weasalHead_MsgLen 3
#define trTask_weasalHead_MsgOrdering 0
#define trTask_weasalHead_MsgPayload 16
#define trTask_weasalHead_EvtLen 0
	void	trTask_weasalHead	(void)  // Valid msg, no strm or evt.
	{
	}

#define trTask_hashOil_ID 510  // Task with eve nts, but payload is valid.
#define trTask_hashOil_StackSize 510  // Stack should be bumped to 512.
#define trTask_hashOil_NominalPriority 510
#define trTask_hashOil_StrmLen 0
#define trTask_hashOil_MsgLen 0
#define trTask_hashOil_MsgOrdering 0
#define trTask_hashOil_MsgPayload 8  // This should be ignored.
#define trTask_hashOil_EvtLen 12  // Has valid evt.
	void	trTask_hashOil	(void)  // No strm or msg, but has evt.
	{
	}
	
#define trTask_BangkokExpress_ID 514  // Task with msg ordered by priority.
#define trTask_BangkokExpress_StackSize 514  // Stack should bump to 520.
#define trTask_BangkokExpress_NominalPriority 514
#define trTask_BangkokExpress_StrmLen 0
#define trTask_BangkokExpress_MsgLen 5  // Valid.
#define trTask_BangkokExpress_MsgOrdering 1  // Valid.
#define trTask_BangkokExpress_MsgPayload 24  // Valid.
#define trTask_BangkokExpress_EvtLen 0
	void	trTask_BangkokExpress	(void)  // Valid msg by priority, nothing else.
	{
	}
	
#define trTask_OldRushAlbum_ID 515  // Task with msg ordered by priority.
#define trTask_OldRushAlbum_StackSize 1023  // Stack should bump to 1024.
#define trTask_OldRushAlbum_NominalPriority 254  // Should be reset to 32767
#define trTask_OldRushAlbum_StrmLen 0
#define trTask_OldRushAlbum_MsgLen 5  // Valid.
#define trTask_OldRushAlbum_MsgOrdering 3  // Invalid; overridden with FIFO.
#define trTask_OldRushAlbum_MsgPayload 32  // Valid.
#define trTask_OldRushAlbum_EvtLen 0
	void	trTask_OldRushAlbum	(void)  // Valid msg by FIFO, nothing else.
	{
	}

	
struct trTi_T_IB_s *trX_test_TaskInfoList;
struct trBidt_Node_s *trX_test_TaskIDTree;
struct trPl_Lst_s *trX_test_TaskReadyResrc_pe;

	
int main ()
{
	trAPI_start();
	trX_test_TaskInfoList = (struct trTi_T_IB_s *) &trTi_TaskInfoList[1];
	trX_test_TaskIDTree = (struct trBidt_Node_s *) &trTi_TaskIDTree[2];
	trX_test_TaskReadyResrc_pe = (struct trPl_Lst_s *) &trTi_TaskReadyResrc_pe[3];
	return 0;
}

