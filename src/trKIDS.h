/** \file
*   \brief ThingRendevous's Kernel Interface Data Structures and function codes.
*
* Here's
*
* \li the defines for kernel request codes
* \li the format for data repacked by the API for a kernel function
* \li some handy macros that type-cast pointers for you.
*
* We try to use names similar to the API to keep things obvious.
*
* <em>A Special Note About Members And Ordering</em>
*
* For every structure defined here, the first three members must be:
*
* \li request code
* \li place for the return value
* \li the task ID index (not the ID!) of the requesting task
*
* We need that because:
*
* \li we're doing a void pointer for a psuedo-generic interface function
* \li we sometime need to make decisions based on those three items
*
* These items need to be in the same place so all the data is contiguous in memory.
*
* <em>A Special Note About Request Codes</em>
*
* Note that kernel request codes should be contiguous.
* These are used as indexes into arrays, so no gaps, please!
*
* This is an interface only; there is no implementation.
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

#ifndef trKIDS_h
#define trKIDS_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

//-------------------------------------------------------------------
#define trKIDS_Start_rq 0
#define trKIDS_SClk_Tick_rq 1
//-------------------------------------------------------------------
#define trKIDS_Strm_Status_rq 2
#define trKIDS_Strm_Writ_rq 3
#define trKIDS_Strm_Put_rq 4
#define trKIDS_Strm_Read_rq 5
#define trKIDS_Strm_Get_rq 6
//-------------------------------------------------------------------
#define trKIDS_Msg_Status_rq 7
#define trKIDS_Msg_Send_rq 8
#define trKIDS_Msg_Giv_rq 9
#define trKIDS_Msg_Recv_rq 10
#define trKIDS_Msg_Tak_rq 11
//-------------------------------------------------------------------
#define trKIDS_Evt_Status_rq 12
#define trKIDS_Evt_Post_rq 13
#define trKIDS_Evt_Stf_rq 14
#define trKIDS_Evt_Pend_rq 15
#define trKIDS_Evt_Grb_rq 16
//-------------------------------------------------------------------
#define trKIDS_Task_Status_rq 17
#define trKIDS_Task_Delay_rq 18
//-------------------------------------------------------------------
#define trKIDS_TI_Enter_srq 19
#define trKIDS_TI_Leave_srq 20
#define trKIDS_EI_Enter_srq 21
#define trKIDS_EI_Leave_srq 22
#define trKIDS_Private_Section_srq 23
//-------------------------------------------------------------------

#define trKIDS_rq_first trKIDS_Start_rq
#define trKIDS_rq_last trKIDS_Task_Delay_rq
#define trKIDS_rq_len (trKIDS_rq_last-trKIDS_rq_first+1)

#define trKIDS_srq_first trKIDS_TI_Enter_srq
#define trKIDS_srq_last trKIDS_Private_Section_srq
#define trKIDS_srq_len (trKIDS_srq_last-trKIDS_srq_first+1)

#define trKIDS_rqlst_len (trKIDS_rq_len+trKIDS_srq_len)

//-------------------------------------------------------------------

struct trKIDS_ComParams_s {
    uint16_t cp_request;
    uint16_t cp_ret_val;
    uint16_t cp_my_task_idx;
    uint16_t cp_destination_target_task;
    void (*cp_kernel_func_hndlr) (void *aKIDS_p);
    uint16_t cp_kernel_func_hndlr_flags;
};
#define trKIDS_ComParams_aKIDS_p() ((struct trKIDS_ComParams_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_ComParams_AntiAlias_s {
    struct trKIDS_ComParams_s cp;
};
#define trKIDS_ComParams_aKIDS_p_AntiAlias() (((struct trKIDS_ComParams_AntiAlias_s *)aKIDS_p)->cp)

//-------------------------------------------------------------------

struct trKIDS_Start_s {
    struct trKIDS_ComParams_s cp;
    uint16_t kstart_notUsed;
};
#define trKIDS_Start_aKIDS_p() ((struct trKIDS_Start_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_SClk_Tick_s {
    struct trKIDS_ComParams_s cp;
};
#define trKIDS_SClk_Tick_aKIDS_p() ((struct trKIDS_SClk_Tick_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_Strm_Status_s {
    struct trKIDS_ComParams_s cp;
    struct trAPIRet_StrmStatus_s *kss_stat_p;
};
#define trKIDS_Strm_Status_aKIDS_p() ((struct trKIDS_Strm_Status_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_Strm_PutWrit_s {
    struct trKIDS_ComParams_s cp;
    uint16_t kspw_min_bytes;
    uint16_t kspw_max_bytes;
    uint8_t *kspw_buff_p;
    uint16_t *kspw_actual_bytes_p;
    uint16_t *kspw_avail_bytes_p;
};
#define trKIDS_Strm_PutWrit_aKIDS_p() ((struct trKIDS_Strm_PutWrit_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_Strm_GetRead_s {
    struct trKIDS_ComParams_s cp;
    uint16_t ksgr_min_bytes;
    uint16_t ksgr_max_bytes;
    uint8_t *ksgr_buff_p;
    uint16_t *ksgr_actual_bytes_p;
    uint16_t *ksgr_avail_bytes_p;
};
#define trKIDS_Strm_GetRead_aKIDS_p() ((struct trKIDS_Strm_GetRead_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_Msg_Status_s {
    struct trKIDS_ComParams_s cp;
    struct trAPIRet_MsgStatus_s *kms_stat_p;
};
#define trKIDS_Msg_Status_aKIDS_p() ((struct trKIDS_Msg_Status_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_Msg_GivSend_s {
    struct trKIDS_ComParams_s cp;
    uint16_t kmgs_flags;
    uint8_t *kmgs_payload_p;
};
#define trKIDS_Msg_GivSend_aKIDS_p() ((struct trKIDS_Msg_GivSend_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_Msg_TakRecv_s {
    struct trKIDS_ComParams_s cp;
    uint16_t *kmtr_source_p;
    uint16_t *kmtr_flags_p;
    uint8_t *kmtr_payload_p;
};
#define trKIDS_Msg_TakRecv_aKIDS_p() ((struct trKIDS_Msg_TakRecv_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_Evt_Status_s {
    struct trKIDS_ComParams_s cp;
    struct trAPIRet_EvtStatus_s *kes_stat_p;
};
#define trKIDS_Evt_Status_aKIDS_p() ((struct trKIDS_Evt_Status_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_Evt_StfPost_s {
    struct trKIDS_ComParams_s cp;
    uint16_t kesp_flags;
    uint32_t kesp_event;
};
#define trKIDS_Evt_StfPost_aKIDS_p() ((struct trKIDS_Evt_StfPost_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_Evt_GrbPend_s {
    struct trKIDS_ComParams_s cp;
    uint16_t *kegp_source_p;
    uint16_t *kegp_flags_p;
    uint32_t *kegp_event_p;
};
#define trKIDS_Evt_GrbPend_aKIDS_p() ((struct trKIDS_Evt_GrbPend_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_Task_Status_s {
    struct trKIDS_ComParams_s cp;
    struct trAPIRet_TaskStatus_s *kts_stat_p;
};
#define trKIDS_Task_Status_aKIDS_p() ((struct trKIDS_Task_Status_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_Task_Delay_s {
    struct trKIDS_ComParams_s cp;
    uint16_t ktd_how_long;
};
#define trKIDS_Task_Delay_aKIDS_p() ((struct trKIDS_Task_Delay_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_TI_Enter_s {
    struct trKIDS_ComParams_s cp;
    uint16_t kte_NotUsed_CanBeAnyKIDS;
};
#define trKIDS_TI_Enter_aKIDS_p() ((struct trKIDS_TI_Enter_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_TI_Leave_s {
    struct trKIDS_ComParams_s cp;
    uint16_t ktl_NotUsed_CanBeAnyKIDS;
};
#define trKIDS_TI_Leave_aKIDS_p() ((struct trKIDS_TI_Leave_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_EI_Enter_s {
    struct trKIDS_ComParams_s cp;
    uint16_t kee_NotUsed;
};
#define trKIDS_EI_Enter_aKIDS_p() ((struct trKIDS_EI_Enter_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_EI_Leave_s {
    struct trKIDS_ComParams_s cp;
    uint16_t kel_NotUsed;
};
#define trKIDS_EI_Leave_aKIDS_p() ((struct trKIDS_EI_Leave_s *)aKIDS_p)

//-------------------------------------------------------------------

struct trKIDS_Private_Section_s {
    struct trKIDS_ComParams_s cp;
    uint16_t kps_NotUsed;
};
#define trKIDS_Private_Section_aKIDS_p() ((struct trKIDS_Private_Section_s *)aKIDS_p)

#endif
