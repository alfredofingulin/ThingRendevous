/** \file
*   \brief ThingRendevous's Application Program Interface implementation.
*
* Packs all API calls into kernel data structures,
* then traps to the kernel.
*
* Note explicit interrupt enter/leave are not defined here;
* use trAPIEI.h.
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
#include "trKhf.h"
#include "trTti.h"
#include "trAPIRet.h"
#include "trSR.h"
#include "trStart.h"
#include "trAPI.h"

/** Private utility to gather data used for all API calls into KIDS.
*
* Pre-loads the return valid with in-progress.
* Grabs the task ID val.
* Kernel function handler too.
*/
void trAPI_CP_gather(void *aKIDS_p,
                        /**< KIDS being built for this API call. */
                     uint16_t kernel_request,
                                  /**< Unique request number. */
                     uint16_t destin_targ_task_ID
                                      /**< And that's the ID not the idx! */
    )
{
    trKIDS_ComParams_aKIDS_p_AntiAlias().cp_request = kernel_request;
    trKIDS_ComParams_aKIDS_p_AntiAlias().cp_ret_val = trAPIRet_Gen_InProcess;
    trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx = trSR_RunningTask_idx;
    trKIDS_ComParams_aKIDS_p_AntiAlias().cp_destination_target_task = destin_targ_task_ID;
    trKhf_find(aKIDS_p, &(trKIDS_ComParams_aKIDS_p_AntiAlias().cp_kernel_func_hndlr),
               &(trKIDS_ComParams_aKIDS_p_AntiAlias().cp_kernel_func_hndlr_flags));
}

/** Tr's entry point.
*
* When invoked, it inherits the stack set-up, initializes everything,
* starts Tr task, and lets your app have at-it.
*
* Intended to be called from main().
* You must be running on the main stack pointer, not the PSP.
*
* Can really be called anytime to restart tr.  
* Just make sure you've got your hardware initialization 
* and stack set-up where you want it (see cavet above).
*
*
*
* \retval trAPIRet_Gen_NoReturnExpected
*/
uint16_t trAPI_start(void)
{
    trStart_start();
    return trAPIRet_Gen_NoReturnExpected;
}

/** Inform Tr that a clock tick has occured; reschedule and pre-empt as necessary.
*
* Must be called from within an explicit interrupt or a task.
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_SClk_tick(void)
{
    struct trKIDS_SClk_Tick_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_SClk_Tick_rq, trAPI_TaskID_self);
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Report stream status.
*
* This does not reschedule or task switch.
*
* All the action takes place on the calling task's thread.
*
* No sync is done; the data may be stale or changing.
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Strm_status(uint16_t target,
                          /**< Task ID for the stream being stat-ed. */
                           struct trAPIRet_StrmStatus_s * stat_p
                                              /**< Where to put the status info. */
    )
{
    struct trKIDS_Strm_Status_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Strm_Status_rq, target);
    my_rq.kss_stat_p = stat_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Write to a task's input stream; reschedule and maybe pre-empt afte doing so.
*
* If there's room for the minimum number of bytes specifed, 
* adds the bytes to the destination task stream.
* Never adds more than the maximum number of bytes specified.
* Otherwise, serves up an error code saying there's no room.
*
* Rescheduling and task switching may occur.
*
* Note that:
*
* \li 0 < min_bytes <= max_bytes < uint16_t max
* \li 0 and uint16_t max are considered errors
* \li no errors returned if min_bytes <= actual_bytes <= max_bytes
* \li if min_bytes == max_bytes than the exact amount will be made available
*
* and:
*
* \li actual_bytes is how many bytes actually added to the stream
* \li avail_bytes is how many bytes can still be added to the stream
*
* A memcpy() will be used to copy the bytes.
*
* This should not be called from an explicit interrupt.
* Doing so will yield an error.
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Strm_writ(uint16_t destination,
                               /**< The destination task ID for the stream write.  Self is valid. */
                         uint16_t min_bytes,
                             /**< The minimum number of bytes to write. */
                         uint16_t max_bytes,
                             /**< The maximum number of bytes to write. */
                         uint8_t * buff_p,
                          /**< Pointer to a buffer containing the bytes.  */
                         uint16_t * actual_bytes_p,
                                   /**< Where the actual number of bytes written will be returned.    */
                         uint16_t * avail_bytes_p
                                 /**< Where the number of bytes available in the stream will be served up; 0 means nothing in the stream.    */
    ) {
    struct trKIDS_Strm_PutWrit_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Strm_Writ_rq, destination);
    my_rq.kspw_min_bytes = min_bytes;
    my_rq.kspw_max_bytes = max_bytes;
    my_rq.kspw_buff_p = buff_p;
    my_rq.kspw_actual_bytes_p = actual_bytes_p;
    my_rq.kspw_avail_bytes_p = avail_bytes_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Put bytes to a task's input stream without pre-empting.
*
* Just like write, but this will not cause a task switch; 
* the calling task retains control.
* Rescheduling will still occur.
*
* This may be called from an explicit interrupt,
* but doing so with task ID self or explicit interrupt is an error.
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Strm_put(uint16_t destination,
                               /**< The destination task ID for the stream write.  Self is valid. */
                        uint16_t min_bytes,
                             /**< The minimum number of bytes to write. */
                        uint16_t max_bytes,
                             /**< The maximum number of bytes to write. */
                        uint8_t * buff_p,
                          /**< Pointer to a buffer containing the bytes.  */
                        uint16_t * actual_bytes_p,
                                   /**< Where the actual number of bytes written will be returned.    */
                        uint16_t * avail_bytes_p
                                 /**< Where the number of bytes available in the stream will be served up; 0 means nothing in the stream.    */
    ) {
    struct trKIDS_Strm_PutWrit_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Strm_Put_rq, destination);
    my_rq.kspw_min_bytes = min_bytes;
    my_rq.kspw_max_bytes = max_bytes;
    my_rq.kspw_buff_p = buff_p;
    my_rq.kspw_actual_bytes_p = actual_bytes_p;
    my_rq.kspw_avail_bytes_p = avail_bytes_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Read from the calling task's (self) input stream; reschedule and maybe pre-empt if nothing is available.
*
* If there's at least the minimum number of bytes requested in the stream, serves 'em up.
* Never serves up more than the maximum requested.
*
* Reschedule, maybe pre-empt, and maybe run something else.
*
* Note that:
*
* \li 0 < min_bytes <= max_bytes < uint16_t max
* \li 0 and uint16_t max are considered errors
* \li no errors returned if min_bytes <= actual_bytes <= max_bytes
* \li if min_bytes == max_bytes than the exact amount will be made available
*
* and:
*
* \li actual_bytes is how many bytes actually taken from the stream
* \li avail_bytes is how many bytes remain in the stream.
*
* A memcpy() will be used to copy the bytes.
*
* This should not be called from an explicit interrupt.
* Doing so will yield an error.
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Strm_read(uint16_t min_bytes,
                             /**< The minimum number of bytes to read. */
                         uint16_t max_bytes,
                             /**< The maximum number of bytes to read. */
                         uint8_t * buff_p,
                          /**< Pointer to a buffer to store the bytes.  */
                         uint16_t * actual_bytes_p,
                                   /**< Where the actual number of bytes read will be returned. */
                         uint16_t * avail_bytes_p
                                 /**< Where the number of bytes available in the stream will be served up; 0 means nothing in the stream.  */
    ) {
    struct trKIDS_Strm_GetRead_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Strm_Read_rq, trAPI_TaskID_self);
    my_rq.ksgr_min_bytes = min_bytes;
    my_rq.ksgr_max_bytes = max_bytes;
    my_rq.ksgr_buff_p = buff_p;
    my_rq.ksgr_actual_bytes_p = actual_bytes_p;
    my_rq.ksgr_avail_bytes_p = avail_bytes_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Get bytes from the calling task's (self) input stream without pre-empting.
*
* Just like read, but this will not cause a task switch; 
* the calling task retains control.
* Rescheduling will still occur.
*
* This should not be called from an explicit interrupt.
* Doing so will yield an error.
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Strm_get(uint16_t min_bytes,
                             /**< The minimum number of bytes to get. */
                        uint16_t max_bytes,
                             /**< The maximum number of bytes to get. */
                        uint8_t * buff_p,
                          /**< Pointer to a buffer to store the bytes.  */
                        uint16_t * actual_bytes_p,
                                   /**< Where the actual number of bytes gotten will be returned. */
                        uint16_t * avail_bytes_p
                                 /**< Where the number of bytes available in the stream will be served up; 0 means nothing in the stream.  */
    ) {
    struct trKIDS_Strm_GetRead_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Strm_Get_rq, trAPI_TaskID_self);
    my_rq.ksgr_min_bytes = min_bytes;
    my_rq.ksgr_max_bytes = max_bytes;
    my_rq.ksgr_buff_p = buff_p;
    my_rq.ksgr_actual_bytes_p = actual_bytes_p;
    my_rq.ksgr_avail_bytes_p = avail_bytes_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Report message status.
*
* This does not reschedule or task switch.
*
* All the action takes place on the calling task's thread.
*
* No sync is done; the data may be stale or changing.
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Msg_status(uint16_t target,
                          /**< Task ID for the message queue being stat-ed. */
                          struct trAPIRet_MsgStatus_s * stat_p
                                             /**< Where to put the status info. */
    )
{
    struct trKIDS_Msg_Status_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Msg_Status_rq, target);
    my_rq.kms_stat_p = stat_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Send a message to a task; reschedule and maybe pre-empt after doing so.
*
* If there's room for the message, send it on.
* Never adds more than the number of payload bytes expected by the destination task.
* Otherwise, serve up an error code saying there's no room.
*
* Rescheduling and task switching may occur.
*
* A memcpy() will be used to copy the payload.
*
* This should not be called from an explicit interrupt.
* Doing so will yield an error.
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Msg_send(uint16_t destination,
                               /**< The destination task ID for the message send.  Self is valid. */
                        uint16_t flags,
                         /**< Not presently used; set it to zero.  */
                        uint8_t * payload_p
                            /**< Pointer to a buffer containing the payload bytes.  Only the number of bytes expected by the destination are copied.  */
    ) {
    struct trKIDS_Msg_GivSend_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Msg_Send_rq, destination);
    my_rq.kmgs_flags = flags;
    my_rq.kmgs_payload_p = payload_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Give a message to a task without pre-empting.
*
* Just like send, but this will not cause a task switch;
* the calling task retains control.
* Rescheduling will still occur.
*
* This may be called from an explicit interrupt,
* but doing so with task ID self or explicit interrupt is an error.
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Msg_giv(uint16_t destination,
                               /**< The destination task ID for the message send.  Self is valid. */
                       uint16_t flags,
                         /**< Not presently used; set it to zero.  */
                       uint8_t * payload_p
                            /**< Pointer to a buffer containing the payload bytes.  Only the number of bytes expected by the destination are copied.  */
    ) {
    struct trKIDS_Msg_GivSend_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Msg_Giv_rq, destination);
    my_rq.kmgs_flags = flags;
    my_rq.kmgs_payload_p = payload_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Receive from the calling task's (self) message queue; reschedule and maybe pre-empt.
*
* If there's a message waiting, serve's it up.
* Never copies-over more payload bytes than expected by the task.
*
* Reschedule, maybe pre-empt, and maybe run something else.
*
* A memcpy() will be used to copy the payload bytes.
*
* This should not be called from an explicit interrupt.
* Doing so will yield an error.
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Msg_recv(uint16_t * source_p,
                             /**< The source task ID for the message received.  */
                        uint16_t * flags_p,
                            /**< Flags sent with the message.  */
                        uint8_t * payload_p
                            /**< Pointer to a buffer where the payload will be placed.  Only the number of bytes expected by the source are copied.  */
    )
{
    struct trKIDS_Msg_TakRecv_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Msg_Recv_rq, trAPI_TaskID_self);
    my_rq.kmtr_source_p = source_p;
    my_rq.kmtr_flags_p = flags_p;
    my_rq.kmtr_payload_p = payload_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Take from the calling task's (self) message queue without pre-empting.
*
* Just like receive, but this will not cause a task switch; 
* the calling task retains control.
* Rescheduling will still occur.
*
* This should not be called from an explicit interrupt.
* Doing so will yield an error.
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Msg_tak(uint16_t * source_p,
                             /**< The source task ID for the message received.  */
                       uint16_t * flags_p,
                            /**< Flags sent with the message.  */
                       uint8_t * payload_p
                            /**< Pointer to a buffer where the payload will be placed.  Only the number of bytes expected by the source are copied.  */
    )
{
    struct trKIDS_Msg_TakRecv_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Msg_Tak_rq, trAPI_TaskID_self);
    my_rq.kmtr_source_p = source_p;
    my_rq.kmtr_flags_p = flags_p;
    my_rq.kmtr_payload_p = payload_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Report event status.
*
* This does not reschedule or task switch.
*
* All the action takes place on the calling task's thread.
*
* No sync is done; the data may be stale or changing.
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Evt_status(uint16_t target,
                          /**< Task ID for the event queue being stat-ed. */
                          struct trAPIRet_EvtStatus_s * stat_p
                                             /**< Where to put the status info. */
    )
{
    struct trKIDS_Evt_Status_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Evt_Status_rq, target);
    my_rq.kes_stat_p = stat_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Post an event to a task; reschedule and maybe pre-empt after doing so.
*
* If there's room for the event, send it on.
* Otherwise, serve up an error saying there's no room.
*
* Rescheduling and task switching may occur.
*
* The event can be anything you want it to be.
*
* This should not be called from an explicit interrupt.
* Doing so will yield an error.
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Evt_post(uint16_t destination,
                               /**< The destination task ID for the event posting.  Self is a valid ID. */
                        uint16_t flags,
                         /**< Not presently used; set it to zero. */
                        uint32_t event
                        /**< 32 bits of whatever you want. */
    ) {
    struct trKIDS_Evt_StfPost_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Evt_Post_rq, destination);
    my_rq.kesp_flags = flags;
    my_rq.kesp_event = event;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Stuff an event into a task's event queue without pre-empting.
*
* Just like post, but this will not cause a task switch;
* the calling task retains control.
* Rescheduling will still occur.
*
* This may be called from an explicit interrupt,
* but doing so with task ID self or explicit interrupt is an error.
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Evt_stf(uint16_t destination,
                               /**< The destination task ID for the event posting.  Self is a valid ID. */
                       uint16_t flags,
                         /**< Not presently used; set it to zero. */
                       uint32_t event
                        /**< 32 bits of whatever you want. */
    ) {
    struct trKIDS_Evt_StfPost_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Evt_Stf_rq, destination);
    my_rq.kesp_flags = flags;
    my_rq.kesp_event = event;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Pend for an event from the calling task's (self) event queue; reschedule and maybe pre-empt.
*
* If there's an event waiting, serves it up.
*
* Reschedule, maybe pre-empt, and maybe run something else.
*
* The event is whatever you want it to be.
*
* This should not be called from an explicit interrupt.
* Doing so will yield an error.
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Evt_pend(uint16_t * source_p,
                             /**< Task ID of the source for the event. */
                        uint16_t * flags_p,
                            /**< Flags sent with the event.  */
                        uint32_t * event_p
                           /**< 32 bits of whatever you can understand. */
    )
{
    struct trKIDS_Evt_GrbPend_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Evt_Pend_rq, trAPI_TaskID_self);
    my_rq.kegp_source_p = source_p;
    my_rq.kegp_flags_p = flags_p;
    my_rq.kegp_event_p = event_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Grab an event from the calling task's (self) event queue without pre-empting.
*
* Just like pend, but this will not cause a task switch; 
* the calling task retains control.
* Rescheduling will still occur.
*
* This should not be called from an explicit interrupt.
* Doing so will yield an error.
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Evt_grb(uint16_t * source_p,
                             /**< Task ID of the source for the event. */
                       uint16_t * flags_p,
                            /**< Flags sent with the event.  */
                       uint32_t * event_p
                           /**< 32 bits of whatever you can understand. */
    )
{
    struct trKIDS_Evt_GrbPend_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Evt_Grb_rq, trAPI_TaskID_self);
    my_rq.kegp_source_p = source_p;
    my_rq.kegp_flags_p = flags_p;
    my_rq.kegp_event_p = event_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Report task status.
*
* This does not reschedule or task switch.
*
* All the action takes place on the calling task's thread.
*
* No sync is done; the data may be stale or changing.
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Task_status(uint16_t target,
                          /**< Task ID for the task being stat-ed. */
                           struct trAPIRet_TaskStatus_s * stat_p
                                              /**< Where to put the status info. */
    )
{
    struct trKIDS_Task_Status_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Task_Status_rq, target);
    my_rq.kts_stat_p = stat_p;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}

/** Make the target task delay.
*
* Self, zero, and max are valid; see \link Spec07_TaskControl \endlink
*
*
*
* \retval trAPIRet_OK or a failure from trAPIRet.h.
*/
uint16_t trAPI_Task_delay(uint16_t target,
                          /**< Task ID of delay's target. */
                          uint16_t how_long
                           /**< How long in SClk ticks. */
    )
{
    struct trKIDS_Task_Delay_s my_rq;
    trAPI_CP_gather((void *)&my_rq, trKIDS_Task_Delay_rq, target);
    my_rq.ktd_how_long = how_long;
    if (trTti_enter((void *)&my_rq) > 0) {
        trTask_Tti((void *)&my_rq);
    }

    return my_rq.cp.cp_ret_val;
}
