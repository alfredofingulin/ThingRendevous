/** \file
*   \brief ThingRendevous's Application Program Interface.
*
* Just what it says it is.  
* Return type and value delegated to trAPIRet.h.
*
* Note explicit interrupt enter/leave are not defined here;
* use trAPIEI.h.
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

#ifndef trAPI_h
#define trAPI_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "trAPIRet.h"

#define trAPI_TaskID_self 0
#define trAPI_TaskID_none 0xFFFFU

#define trAPI_TaskID_first 0x0100U
#define trAPI_TaskID_last 0xFFFEU

#define trAPI_TaskPri_highest 0x0100U
#define trAPI_TaskPri_lowest 0xFFFEU

#define trAPI_TaskApp_Init_ID 0x00FFU
#define trAPI_TaskApp_Init_Pri 0x00FFU

uint16_t trAPI_start(void);

uint16_t trAPI_SClk_tick(void);

uint16_t trAPI_Strm_status(uint16_t target, struct trAPIRet_StrmStatus_s *stat_p);
uint16_t trAPI_Strm_writ(uint16_t destination,
                         uint16_t min_bytes,
                         uint16_t max_bytes, uint8_t * buff_p, uint16_t * actual_bytes_p, uint16_t * avail_bytes_p);
uint16_t trAPI_Strm_put(uint16_t destination,
                        uint16_t min_bytes,
                        uint16_t max_bytes, uint8_t * buff_p, uint16_t * actual_bytes_p, uint16_t * avail_bytes_p);
uint16_t trAPI_Strm_read(uint16_t min_bytes,
                         uint16_t max_bytes, uint8_t * buff_p, uint16_t * actual_bytes_p, uint16_t * avail_bytes_p);
uint16_t trAPI_Strm_get(uint16_t min_bytes,
                        uint16_t max_bytes, uint8_t * buff_p, uint16_t * actual_bytes_p, uint16_t * avail_bytes_p);

#define trAPI_Flags_Broadcast_bit 0x0001
#define trAPI_Flags_WasBroadcast(x) ( ( x & trAPI_Flags_Broadcast_bit ) == trAPI_Flags_Broadcast_bit )
#define trAPI_Flags_WasMonocast(x) ( ( x & trAPI_Flags_Broadcast_bit ) == 0 )

uint16_t trAPI_Msg_status(uint16_t target, struct trAPIRet_MsgStatus_s *stat_p);
uint16_t trAPI_Msg_send(uint16_t destination, uint16_t flags, uint8_t * payload_p);
uint16_t trAPI_Msg_giv(uint16_t destination, uint16_t flags, uint8_t * payload_p);
uint16_t trAPI_Msg_recv(uint16_t * source_p, uint16_t * flags_p, uint8_t * payload_p);
uint16_t trAPI_Msg_tak(uint16_t * source_p, uint16_t * flags_p, uint8_t * payload_p);

uint16_t trAPI_Evt_status(uint16_t target, struct trAPIRet_EvtStatus_s *stat_p);
uint16_t trAPI_Evt_post(uint16_t destination, uint16_t flags, uint32_t event);
uint16_t trAPI_Evt_stf(uint16_t destination, uint16_t flags, uint32_t event);
uint16_t trAPI_Evt_pend(uint16_t * source_p, uint16_t * flags_p, uint32_t * event_p);
uint16_t trAPI_Evt_grb(uint16_t * source_p, uint16_t * flags_p, uint32_t * event_p);

uint16_t trAPI_Task_status(uint16_t target, struct trAPIRet_TaskStatus_s *stat_p);
uint16_t trAPI_Task_delay(uint16_t target, uint16_t how_long);

#endif
