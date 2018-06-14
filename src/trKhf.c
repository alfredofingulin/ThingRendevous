/** \file
*   \brief ThingRendevous's Kernel Handler Finder implementation.
*
* Helps find a specific kernel handler, based on the expected context for the handler.
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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "trKIDS.h"
#include "trStrm.h"
#include "trMsg.h"
#include "trEvt.h"
#include "trT.h"
#include "trSClk.h"
#include "trKhf.h"

/** Encapsulates info about kernel handler functions.
*
* Note all the const.  We expect this to be stored in .rodata section, not .data or .bss or .init.
*
* Uses a struct 'cause we may want to add things to this in the future.
*/
struct trKhf_Info_s {
    const uint16_t ki_flags;      /**< Bit field flags for encoding special cases. */
    void (*const ki_func_hdlr) (void *aKIDS_p);           /**< The kernel handler function itself. */
};

/** A table of kernel handler info and functions.
*
* Note that if some features are less than or equal to zero,
* then a not implemented handler will be compiled.
*
* This is indexed by the trKIDS.h request code.
* Note it gets the array length from trKIDS.h too.
*
* Note further the initialization of this array.
* We expect this to be stored in a flash section, like .rodata.
* We sure as hell don't want it in .data or .bss or .init or anything like that.
*/
const struct trKhf_Info_s trKhf_Info[trKIDS_rqlst_len] = {
    {                           // 0
     0,
     NULL                       // Start_start is called direct.
     },
    {                           // 1
     0,
     trSClk_sclk},
    {                           // 2
     (trKhf_Flags_RunOnCaller | trKhf_Flags_DontPreEmpt),
     trStrm_status},
    {                           // 3
     0,
     trStrm_writ},
    {                           // 4
     trKhf_Flags_DontPreEmpt,
     trStrm_put},
    {                           // 5
     0,
     trStrm_read},
    {                           // 6
     trKhf_Flags_DontPreEmpt,
     trStrm_get},
    {                           // 7
     (trKhf_Flags_RunOnCaller | trKhf_Flags_DontPreEmpt),
     trMsg_status},
    {                           // 8
     0,
     trMsg_send},
    {                           // 9
     trKhf_Flags_DontPreEmpt,
     trMsg_giv},
    {                           // 10
     0,
     trMsg_recv},
    {                           // 11
     trKhf_Flags_DontPreEmpt,
     trMsg_tak},
    {                           // 12
     (trKhf_Flags_RunOnCaller | trKhf_Flags_DontPreEmpt),
     trEvt_status},
    {                           // 13
     0,
     trEvt_post},
    {                           // 14
     trKhf_Flags_DontPreEmpt,
     trEvt_stf},
    {                           // 15
     0,
     trEvt_pend},
    {                           // 16
     trKhf_Flags_DontPreEmpt,
     trEvt_grb},
    {                           // 17
     (trKhf_Flags_RunOnCaller | trKhf_Flags_DontPreEmpt),
     trT_status},
    {                           // 18
     0,
     trT_delay},
    {                           // 19
     0,
     NULL                       // kernel task itself enter handler used directly
     },
    {                           // 20
     0,
     NULL                       // kernel task itself leave handler used directly
     },
    {                           // 21
     0,
     NULL                       // explicit interrupt enter handler used directly
     },
    {                           // 22
     0,
     NULL                       // explicit interrupt leave handler used directly
     },
    {                           // 23
     0,
     NULL                       // private section handler used directly
     }
};

/** Finds/serves-up the kernel handler for the request specified.
*
* Range check the request, get the handler from the table.
*
*/
void trKhf_find(void *aKIDS_p,
                       /**< Must be one of the data structures defined in trKIDS.h. */
                void (**the_kernel_func_hdlr) (void *aKIDS_p),
                                                             /**< Where to put the kernel function handler. */
                uint16_t * the_kernel_func_hdlrFlags_p
                                               /**< Where to put kernel function flags.  */
    )
{
    uint16_t idx = trKIDS_ComParams_aKIDS_p_AntiAlias().cp_request;
    void (*temp_KFH) (void *aKIDS_p);
    uint16_t temp_KFH_flags;

    if (idx < trKIDS_rqlst_len) {
        temp_KFH = trKhf_Info[idx].ki_func_hdlr;
        temp_KFH_flags = trKhf_Info[idx].ki_flags;
    } else {
        temp_KFH = NULL;
        temp_KFH_flags = 0;
    }

    if (the_kernel_func_hdlr != NULL) {
        *the_kernel_func_hdlr = temp_KFH;
    }
    if (the_kernel_func_hdlrFlags_p != NULL) {
        *the_kernel_func_hdlrFlags_p = temp_KFH_flags;
    }
}
