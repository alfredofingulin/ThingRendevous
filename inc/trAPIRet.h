/** \file
*   \brief ThingRendevous's Application Program Interface Return data values.
*
* A companion to trAPI.h, this defines all the possible data and return values.
* Done in a seperate header 'cause some tr internals just need to know the return 
* value and not the whole API.  
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

#ifndef trAPIRet_h
#define trAPIRet_h

#include <stdint.h>
#include <stdbool.h>

#define trAPIRet_OK 0

#define trAPIRet_WTF 0x0001U
#define trAPIRet_todo 0x0002U

// 2...0xFF9FU unused

// 0xFFA0U...0xFFADU unused  trAPIRet_Evt_

#define trAPIRet_Evt_Nothing2Grb 0xFFAEU
#define trAPIRet_Evt_NoRoom2PostOrStf 0xFFAFU

// 0xFFB0U...0xFFBDU unused  trAPIRet_Msg_

#define trAPIRet_Msg_Nothing2Tak 0xFFBEU
#define trAPIRet_Msg_NoRoom2SendOrGiv 0xFFBFU

// 0xFFC0U...0xFFCDU unused  trAPIRet_Strm_

#define trAPIRet_Strm_Nothing2Get 0xFFCEU
#define trAPIRet_Strm_NoRoom2WritOrPut 0xFFCFU

// 0xFFD0U...0xFFDFU unused  trAPIRet_Task_

#define trAPIRet_Usr_NoSuchID 0xFFE0U

// 0xFFE1U...0xFFEBU unused  trAPIRet_Usr_

#define trAPIRet_Usr_CallNotValid 0xFFECU
#define trAPIRet_Usr_ValueNotValid 0xFFEDU
#define trAPIRet_Usr_NotValidAtThisTime 0xFFEEU
#define trAPIRet_Usr_NotSupported 0xFFEFU

// 0xFFF0U...0XFFFCU unused  trAPIRet_Gen_

#define trAPIRet_Gen_NoReturnExpected 0xFFFDU
#define trAPIRet_Gen_InProcess 0xFFFEU

#define trAPIRet_Undefined 0xFFFFU

/** The data returned by a event status call.
*/
struct trAPIRet_EvtStatus_s {
    uint16_t res_task_ID;      /**< The event queue's unique task ID number. */
    uint16_t res_max_len;      /**< Max number of event packets. */
    uint16_t res_len_used;      /**< Event packets in the queue at this moment. */
};

/** The data returned by a message status call.
*/
struct trAPIRet_MsgStatus_s {
    uint16_t rms_task_ID;      /**< The message queue's unique task ID number. */
    uint16_t rms_cfg;      /**< Config info; priority vs FIFO. */
    uint16_t rms_payload_size;      /**< Number of bytes in every message payload. */
    uint16_t rms_max_len;      /**< Max message queue length in messages. */
    uint16_t rms_len_used;      /**< Messages in the queue at this moment. */
};

/** The data returned by a stream status call.
*/
struct trAPIRet_StrmStatus_s {
    uint16_t rss_task_ID;      /**< The stream's unique task ID number. */
    uint16_t rss_max_len;      /**< Max stream length in bytes. */
    uint16_t rss_len_used;      /**< Stream length used at this moment. */
    uint16_t rss_offset_read;      /**< Zero-based offset in the stream where data will be read from. */
    uint16_t rss_offset_writ;      /**< Zero-based offset in the stream where data will be written to. */
};

/** The data returned by a task status call.
*/
struct trAPIRet_TaskStatus_s {
    uint16_t rts_task_ID;      /**< The task's unique ID number. */
    uint16_t rts_nominal_priority;      /**< The nominal priority at which the task runs.  Remember: Low values indicate high priority and high values indicate low priority.  That is: priority 17 runs before priority 177. */
    int16_t rts_priority_offset;      /**< Any offset to the nominal priority.  0 means none; task is at nominal priority.  Negative values indicate the task's priority has been raised by this amount (remember: lower number means higher priority).  Positive values should never occur. */
    uint16_t rts_task_delay_remaining;      /**< If the task is blocked by another task and a timeout is pending, how long remains.  Always in units of system clock ticks.  In any other case, the value is stale and has no meaning. */
};

#endif
